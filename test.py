import listdiff
import numpy as np
import pandas as pd

reclist = pd.DataFrame({
	'pid':		[1,1,1,2,2,2,3,3,3],
	'song_id':	[4,5,6,4,5,6,4,5,12],
	'pos':		[1,2,3,1,2,3,3,2,1]
})
gt = pd.DataFrame({
	'pid':		[1,1,1,2,2,2,2],
	'song_id':	[1,5,9,4,5,12,9],
	'pos':		[1,2,3,4,3,2,1]
})
complement = pd.DataFrame({
	'pid':		[1,1,1,2,2,2,2,3,3,3,-1],
	'song_id':	[10,11,12,10,11,12,13,10,11,12,1],
	'pos':		[1,2,3,1,2,3,4,3,2,1,1]
})
res = listdiff.listdiff(
	reclist[["pid","song_id","pos"]].sample(frac=1).values,
	gt[["pid","song_id","pos"]].sample(frac=1).values,
	complement=complement[["pid","song_id","pos"]].sample(frac=1).values,
	extra_pids=np.array([3,4]),
	cut_to_size=4
)
print(res)

assert np.all(res[res[:,0]==1] == np.array([
	[1,4,1],
	[1,6,2],
	[1,10,3],
	[1,11,4]
]))
assert np.all(res[res[:,0]==2] == np.array([
	[2,6,1],
	[2,10,2],
	[2,11,3],
	[2,13,4]
]))
assert np.all(res[res[:,0]==3] == np.array([
	[3,12,1],
	[3,5,2],
	[3,4,3],
	[3,11,4]
]))
assert np.all(res[res[:,0]==4] == np.array([
	[4,1,1],
]))