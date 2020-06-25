import matplotlib.pyplot as plt
plt.style.use('seaborn-whitegrid')
import numpy as np
from math import log2

fig = plt.figure()
ax = plt.axes()

rat=dict()
mse=dict()

for l in open("all.txt").readlines()[1:]:
    vals=l.split(" ")
    s=int(vals[0])
    q=int(vals[1])
    s=int(log2(s))
    q=int(log2(q))
    ratioV=float(vals[2])
    mseV=float(vals[3])
    if s not in rat:
        rat[s]=[]
        mse[s]=[]
    rat[s].append(ratioV)
    mse[s].append(mseV)

for x in rat.keys():
    plt.plot(rat[x], mse[x], label=f'var q, s={2**x}')


# x = np.linspace(0, 10, 1000)
# plt.plot(x, np.sin(x), label='sin')
# plt.plot(x, np.cos(x), label='cos')
# plt.plot(ratSQ0, mseSQ0, label='s (q=0)')
# plt.plot(ratSQ1, mseSQ1, label='s (q=1)')
# plt.plot(ratQ, mseQ, label='q (s=1)')

plt.title("Ratio - MSE for different s")
plt.xlabel("ratio")
plt.ylabel("MSE");
plt.legend(loc='upper left')
plt.show()