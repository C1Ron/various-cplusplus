import sys
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Read the CSV file
df = pd.read_csv(sys.argv[1], sep=',', index_col=False)

# Extract time and normalize to start at zero
t = df.Timestamp.to_numpy()
time = (t - t[0]) / 1e6

try:
    # Extract speed and torque
    speed = df['speed-setpoint'].to_numpy()
    torque = df['torque-ref'].to_numpy()

    # Find points of abrupt change
    indx = np.where(np.abs(np.diff(speed)) > 10)[0]
    # Define segments by applying a buffer around each abrupt change point
    buffer = 5
    segs = []
    for i in range(len(indx) - 1):  # For each pair of consecutive indices
        start = indx[i] + buffer
        end = indx[i + 1] - buffer
        if start < end:  # Test for valid segment
            segs.append((start, end))

    # Display segs
    print("Segments (start:end):")
    for start, end in segs:
        print(f"{start}:{end}")

    # Calculate slopes for each segment
    slopes = []
    for start, end in segs:
        dt = time[end] - time[start]
        dy = torque[end] - torque[start]
        s = dy / dt
        slopes.append(s)

    print("Slopes for each segment:")
    for i, s in enumerate(slopes):
        print(f"Segment {i + 1}: {s:.2f}")

    # Plot results
    fig, (ax1, ax2) = plt.subplots(2, 1)

    ax1.plot(time, speed, label="Speed")
    ax2.plot(time, torque, label="Torque")

    for i, (start, end) in enumerate(segs):
        ax1.axvline(time[start], color="g", linestyle="--", alpha=0.5)
        ax1.axvline(time[end], color="r", linestyle="--", alpha=0.5)
        ax2.axvline(time[start], color="g", linestyle="--", alpha=0.5)
        ax2.axvline(time[end], color="r", linestyle="--", alpha=0.5)
        tp = (time[start] + time[end]) / 2
        yp = (torque[start] + torque[end]) / 2
        ax2.text(tp, yp, f"{slopes[i]:.2f}", fontsize=8, color="blue")

    ax1.legend(loc="upper left")
    ax2.legend(loc="upper left")
    plt.show()

except Exception as e:
    print(f"Error: {e}")
