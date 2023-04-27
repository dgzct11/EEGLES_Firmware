import serial
import numpy as np
import matplotlib.pyplot as plt

WINDOW = 5 # seconds
SAMPLING_RATE = 250 # Hz

ser = serial.Serial('COM4', 115200)

data = []

for x in range(SAMPLING_RATE * WINDOW):
    data.append(list(map(float, ser.readline().decode().split(' ')[:-1])))

data = np.asarray(data).T

fig, axes = plt.subplots(6, 1)

