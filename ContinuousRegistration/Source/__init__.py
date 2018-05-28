import numpy as np

# parse the numpy versions
np_version = [int(i) for i in np.version.full_version.split('.')]
# comparing strings does not work for version lower 1.10
if np_version >= [1, 14]:
    np.set_printoptions(legacy='1.13')