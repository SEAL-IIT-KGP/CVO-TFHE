Below is a simplified demo of how the server can introduce perturbations in the resulting ciphertext of a homomorphic gate computation by observing its timing value. Based on this timing value, the server decides whether to proceed with perturbing the corresponding ciphertext, if this value lies in a pre-defined timing range, or to ignore it. This acts as a first level filter.

Before proceeding with the demo code, please run the following command to install the necessary tools to run this demo. Please skip if the tools are already downloaded:

1. sudo bash setup.sh		#setup.sh is a bash script containing all the commands to install required tools. In case the 					script does not run for some reason, please run the commands individually to install the tools.

Once the above tools are downloaded and installed, please perform the following two steps before proceeding with the next set of commands:

1. Please copy the file "cacheutils.h" into the FHEW directory, as it contains helper functions to record timing values of an operation.

2. Once done, please update the "Makefile" inside the FHEW directory to add cacheutils.h in the "INCLUDE=" parameter.

############################################## Generating Buckets ##############################################################

Copy the following files from "code_to_generate_buckets" directory into FHEW directory to proceed with the bucket generation phase:

i. fhewTest.cpp 
ii. clean_temporary_files.py 
iii. extract_traces.py
iv. generate_buckets.py
v. no_of_errors_in_each_bucket_NAND.py

These commands are used to compile the FHEW library and our script. Open a terminal window inside the FHEW folder to run them:

1. make clean			#to remove any build version of FHEW library.

2. make				#to build the library.

3. ./fhewTest			#fhewTest.cpp contains the code to generate the timing traces. We suggest not to change the 					value in srand() and keep it to its current value of 100. Otherwise it will generate a 					different secret key which will result in computation of incorrect errors during the bucket 					generation phase.

This takes around 5 to 6 hours to complete. In case the above command gets terminated before completion, please run the fourth command to clear out any temporary files created before running the third command again.

4. python clean_temporary_files.py

If the third command runs successfully, please run the fifth command to extract the timing traces, which are dumped into a file "template_data_for_HomNAND.csv".

5. python extract_traces.py

Once extracted, please find the minimum and maximum timing values from the traces using the file "template_data_for_HomNAND.csv". Update the values of start and last in the script "generate_buckets.py" with the minimum and maximum timing values, and then run the following command to generate the buckets:

6. python generate_buckets.py $bucket_size$		#Replace $bucket_size$ with the size of the bucket you want. This 							value needs to be chosen optimally.

The following command may be run to get the number of errors in each bucket, stored in "no_of_errors_in_each_bucket.txt", which may help in choosing the appropriate value of $bucket_size$:

7. python no_of_errors_in_each_bucket.py

In case the buckets are to be regenerated with a different value of $bucket_size$, please delete the files "template_error_clusters_for_NAND.txt" and "no_of_errors_in_each_bucket.txt" before running sixth and seventh commands again.

Once the buckets are generated, please note the minimum and maximum timing and positive error values of the bucket with the highest number of errors. These values will be needed to carry out the actual attack.

################################################# Actual Attack ################################################################

Copy the following files from "code_to_carry_out_attack" directory into FHEW directory to proceed with the actual attack phase:

i. fhewTest.cpp 
ii. FHEW.cpp				#The only modification that has been made in this file is to disable the check whether 						a gate is being run on a set of related ciphertexts (either both are same or one is 						complement of other). Since this check is being done on server side, the server is 					free to disable it.
iii. clean_temporary_files.py 
iv. equation_solver.py
v. obtain_error_using_decryption_failure.py

Before proceeding with the next set of commands, please update the values of min_timing, max_timing, pos_min and pos_max variables in the file "obtain_error_using_decryption_failure.py" with the starting and ending time along with the minimum and maximum value of positive error of the bucket containing the maximum number of errors, as obtained during the bucket generation phase. This is a crucial step.

These commands are used to compile the FHEW library and our script. Open a terminal window inside the FHEW folder to run them:

1. make clean		#to remove any build version of FHEW library.

2. make			#to build the library.

3. ./fhewTest		#fhewTest.cpp contains the entire code for the end-to-end attack. To test with a different key, 				replace the value inside srand() function from current value of 2022 to some other value. This command 				automates the end-to-end process of generating random plaintext pairs, encrypting them, sending them 				to the server where homomorphic operations are performed on the same and the tampered result is sent 				back to client where it is decrypted and a reaction is sent if the decryption leads to an incorrect 			result.

This takes around 1.5 hours to complete. In case the above command gets terminated before completion, please run the fourth command to clear out any temporary files created before running the third command again.

4. python clean_temporary_files.py

If the third command runs successfully, please run the fifth command to solve the system of equations created earlier and to obtain the secret key. The command outputs the number of secret key bits recovered successfully, or if the system of equations has no solutions.

5. sage -python equation_solver.py

The above script uses SageMath to perform Gaussian Elimination method of solving the system of equations.


