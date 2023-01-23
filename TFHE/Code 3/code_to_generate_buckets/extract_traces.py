flag = 0
num_iter = 20
timing_data_NAND = 0
error_NAND = 0

count = 0
with open('plaintext_ciphertext.txt', 'r') as fp:
	for line in fp:
		temp = line.strip().split(" ")
		if(temp[1] == "iteration"):
			flag = 1
		if(len(temp) == 16 and flag == 1):
			if(temp[1] == "HomNAND:"):
				timing_data_NAND = timing_data_NAND + int(temp[3]) + int(temp[5])
				error_NAND = temp[9]
			count = count + 1
		if(count == 20):
			with open("template_data.csv","a") as fp:
				fp.write(str(int(round(timing_data_NAND/num_iter))) + "," + error_NAND + "\n")
			timing_data_NAND = 0
			count = 0
			flag = 0
print("Files generated successfully!! Please check.")
