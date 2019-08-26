import os
import sys


if len(sys.argv) != 2:
    print("Usage: python3 cdp_to_cpp.py {cdp file}")
    sys.exit(1)

with open(sys.argv[1], "r") as i:
    dir = os.path.dirname(sys.argv[1])
    name = os.path.splitext(os.path.basename(sys.argv[1]))[0]

    with open(os.path.join(dir, name + ".h"), "w") as o:
        alt_low = 0
        alt_high = 0
        alt_step = 0
        vel_low = 0
        vel_high = 0
        vel_step = 0
        arr = []

        for line in i.readlines():
            line = line.strip()
            if len(line) > 0:
                if not '=' in line:
                    arr.append(line)
                else:
                    pair = line.split('=')
                    if pair[0] == 's_low':
                        alt_low = float(pair[1])
                    elif pair[0] == 's_high':
                        alt_high = float(pair[1])
                    elif pair[0] == 's_step':
                        alt_step = float(pair[1])
                    elif pair[0] == 'v_low':
                        vel_low = float(pair[1])
                    elif pair[0] == 'v_high':
                        vel_high = float(pair[1])
                    elif pair[0] == 'v_step':
                        vel_step = float(pair[1])
                    else:
                        print("Warning: unknown key '%s'" % pair[0])

        header = [
            '#ifndef CD_PLANE_%s_H' % name,
            '#define CD_PLANE_%s_H' % name,
            '',
            '#include "cd_plane.hpp"',
            '',
            'float %s_vals[] = {' % name
        ]

        for line in header:
            o.write(line + "\n")

        for val in arr:
            o.write("\t%s,\n" % val)

        for line in i.readlines():
            line = line.strip()
            if len(line) > 0 and not '=' in line:
                o.write("\t%s,\n" % line)

        footer = [
            '};',
            '',
            'aimbot::cd_plane_t %s = {' % name,
            '\t%s_vals,' % name,
            '\t%s,' % alt_low,
            '\t%s,' % alt_high,
            '\t%s,' % alt_step,
            '\t%s,' % vel_low,
            '\t%s,' % vel_high,
            '\t%s' % vel_step,
            '};',
            '',
            '#endif'
        ]

        for line in footer:
            o.write(line + "\n")
