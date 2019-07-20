import matplotlib.pyplot as plt
import os
import sys


if len(sys.argv) < 2:
    sys.exit("Usage: python3 vis.py <dat_1> <dat_2> ... <dat_n>\n")

fig, ax = plt.subplots(1)

# Generate a plot of each specified dat
for i in range(1, len(sys.argv)):
    with open(sys.argv[i], 'r') as fin:
        set = fin.readlines()
        x = []
        y = []

        for item in set:
            point = [float(x) for x in item.strip().split(" ")]
            x.append(point[0])
            y.append(point[1])

        ax.plot(x, y)

fig.show()
ax.set_title(", ".join(sys.argv[1:]))
axes = plt.gca()
plt.show()
