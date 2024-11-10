import pandas as pd
import numpy as np
df = pd.read_csv('log.csv')
t = df.Timestamp.to_numpy()
t = (t - t[0]) / 1e6
f = 1 / np.mean(np.diff(t))
print(f"Frequency: {f:.2f} Hz")
