from pathlib import Path

import sys
build_path = Path(__file__).absolute().parent / 'build'
sys.path.append(str(build_path / 'release'))
sys.path.append(str(build_path / 'debug'))
import pid_tuning

import random
import math
import matplotlib.pyplot as plt
import numpy as np

target_humidity = 50
current_humidity = 30
humidity_natural_decay_rate = lambda t: 1 + 0.01 * math.sin(2 * math.pi * t / (60 * 24)) + random.gauss(0, 0.05)
humidity_gain_by_fan_speed = 1.08 / 100

time = np.arange(0, 60 * 72)
fan_speed = np.zeros(time.shape[0])
humidity = np.zeros(time.shape[0])
for t in time:
    humidity[t] = current_humidity
    speed = round(pid_tuning.get_output(round(current_humidity), target_humidity), 2)
    fan_speed[t] = speed
    current_humidity -= humidity_natural_decay_rate(t)
    current_humidity += speed * humidity_gain_by_fan_speed

plt.plot(time, fan_speed, label='fan speed')
plt.plot(time, humidity, label='humidity')
plt.ylim(0, 100)
plt.xlabel("time")
plt.ylabel("%")
plt.title("PID fan speed")
plt.show()
