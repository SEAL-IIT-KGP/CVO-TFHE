import os, sys

def get_error_range(start,end):
	error_data = []
	with open("template_data_for_HomNAND.csv", "r") as fp:
		lines = fp.readlines()
	for line in lines:
		temp = line.strip().split(",")
		if (int(temp[0]) >= start and int(temp[0]) < end):
			error_data.append(int(temp[1]))
	error_data.sort()
	with open("template_error_clusters_for_NAND.txt","a") as fp:
		fp.write("Error values in timing range " + str(start) + " and " + str(end) + " : ")
		for error in error_data:
			fp.write(str(error) + ",")
		fp.write("\n")

bucket_size = int(sys.argv[1])
start = 143000000
last = 168000000
end = start + bucket_size
while((end-1) < last):
	get_error_range(start,end)
	start = end
	end = start + bucket_size
print("Done!!")
