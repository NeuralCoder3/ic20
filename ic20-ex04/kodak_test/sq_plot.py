import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D  
from math import log2

fig = plt.figure(figsize=(4, 3))
ax1 = fig.add_subplot(111, projection='3d')

x=[]
y=[]
top=[]

for l in open("all.txt").readlines()[1:]:
    vals=l.split(" ")
    s=int(vals[0])
    q=int(vals[1])
    s=log2(s)
    q=log2(q)
    ratio=float(vals[2])
    mse=float(vals[3])
    x.append(s)
    y.append(q)
    top.append(mse)

bottom = np.zeros_like(top)
width = depth = 1

ax1.bar3d(x, y, bottom, width, depth, top, shade=True)
ax1.set_title('S-Q-MSE')
ax1.set_xlabel('Log2(s)')
ax1.set_ylabel('Log2(q)')
ax1.set_zlabel('MSE')

plt.show()