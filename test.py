import listdiff
import numpy as np
import pandas as pd

b = pd.DataFrame({'a':[1,2,3],'b':[4,5,6], 'c':[7,8,9]})
a = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]])
print(listdiff.get_elem(a))
print(listdiff.get_elem2(b.values))