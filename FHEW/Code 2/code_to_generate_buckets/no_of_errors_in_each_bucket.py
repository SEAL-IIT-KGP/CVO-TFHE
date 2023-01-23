import math

def convert(str_lst):
	lst = []
	for string in str_lst:
		if (string != ""):
			temp = int(string)
			lst.append(temp)
	return lst

with open("template_error_clusters_for_NAND.txt", "r") as fp:
	lines = fp.readlines()
for line in lines:
	temp = line.strip().split(" ")
	with open("no_of_errors_in_each_bucket.txt","a") as fp:
		if(len(temp) == 10):
			temp_1 = temp[9].strip().split(",")
			temp_1 = convert(temp_1)
			temp_1.sort()
			l = len(temp_1)
			if(l > 1):
				diff = temp_1[l-1] - temp_1[0]
				fp.write(str(l) + "\t" + str(temp_1[0]) + "\t" + str(temp_1[l-1]) + "\t" + str(diff) + "\t" + str(math.log(diff,2)) + "\n")
			else:
				fp.write("1\n")
		else:
			fp.write("0\n")
