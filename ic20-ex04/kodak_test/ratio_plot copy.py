import matplotlib.pyplot as plt
plt.style.use('seaborn-whitegrid')
import numpy as np
from math import log2

fig = plt.figure()
ax = plt.axes()

ratSQ0=[]
mseSQ0=[]
ratSQ1=[]
mseSQ1=[]
ratQ=[]
mseQ=[]

for ratL,mseL,f in [(ratSQ0,mseSQ0,"s.txt"), \
            (ratSQ1,mseSQ1,"s_q1.txt"), \
            (ratQ,mseQ,"q.txt"), \
            ]:
    for l in open(f).readlines()[1:]:
        vals=l.split(" ")
        # s=int(vals[0])
        # q=int(vals[1])
        # s=log2(s)
        # q=log2(q)
        ratio=float(vals[2])
        mse=float(vals[3])
        ratL.append(ratio)
        mseL.append(mse)

# x = np.linspace(0, 10, 1000)
# plt.plot(x, np.sin(x), label='sin')
# plt.plot(x, np.cos(x), label='cos')
plt.plot(ratSQ0, mseSQ0, label='s (q=0)')
plt.plot(ratSQ1, mseSQ1, label='s (q=1)')
plt.plot(ratQ, mseQ, label='q (s=1)')

plt.title("Ratio - MSE")
plt.xlabel("ratio")
plt.ylabel("MSE");
plt.legend();
plt.show()