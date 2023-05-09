import numpy as np
import matplotlib.pyplot as plt
from scipy.fft import fft, fftfreq
import serial



        
# generate some test data
time = 100
N = 250*time
T = 1 / 250 # sampling period (44100 Hz is the standard audio sampling rate)
x = np.linspace(0, N*T, N)
y = np.sin(2*np.pi*440*x) + 0.5*np.sin(2*np.pi*880*x)

# compute the FFT
y_fft = fft(y)

# compute the frequencies
freqs = fftfreq(N, d=T)
print(np.abs(y_fft))
# plot the results
fig, ax = plt.subplots()
ax.plot(freqs, np.abs(y_fft))
ax.set_xlabel('Frequency (Hz)')
ax.set_ylabel('Magnitude')
plt.show()
