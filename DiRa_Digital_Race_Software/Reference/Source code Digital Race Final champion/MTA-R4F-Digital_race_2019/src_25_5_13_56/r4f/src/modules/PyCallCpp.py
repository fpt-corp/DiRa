import os 
import subprocess
def get_shortest_route(input):
	myCmd = '/home/nvidia/catkin_ws/src/r4f/src/modules/a.out '+ input
	k = subprocess.check_output(myCmd, shell=True)
	k_arr = k.split()
	#print('path finded:', k_arr)
	x = []
	for  i in  range(len(k_arr)):
		if k_arr[i][0] != '1' and  k_arr[i][0] != '2' and  k_arr[i][0] != '6':
			x.append(k_arr[i])
			if k_arr[i][0] == '5' and k_arr[i+1][0] == '1' and k_arr[i+3][0] == '5':
				x.append('12')
			if k_arr[i][0] == '5' and k_arr[i+1][0] == '2' and k_arr[i+3][0] == '5':
				x.append('21')
	x.append('10')
	x.append("10E")
	return x


