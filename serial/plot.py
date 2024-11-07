import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import time
import datetime
import matplotlib.animation as animation

fig, (ax1,ax2) = plt.subplots(2,1, sharex = True)
sx = 200
def animateFunc(i):
        
    df = pd.read_csv('log.csv', sep = ',', index_col=False)
    t = df.Timestamp.to_numpy()
    t = (t - t[0])/1e6
    try:
        speed = df['SpeedRef'].to_numpy()
        torque = df['TorqueRef'].to_numpy()
        ax1.clear()
        ax2.clear()
        ax1.set_ylim([-2000, 2000])
        ax2.set_ylim([-25000, 25000])
        ax1.plot(t[-sx:], speed[-sx:], '-o')
        ax2.plot(t[-sx:], torque[-sx:], '-o')
    except Exception as e:
        print(f"There is no {e}")

if __name__ == '__main__':
    try:
        ani = animation.FuncAnimation(fig, animateFunc, interval=10)
        plt.show()
    except Exception as e:
        print(f"Error: {e}")
    except KeyboardInterrupt:
        print("Interrupted")
        plt.close()
        pass