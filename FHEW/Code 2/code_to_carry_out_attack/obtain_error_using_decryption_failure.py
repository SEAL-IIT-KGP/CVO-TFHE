import os, sys

with open("plaintext_ciphertext.txt","r") as fp:
	lines = fp.readlines()
	
timing = 0

for line in lines:
	temp = line.strip().split(" ")
	timing = timing + int(temp[9])

timing = int(round(timing/20))

min_timing = 148000000
max_timing = 153000000

if(timing >= min_timing and timing < max_timing):
	pos_min = 0
	pos_max = 33
	with open("ranges.txt","w") as fp:
		fp.write(str(pos_min) + " " + str(pos_max) + "\n")
