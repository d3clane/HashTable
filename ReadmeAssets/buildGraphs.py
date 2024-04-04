import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt('TestResults/ListSizesTest/FirstCharASCIIHash.txt')

values_col1 = data[:, 0]
heights_col2 = data[:, 1]

values_col1_clipped = np.clip(values_col1, 80, 120)

plt.bar(values_col1_clipped, heights_col2, color='skyblue', edgecolor='black')
plt.xlabel('list id')
plt.ylabel('list size')
plt.title('histogram')
plt.grid(True)

plt.savefig('ReadmeAssets/imgs/FirstCharASCIIHashClose.png')
