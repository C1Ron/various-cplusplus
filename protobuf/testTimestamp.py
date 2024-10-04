from datetime import datetime
ts = 175863600
dt = datetime.fromtimestamp(ts)
st = dt.strftime("%d'th %B, %Y - %H:%M:%S")
print(f"{st}")