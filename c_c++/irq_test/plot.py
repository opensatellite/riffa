import matplotlib.pyplot as plt
import numpy as np
import sys

#print(sys.argv[1])
f=open(sys.argv[1],'r')
time=[]

for line in f:
    #print(line)
    try:
	time.append(float(line))
    except:
	pass

time.pop(0)
time.pop()
f.close()

dt=np.diff(time)
plt.plot(dt)
#plt.tight_layout()
plt.grid(True)
plt.show()

