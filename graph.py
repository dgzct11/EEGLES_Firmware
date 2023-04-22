import serial
import numpy as np
import pyqtgraph as pg
from pyqtgraph.Qt import QtGui, QtCore
from brainflow.data_filter import DataFilter, FilterTypes, DetrendOperations#, WindowFunctions


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
        data = []
        for i in range(self.sampling_rate * 2):
            data.append(list(map(float, self.ser.readline().decode().split(' ')[:-1])))
        data = np.asarray(data)
        print(data.T)
        return np.asarray(data).T

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
