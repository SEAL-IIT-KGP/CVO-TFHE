import os, sys

def covert_unsigned_to_signed(num):
	if num >= 2147483648: #2147483648 = 2^31
		num = num - 4294967296 #4294967296 = 2^32
	return num

def get_error_range(start,end):
	error_data = []
	with open("template_data.csv", "r") as fp:
		lines = fp.readlines()
	for line in lines:
		temp = line.strip().split(",")
		if (int(temp[0]) >= start and int(temp[0]) < end):
			error_data.append(covert_unsigned_to_signed(int(temp[1])))
	error_data.sort()
	with open("template_error_clusters_for_NAND.txt","a") as fp:
		fp.write("Error values in timing range " + str(start) + " and " + str(end) + " : ")
		for error in error_data:
			fp.write(str(error) + ",")
		fp.write("\n")

bucket_size = int(sys.argv[1])
start = 61000000
last = 64000000
end = start + bucket_size
while((end-1) < last):
	get_error_range(start,end)
	start = end
	end = start + bucket_size
print("Done!!")
