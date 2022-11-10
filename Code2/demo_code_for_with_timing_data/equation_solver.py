from sage.all import *
import numpy as np

def solve_equation(A,B,K):
	Z = Zmod(2**32)
	A_1 = matrix(Z, 723, 630, range(455490))
	for i in range(0,723):
		A_1.set_row(i,A[i])
	b = vector(Z, 723, B)
	S = A_1.solve_right(b)
	count = 0
	with open("generated_key.txt","a") as fp:
		for i in range(630):
			fp.write(str(S[i]) + " ")
			if((S[i] == 0 or S[i] == 1) and S[i] == K[i]):
				count = count + 1
	print("\n" + str(count) + "-bits has been identified correctly!!")

def convert(str_lst):
	lst = []
	for string in str_lst:
		if (string != ""):
			temp = int(string)
			lst.append(temp)
	return lst

with open("gaussian_elimination.txt","r") as fp:
	lines = fp.readlines()
	
with open("secret_key_testing.txt","r") as fp:
	lines_1 = fp.readline()
key = lines_1.strip().split(" ")
key = convert(key)

A = []
B = []
for line in lines:
	temp = line.strip().split(" ")
	if(len(temp) == 8):
		a = temp[7].strip().split(",")
		a = convert(a)
		A.append(a)
		message = int(temp[3])
		error = int(temp[1])
		b = int(temp[5])
		b = (b - message) % 2**32
		b = (b - error) % 2**32
		B.append(b)
solve_equation(A,B, key)

