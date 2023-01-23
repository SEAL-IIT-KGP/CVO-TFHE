import os

with open("plaintext_ciphertext.txt","r") as fp:
	lines = fp.readlines()
	
timing = 0

for line in lines:
	temp = line.strip().split(" ")
	timing = timing + int(temp[3])

timing = int(round(timing/20))

min_timing = 62000000
max_timing = 62500000

if(timing >= min_timing and timing < max_timing):
	pos_min = 5948
	pos_max = 53785065
	os.system("./verify_testing " + str(pos_min) + " " + str(pos_max))
