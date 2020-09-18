import os
import numpy as np

def func(external_proc_num, internal_proc_num):
	for e in range(external_proc_num):
		for i in range(internal_proc_num):
			cmd = './heatmap.out ' + str(e) + ' ' + str(i) 
			print(cmd, sep = '\n')
			# so = os.popen(cmd).read()
			# print(so)

if __name__ == "__main__":
	Matrix
	func(2, 5)
