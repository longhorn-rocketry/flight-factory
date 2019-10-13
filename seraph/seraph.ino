/**
 * Seraph is a virtual airbrake rocket used for experimental controls testing.
 * Its geometry and specifications are identical to that of Torchy, LRA's SAC
 * 2018 rocket whose wings were so tragically clipped.
 *
 * The vehicle is defined across several files:
 *   seraph.ino      - Flight computer logic
 *   seraph_cdp.dat  - Raw Cd plane data
 *   seraph_cdp.h    - Aimbot Cd plane definition generated from seraph_cdp.dat
 *   seraph_config.h - Vehicle specifications and airbrake controller config
 */

#define FF // Use to toggle simulation in Flight Factory

#ifndef FF
  // STL redefinitions that Arduino can't build without
  namespace std {
    void __throw_bad_alloc() {}

    void __throw_length_error(char const *e) {}
  }
#endif

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

#define KG_PRECOMP_DEPTH       5
#define KF_IMU_VAR             0.25
#define KF_BARO_VAR            0.75

#define AIRFLOW_DEFLECTION_RHO seraph_vehicle::g_AIRFLOW_DEFLECTION_RHO
#define AIMBOT_EXTRAP_DT       0.01
#define AIMBOT_TIMEGATE        10

#define CURRENT_ALTITUDE       g_state[0][0]
#define CURRENT_VELOCITY       g_state[1][0]
#define CURRENT_ACCEL          g_state[2][0]

#define LAUNCHPAD_ALTITUDE     1293.876
#define P0_SAMPLE_SIZE         1000

#include "aimbot.hpp"
#include "airflow_deflection_airbrake_model.hpp"
#include "photic.h"
#include "planar_cd_model.hpp"
#include "seraph_config.h"

/*******************************************************************************
 * GLOBALS
 ******************************************************************************/

/**
 * Inert launchpad state.
 */
const photic::matrix g_INITIAL_STATE(3, 1, LAUNCHPAD_ALTITUDE, 0, 0);

/**
 * Sensors and actuators.
 */
photic::Imu* g_imu = nullptr;
photic::Barometer* g_barometer = nullptr;
photic::TelemetryHeap *g_heap = nullptr;

/**
 * State estimation.
 */
photic::Metronome g_mtr_estimator(10);
photic::KalmanFilter* g_estimator;
photic::matrix g_state = g_INITIAL_STATE;

/**
 * Telemetry history.
 */
photic::history<float> g_hist_accel_z(10);
photic::history<float> g_hist_vel_z(10);
photic::history<float> g_hist_alt(10);
photic::history<float> g_hist_pressure(10);

/**
 * Airbrake control and flight modeling.
 */
photic::Metronome g_mtr_airbrake(10);
aimbot::CdModel* g_cd_model = nullptr;
aimbot::AirbrakeModel* g_airbrake_model = nullptr;
aimbot::Engine* g_aimbot = nullptr;
aimbot::rocket_t g_rocket = seraph_vehicle::rocket();

/**
 * Flight event flags.
 */
bool g_liftoff = false;
bool g_burnout = false;
bool g_apogee = false;

/**
 * Launchpad pressure sampled during startup.
 */
float g_p0;

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
  g_hist_pressure.add(g_barometer->get_pressure());
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
  g_mtr_airbrake.reset();

  g_hist_accel_z.clear();
  g_hist_vel_z.clear();
  g_hist_alt.clear();
  g_hist_pressure.clear();

  g_state = g_INITIAL_STATE;
#endif

// Telemetry streams init (FF only)
#ifdef FF
  ff::topen("gt_altitude");
  ff::topen("gt_velocity");
  ff::topen("gt_accel");
  ff::topen("kf_altitude");
  ff::topen("kf_velocity");
  ff::topen("kf_accel");
  ff::topen("brake_ext");
  ff::topen("kf_alt_err");
  ff::topen("hypso_alt");
  ff::topen("hypso_alt_err");

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
  g_estimator->set_sensor_variance(KF_BARO_VAR, KF_IMU_VAR);
  g_estimator->set_initial_estimate(
    g_state[0][0], g_state[1][0], g_state[2][0]
  );
  g_estimator->compute_kg(KG_PRECOMP_DEPTH);

  // Aimbot init
  g_cd_model = new aimbot::PlanarCdModel(seraph_vehicle::cdp);
  g_airbrake_model = new aimbot::AirflowDeflectionAirbrakeModel(
    g_cd_model, AIRFLOW_DEFLECTION_RHO
  );
  aimbot::abc_config_t abc_conf = seraph_vehicle::abc_config();
  g_aimbot = new aimbot::Engine(
    g_cd_model,
    g_airbrake_model,
    aimbot::Engine::EXTRAP_EULER,
    AIMBOT_EXTRAP_DT,
    abc_conf
  );

  TELEM("#b$g● GNC GO");

  TELEM("#b$g● TELEMETRY GO#r");

  // Sample launchpad pressure
  for (unsigned int i = 0; i < P0_SAMPLE_SIZE; i++) {
    read_sensors();
    g_p0 += g_barometer->get_pressure();
  }
  g_p0 /= P0_SAMPLE_SIZE;

  TELEM("Entering wait sequence at t=%f", photic::rocket_time())
}

/*******************************************************************************
 * LOOP
 ******************************************************************************/

void loop() {
  // Fetch new sensor readings
  read_sensors();

  // GATE 1 - LIFTOFF CHECK. The majority of loop logic is gated by engine
  // ignition. Code preceding this statement will run while the rocket is inert
  // on the launchpad.
  if (photic::check_for_liftoff()) {
    if (!g_liftoff) {
      TELEM("Liftoff detected at t=%f", photic::rocket_time());
    }

    g_liftoff = true;
  } else
    return;

  // Get current mission time
  float t_now = photic::flight_time();

  // STATE ESTIMATION. An acceleration reading is sampled from the IMU and an
  // altitude estimate is made by applying the hypsometric formula to the
  // barometer's recorded temperature and a rolling average of its pressure.
  // These observations enter the Kalman filter and produce a refined estimate
  // of the rocket's state that is used for airbrake control.
  if (g_mtr_estimator.poll(t_now)) {
    float vertical_accel = g_imu->get_acc_z() * 9.81;
    float p_now = g_hist_pressure.mean();
    float temp = g_barometer->get_temperature();
    // Note that we add in the launchpad altitude because we need our world
    // position relative to sea level for purposes of atmosphere modeling,
    // whereas the hypsometric formula produces a position relative to ground
    // level.
    float world_alt = LAUNCHPAD_ALTITUDE + photic::hypso(g_p0, p_now, temp);
    g_state = g_estimator->filter(world_alt, vertical_accel);

  #ifdef FF
    // Log true state
    aimbot::state_t true_state = SIM.get_rocket_state();
    ff::tout("gt_altitude", t_now, true_state.altitude);
    ff::tout("gt_velocity", t_now, true_state.velocity);

    // Log filtered state
    ff::tout("kf_altitude", t_now, g_state[0][0]);
    ff::tout("kf_velocity", t_now, g_state[1][0]);
    ff::tout("kf_accel", t_now, g_state[2][0]);

    // Log error in the hypsometric altitude estimate
    ff::tout("hypso_alt", t_now, world_alt);
    ff::tout("hypso_alt_err", t_now, true_state.altitude - world_alt);

    // Log error in the Kalman filter's altitude estimate
    float alt_err = true_state.altitude - g_state[0][0];
    ff::tout("kf_alt_err", t_now, alt_err);
  #endif
  }

  // GATE 2 - BURNOUT CHECK. For reasons of stability, we prevent the airbrake
  // from activating during powered flight.
  if (photic::check_for_burnout()) {
    if (!g_burnout) {
      TELEM("Burnout detected at t=%f", t_now);
    }

    g_burnout = true;
  } else
    return;

  // AIRBRAKE CONTROL. While cruising, the active airbraking works to guide the
  // rocket to the target altitude. Until AIMBOT_TIMEGATE seconds after liftoff,
  // however, the airbrakes do not move. This is to prevent misinformed braking
  // while the state estimator recovers from the large error accumulated during
  // powered flight.
  if (!g_apogee &&
      t_now - photic::burnout_time() > AIMBOT_TIMEGATE &&
      g_mtr_airbrake.poll(t_now))
  {
    // Compute a new airbrake position
    aimbot::state_t aimbot_state = {CURRENT_ALTITUDE, CURRENT_VELOCITY};
    aimbot::Engine::step_t moment = g_aimbot->update(
      t_now, g_rocket, aimbot_state
    );
  #ifdef FF
    // Update virtual airbrake position
    SIM["airbrake_extension"] = moment.extension;
  #endif
  }

#ifdef FF
  // Log airbrake position
  ff::tout("brake_ext", t_now, SIM["airbrake_extension"]);
#endif

  // GATE 3 - APOGEE CHECK.
  if (photic::check_for_apogee()) {
    if (!g_apogee) {
      TELEM("Apogee detected at t=%f", t_now);
      // Retract airbrakes
    #ifdef FF
      SIM["airbrake_extension"] = 0;
    #endif
    }

    g_apogee = true;
  }
}
