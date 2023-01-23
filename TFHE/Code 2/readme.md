Below is a simplified demo of how the server can introduce perturbations in the resulting ciphertext of a homomorphic gate computation by observing its timing value. Based on this timing value, the server extracts the bound of the error present in this resulting ciphertext from a template it had created during the offline phase using a key of its choice. 

Before proceeding with the demo code, please run the following command to install the necessary tools to run this demo. Please skip if the tools are already downloaded:

1. sudo bash setup.sh		#setup.sh is a bash script containing all the commands to install required tools. In case the 					script does not run for some reason, please run the commands individually to install the tools.

Once the above tools are downloaded and installed, please perform the following step before proceeding with the next set of commands:

1. Please copy the file "cacheutils.h" into the "tfhe/src/include" directory, as it contains helper functions to record timing values of an operation.

Once done, build and install the tfhe library using the following sequence of commands:

1. cd tfhe
2. make
3. sudo make install
4. cd ..

############################################## Generating Buckets ##############################################################

Enter into the "code_to_generate_buckets" directory and run the following set of commands to generate the timing traces:

1. gcc user_template_creation.c -o user_template_creation -ltfhe-spqlios-fma

2. g++ cloud_testing.c -o cloud_testing -ltfhe-spqlios-fma

3. ./user_template_creation

This takes around 14 to 15 hours to complete. In case the above command gets terminated before completion, please run the fourth command to clear out any temporary files created before running the third command again.

4. python clean_temporary_files.py

If the third command runs successfully, please run the fifth command to extract the timing traces, which are dumped into a file "template_data_for_HomNAND.csv".

5. python extract_traces.py

Once extracted, please find the minimum and maximum timing values from the traces using the file "template_data_for_HomNAND.csv". Update the values of start and last in the script "generate_buckets.py" with the minimum and maximum timing values, and then run the following command to generate the buckets:

6. python generate_buckets.py $bucket_size$				#Replace $bucket_size$ with the size of the bucket you want. This value needs to be chosen optimally.

The following command may be run to get the number of errors in each bucket, stored in "no_of_errors_in_each_bucket.txt", which may help in choosing the appropriate value of $bucket_size$:

7. python no_of_errors_in_each_bucket.py

In case the buckets are to be regenerated with a different value of $bucket_size$, please delete the files "template_error_clusters_for_NAND.txt" and "no_of_errors_in_each_bucket.txt" before running sixth and seventh commands again.

Once the buckets are generated, please note the minimum and maximum timing and positive error values of the bucket with the highest number of errors. These values will be needed to carry out the actual attack.

################################################# Actual Attack ################################################################

Enter into the "code_to_carry_out_attack" directory and run the following commands to proceed with the actual attack phase. Before proceeding with the next set of commands, please update the values of min_timing, max_timing, pos_min and pos_max variables in the file "obtain_error_using_decryption_failure.py" with the starting and ending time along with the minimum and maximum value of positive error of the bucket containing the maximum number of errors, as obtained during the bucket generation phase. This is a crucial step.

1. gcc user_testing.c -o user_testing -ltfhe-spqlios-fma 		#user_testing.c contains the code to automate the entire end-to-end process.

2. gcc cloud_testing.c -o cloud_testing -ltfhe-spqlios-fma		#cloud_testing.c contains the code that is run on the server to perform a homomorphic NAND operation on the obtained ciphertexts.

3. gcc verify_testing.c -o verify_testing -ltfhe-spqlios-fma		#verify_testing.c contains the code run by client to perform decryption and send feedback to server in case the result is incorrect.

4. ./user_testing		#This command automates the end-to-end process of generating random plaintext pairs, 					encrypting them, sending them to the server where homomorphic operations are performed on the 					same and the tampered result is sent back to client where it is decrypted and a reaction is 					sent if the decryption leads to an incorrect result.

This takes around 8 to 10 hours to complete. In case the above command gets terminated before completion or the experiment needs to be repeated, please run the fifth command to clear out any temporary files created before running the fourth command again.

5. python clean_temporary_files.py

If the fourth command runs successfully, please run the sixth command to solve the system of equations created earlier and to obtain the secret key. The command automatically checks whether the obtained key is correct or not.

6. sage -python equation_solver.py

The above script uses SageMath to perform Gaussian Elimination method of solving the system of equations.


