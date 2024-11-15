import sys
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.animation as animation

fig, (ax1,ax2) = plt.subplots(2,1)
df = pd.read_csv(sys.argv[1], sep = ',', index_col=False)
t = df.Timestamp.to_numpy()
t = (t - t[0])/1e6
try:
    speed = df['speed-ref'].to_numpy()
    torque = df['torque-ref'].to_numpy()
    ax1.plot(t, speed)
    ax2.plot(t, torque)
    plt.show()
except Exception as e:
    print(f"There is no {e}")

