import os

def covert_unsigned_to_signed(num):
	if num >= 2147483648: #2147483648 = 2^31
		num = num - 4294967296 #4294967296 = 2^32
	return str(num)

with open("testing_data_for_HomNAND_without_AVX.txt","r") as fp:
	lines = fp.readlines()

for line in lines:
	temp = line.strip().split(" ")
	timing = int(temp[3])
	with open("template_error_clusters_for_NAND.txt", "r") as fp:
		lines_template = fp.readlines()
		neg_max = -2
		neg_min = -1
		pos_min = 1
		pos_max = 2
		for lines in lines_template:
			temp_1 = lines.strip().split(" ")
			start = int(temp_1[5])
			end = int(temp_1[7])
			if(timing >= start and timing < end and len(temp_1) == 10):
				line_1 = temp_1[9].strip().split(",")
				leng = len(line_1)
				if(leng > 2):
					if(int(line_1[0]) > 0 and int(line_1[leng - 2]) > 0):
						pos_min = int(line_1[0])
						pos_max = int(line_1[leng - 2])
					elif(int(line_1[0]) < 0 and int(line_1[leng - 2]) < 0):
						neg_max = int(line_1[0])
						neg_min = int(line_1[leng - 2])
					else:
						neg_max = int(line_1[0])
						for i in range(0,leng-1):
							current = int(line_1[i])
							next = int(line_1[i+1])
							if(current < 0 and next > 0):
								neg_min = int(line_1[i])
								pos_min = int(line_1[i+1])
								break
						pos_max = int(line_1[leng - 2])
	print(str(neg_max) + " " + str(neg_min) + " " + str(pos_min) + " " + str(pos_max))		
	os.system("./verify_testing " + str(neg_max) + " " + str(neg_min) + " " + str(pos_min) + " " + str(pos_max) + " " + temp[11] + " " + temp[13])
