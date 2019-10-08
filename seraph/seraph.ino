/**
 * Seraph is a virtual airbrake rocket used for experimental controls testing.
 * Its geometry and specifications are identical to that of Torchy, LRA's SAC
 * 2018 rocket whose wings were so violently clipped.
 *
 * The vehicle is defined across several files:
 *   seraph.ino      - Flight computer logic
 *   seraph_cdp.dat  - Raw Cd plane data
 *   seraph_cdp.h    - Aimbot Cd plane definition generated from seraph_cdp.dat
 *   seraph_config.h - Vehicle specifications and airbrake controller config
 */

#define FF // Use to toggle simulation in Flight Factory

#ifdef FF
  #include <string>
  #include "ff_arduino_harness.hpp"
#endif

#ifdef FF
  #define TELEM(data, ...) {                                                   \
    ff::out("[#b$mseraph#r] ");                                                \
    ff::out(data, ##__VA_ARGS__);                                              \
    ff::out("\n");                                                             \
  }
#else
  #define TELEM(data, ...) {                                                   \
    Serial.printf(data, ##__VA_ARGS__);                                        \
    Serial.printf("\n");                                                       \
  }
#endif

#define KG_PRECOMP_DEPTH 1

#define CURRENT_ALTITUDE g_state[0][0]
#define CURRENT_VELOCITY g_state[1][0]
#define CURRENT_ACCEL    g_state[2][0]

#include "aimbot.hpp"
#include "airflow_deflection_airbrake_model.hpp"
#include "photic.h"
#include "planar_cd_model.hpp"
#include "seraph_config.h"

const photic::matrix g_INITIAL_STATE(3, 1, 1293.876, 0, 0);

photic::Imu* g_imu = nullptr;
photic::Barometer* g_barometer = nullptr;
photic::TelemetryHeap *g_heap = nullptr;

photic::Metronome g_mtr_estimator(10);
photic::KalmanFilter* g_estimator;
photic::matrix g_state = g_INITIAL_STATE;

photic::history<float> g_hist_accel_z(10);
photic::history<float> g_hist_vel_z(10);
photic::history<float> g_hist_alt(10);

aimbot::CdModel* g_cd_model = nullptr;
aimbot::AirbrakeModel* g_airbrake_model = nullptr;
aimbot::Engine* g_aimbot = nullptr;
aimbot::rocket_t g_rocket = seraph_vehicle::rocket();

bool g_liftoff = false;
bool g_burnout = false;
bool g_apogee = false;

const float g_AIRFLOW_DEFLECTION_RHO = 0.75;

/*******************************************************************************
 * SUPPORTING FUNCTIONS
 ******************************************************************************/

/**
 * Updates all sensors and adds to relevant histories.
 */
void read_sensors() {
  g_imu->update();
  g_barometer->update();
  g_hist_accel_z.add(g_imu->get_acc_z());
}

/*******************************************************************************
 * SETUP
 ******************************************************************************/

void setup() {
  TELEM("In startup...");

  // Clean up previous flights (FF only)
#ifdef FF
  CLEANUP(g_imu);
  CLEANUP(g_barometer);
  CLEANUP(g_heap);
  CLEANUP(g_cd_model);
  CLEANUP(g_airbrake_model);
  CLEANUP(g_aimbot);
  CLEANUP(g_estimator);

  g_liftoff = false;
  g_burnout = false;
  g_apogee = false;

  g_mtr_estimator.reset();

  g_hist_accel_z.clear();
  g_hist_vel_z.clear();
  g_hist_alt.clear();

  g_state = g_INITIAL_STATE;
#endif

  // Telemetry streams init (FF only)
  #ifdef FF
    ff::topen("kf_altitude");
    ff::topen("kf_velocity");
    ff::topen("kf_accel");
    ff::topen("brake_ext");
    ff::topen("kf_alt_err");

    SIM["airbrake_extension"] = 0.0;
  #endif

  // IMU init
  g_imu =
  #ifndef FF
    new TorchyImu();
  #else
    new VirtualImu();
  #endif

  // Barometer init
  g_barometer =
  #ifndef FF
    new TorchyBarometer();
  #else
    new VirtualBarometer();
  #endif

  TELEM("#b$g● DAQ GO");

  // Photic init
#ifdef FF
  photic::config(ROCKET_TIMEKEEPER, new VirtualTimekeeper());
#else
  photic::config(ROCKET_TIMEKEEPER, new photic::ArduinoTimekeeper());
  photic::config(ROCKET_MICROCONTROLLER_MODEL, photic::TEENSY_31);
#endif

  // Ignition detection
  photic::config(ROCKET_IGNITION_G_TRIGGER, 3.0);
  photic::config(ROCKET_NO_IGNITION_GRACE_PERIOD,
  #ifdef FF
    0.0
  #else
    60.0 * 10
  #endif
  );

  // Burnout detection
  photic::config(ROCKET_TRIGGER_BURNOUT_ON_NEG_ACCEL, true);
  photic::config(ROCKET_TRIGGER_BURNOUT_ON_NEG_ACCEL_NEGL, 0.0);
  photic::config(ROCKET_TRIGGER_BURNOUT_ON_TIMEOUT, true);
  photic::config(ROCKET_TRIGGER_BURNOUT_ON_TIMEOUT_VAL, 7.0);

  // Apogee detection
  photic::config(ROCKET_TRIGGER_APOGEE_ON_NEG_VEL, true);
  photic::config(ROCKET_TRIGGER_APOGEE_ON_NEG_VEL_NEGL, 1.0);
  photic::config(ROCKET_TRIGGER_APOGEE_ON_TIMEOUT, true);
  photic::config(ROCKET_TRIGGER_APOGEE_ON_TIMEOUT_VAL, 45.0);

  // Hardware
  photic::config(ROCKET_PRIMARY_IMU, g_imu);
  photic::config(ROCKET_PRIMARY_BAROMETER, g_barometer);
  photic::config(ROCKET_TELEMETRY_HEAP, g_heap);

  // Data and telemetry
  photic::config(ROCKET_VERTICAL_ACCEL_HISTORY, &g_hist_accel_z);
  photic::config(ROCKET_VERTICAL_VELOCITY_HISTORY, &g_hist_vel_z);
  photic::config(ROCKET_ALTITUDE_HISTORY, &g_hist_alt);

  TELEM("#b$g● FLIGHT CONTROLLER GO");

  // State estimator init
  g_estimator = new photic::KalmanFilter();
  g_estimator->set_delta_t(g_mtr_estimator.get_wavelength());
  g_estimator->set_sensor_variance(0.15, 0.25);
  g_estimator->set_initial_estimate(g_state[0][0], g_state[1][0],
                                    g_state[2][0]);
  g_estimator->compute_kg(KG_PRECOMP_DEPTH);

  // Aimbot init
  g_cd_model = new aimbot::PlanarCdModel(seraph_vehicle::cdp);
  g_airbrake_model = new aimbot::AirflowDeflectionAirbrakeModel(
    g_cd_model, g_AIRFLOW_DEFLECTION_RHO
  );
  aimbot::abc_config_t abc_conf = seraph_vehicle::abc_config();
  g_aimbot = new aimbot::Engine(
    g_cd_model,
    g_airbrake_model,
    aimbot::Engine::EXTRAP_EULER,
    0.01,
    abc_conf
  );

  TELEM("#b$g● GNC GO");

  TELEM("#b$g● TELEMETRY GO#r");

  TELEM("Entering wait sequence at t=%f", photic::rocket_time())
}

/*******************************************************************************
 * LOOP
 ******************************************************************************/

void loop() {
  read_sensors();

  float t_now = photic::rocket_time();

  // Gate 1 - liftoff check
  if (photic::check_for_liftoff()) {
    if (!g_liftoff) {
      TELEM("Liftoff detected at t=%f", t_now);
    }

    g_liftoff = true;
  } else
    return;

  // Filter vehicle state
  float a = g_imu->get_acc_z() * 9.81;
  if (g_mtr_estimator.poll(t_now)) {
    g_state = g_estimator->filter(g_barometer->get_altitude(), a);

  #ifdef FF
    // Log the filtered state
    ff::tout("kf_altitude", t_now, g_state[0][0]);
    ff::tout("kf_velocity", t_now, g_state[1][0]);
    ff::tout("kf_accel", t_now, g_state[2][0]);

    // Log the altitude error
    aimbot::state_t true_state = SIM.get_rocket_state();
    float alt_err = true_state.altitude - g_state[0][0];
    ff::tout("kf_alt_err", t_now, alt_err);
  #endif
  }

  // Gate 2 - burnout check
  if (photic::check_for_burnout()) {
    if (!g_burnout) {
      TELEM("Burnout detected at t=%f", t_now);
    }

    g_burnout = true;
  } else
    return;

  // Compute a new airbrake control moment
  aimbot::state_t aimbot_state = {CURRENT_ALTITUDE, CURRENT_VELOCITY};
  aimbot::Engine::step_t moment = g_aimbot->update(t_now, g_rocket,
                                                   aimbot_state);
#ifdef FF
  // Update virtual airbrake and log its position
  SIM["airbrake_extension"] = moment.extension;
  ff::tout("brake_ext", t_now, moment.extension);
#endif

  // Apogee check
  if (photic::check_for_apogee()) {
    if (!g_apogee) {
      TELEM("Apogee detected at t=%f", t_now);
    }

    g_apogee = true;
  }
}
