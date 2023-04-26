import serial
import numpy as np
import pyqtgraph as pg
from pyqtgraph.Qt import QtGui, QtCore
from brainflow.data_filter import DataFilter, FilterTypes, DetrendOperations


class Graph:
    def __init__(self, width=800, height=600):
        self.eeg_channels = 6
        self.sampling_rate = 250
        self.ser = serial.Serial('/dev/ttyACM0', 115200)
        self.width = width
        self.height = height
        self.curves = []
        self.app = None
        self.win = None
        self.update_speed_ms = 50
        self.window = 2
        self.buffer = [[0, 0, 0, 0, 0, 0] for _ in range(self.sampling_rate * self.window)]

    def initialize(self):
        plots = []
        self.curves = []

        for i in range(self.eeg_channels):
            p = self.win.addPlot(row=i, col=0)
            p.setMenuEnabled('left', False)
            p.setMenuEnabled('bottom', False)

            if i == 0:
                p.setTitle('TimeSeries Plot')
            plots.append(p)
            curve = p.plot()
            self.curves.append(curve)

    def start(self):
        self.app = QtGui.QApplication([])
        self.win = pg.GraphicsWindow(title='BrainFlow Plot', size=(self.width, self.height))

        self.initialize()

        timer = QtCore.QTimer()
        timer.timeout.connect(self.update)
        timer.start(self.update_speed_ms)
        QtGui.QApplication.instance().exec_()

    def get_data(self):
        try:
            data = list(map(float, self.ser.readline().decode().split(' ')[:-1]))
            print(f'{data[-3]:08b}'[4:] + f'{data[-2]:08b}'[:2])
            data = data[:-3]
            if len(data) == self.eeg_channels:
                self.buffer.append(data)
            if len(self.buffer) > self.sampling_rate * self.window:
                self.buffer = self.buffer[(-self.sampling_rate * self.window):]
        except ValueError:
            pass

        return np.asarray(self.buffer).T

    def update(self):
        data = self.get_data()

        for channel in range(self.eeg_channels):
            # DataFilter.detrend(data[channel], DetrendOperations.CONSTANT.value)
            # DataFilter.perform_bandpass(data[channel], self.sampling_rate, 51.0, 100.0, 2,
            #                             FilterTypes.BUTTERWORTH.value, 0)
            # DataFilter.perform_bandpass(data[channel], self.sampling_rate, 51.0, 100.0, 2,
            #                             FilterTypes.BUTTERWORTH.value, 0)
            # DataFilter.perform_bandstop(data[channel], self.sampling_rate, 50.0, 4.0, 2,
            #                             FilterTypes.BUTTERWORTH.value, 0)
            # DataFilter.perform_bandstop(data[channel], self.sampling_rate, 60.0, 4.0, 2,
            #                             FilterTypes.BUTTERWORTH.value, 0)
            self.curves[channel].setData(data[channel].tolist())

        self.app.processEvents()
