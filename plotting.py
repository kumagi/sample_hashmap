import json
import matplotlib.pyplot as plt
import pylab as pl

for name in ["extending.json", "not_extending.json"]:
    with open(name) as f:
        data = json.loads(f.read())
        keys = sorted([int(k) for k in data.keys()])
        values = []
        for k in keys:
            values.append(data[str(k)])
        plt.plot(keys, values, '-')

plt.show()
