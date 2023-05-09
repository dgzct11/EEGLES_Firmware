import serial
import numpy as np
import pyqtgraph as pg
from pyqtgraph.Qt import QtGui, QtCore
from brainflow.data_filter import DataFilter, FilterTypes, DetrendOperations
from scipy.fft import fft, fftfreq
from scipy.signal import find_peaks

class Graph:
    def __init__(self, width=800, height=600):
        self.eeg_channels = 6
        self.sampling_rate = 250
        self.ser = serial.Serial('COM4', 115200)
        self.width = width
        self.height = height
        self.curves = []
        self.app = None
        self.win = None
        self.update_speed_ms = 50
        self.window = 100
        self.buffer = [[0, 0, 0, 0, 0, 0] for _ in range(self.sampling_rate * self.window)]
        self.timestep = 1/250
        
        self.freqs = fftfreq(int(self.window/self.timestep), d=self.timestep)
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

        for i in range(self.eeg_channels):
            p = self.win.addPlot(row=i, col=1)
            p.setMenuEnabled('left', False)
            p.setMenuEnabled('bottom', False)

            if i == 0:
                p.setTitle('Fourier Plot')
            plots.append(p)
            curve = p.plot()
            self.curves.append(curve)

    def start(self):
        self.app = QtGui.QApplication([])
        self.win = pg.GraphicsWindow(title='BrainFlow Plot', size=(self.width*2, self.height*2))

        self.initialize()

        timer = QtCore.QTimer()
        timer.timeout.connect(self.update)
        timer.start(self.update_speed_ms)
        QtGui.QApplication.instance().exec_()
        

    def get_data(self):
        try:
            
            data = list(map(float, self.ser.readline().decode().rstrip().rstrip().split(' ')))
            #print(data)
            #print(f'{int(data[-3]):08b}'[4:] + f'{int(data[-2]):08b}'[:2])
            data = data[:-3]
            if len(data) == self.eeg_channels:
                self.buffer.append(data)
            if len(self.buffer) > self.sampling_rate * self.window:
                self.buffer = self.buffer[(-self.sampling_rate * self.window):]
                
        except ValueError:
            print("error")
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
        for channel in range(self.eeg_channels, 2*self.eeg_channels):
            d_fft = fft(data[channel - self.eeg_channels])
            #print(self.freqs[np.argmax(d_fft)])
            ind = np.argpartition(d_fft, -5)[-5:]
            print(self.freqs[ind])
            print(self.freqs[1])
            self.curves[channel].setData(self.freqs, np.abs(d_fft))

        self.app.processEvents()
