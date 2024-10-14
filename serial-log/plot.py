import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv('log.csv')
t = df.time.to_numpy()
t = (t - t[0])/1e6
try:
    speed = df['reg-4'].to_numpy()
    torque = df['reg-8'].to_numpy()
    fig, (ax1, ax2) = plt.subplots(2, 1)
    ax1.plot(t, speed, '-o')
    ax2.plot(t, torque, '-o')
    plt.show()
except Exception as e:
    print(f"There is no {e}")