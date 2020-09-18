import os
import numpy as np
import matplotlib.pyplot as plt

def func(external_proc_num, internal_proc_num, Matrix):
	for e in range(0, external_proc_num):
		for i in range(0, internal_proc_num):
			cmd = './heatmap.out ' + str(e) + ' ' + str(i) 
			# print(cmd, sep = '\n')
			so = os.popen(cmd).read()
			# print(so)
			Matrix[e][i] = so
def draw(Matrix):
	plt.imshow(Matrix, cmap='hot', interpolation='nearest')
	plt.show()

if __name__ == "__main__":
	ex_proc_num = 4
	in_proc_num = 16
	Matrix = np.zeros((ex_proc_num, in_proc_num))
	
	func(ex_proc_num, in_proc_num, Matrix)

	print(Matrix)
	draw(Matrix)
