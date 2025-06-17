import numpy as np
data_file_name="BM_Output_30.csv"

array=np.loadtxt(data_file_name,delimiter=",")
print(array.shape)
count_data=array[:,3:].reshape(-1)
prob_arr=np.linspace(0,1,count_data.shape[0])
ans=np.dstack([prob_arr,np.sort(count_data)])[0,:,:]
print(ans[:,0])
np.savetxt("prob_"+data_file_name,ans,delimiter=",")

err07=np.abs(ans[:,0]-0.7)
ind=np.argmin(err07)
print(F"ITS0.7 at p={ans[ind,0]}, with ITS={ans[ind,1]}")

# import matplotlib.pyplot as plt
# plt.plot(ans)
# plt.show()