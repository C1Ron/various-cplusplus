import sys
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.animation as animation

fig, (ax1,ax2,ax3) = plt.subplots(3,1)
df = pd.read_csv(sys.argv[1], sep = ',', index_col=False)
t = df.Timestamp.to_numpy()
t = (t - t[0])/1e6
try:
    speed = df['speed-meas'].to_numpy()
    speedRef = df['speed-ref'].to_numpy()
    torque = df['torque-meas'].to_numpy()
    torqueRef = df['torque-ref'].to_numpy()
    flux = df['flux-meas'].to_numpy()
    fluxRef = df['flux-ref'].to_numpy()
    ax1.plot(t, speed, t, speedRef)
    ax2.plot(t, torque, t, torqueRef)
    ax3.plot(t, flux, t, fluxRef)
    plt.show()
except Exception as e:
    print(f"There is no {e}")

