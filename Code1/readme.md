Below is a demo of how the server can introduce perturbations in the resulting ciphertext of a homomorphic gate computation by choosing a bound of the error present in the resultant ciphertexts. One such bound is between 0 to 2^31, as it will contain all possible positive errors due to the fact that TFHE works under modulo-2^32.

We have provided all the necessary code along with the Dockerfile in our GitHub repository. To clone this repository and build the image locally, the following set of commands need to be run:

1. git clone https://github.com/cracking-tfhe/s_n_p_submission_2023.git 	#clones the repository locally in the current directory.

2. cd s_n_p_submission_2023/Code1/demo_code_for_without_timing_data 	#to build demo code for attack without timing data.
or
2. cd s_n_p_submission_2023/Code2/demo_code_for_with_timing_data 		#to build demo code for attack with timing data.

3. docker build -t cracktfhe/code1:v_0 . 				#to build image for demo code for attack without timing data.
or
3. docker build -t cracktfhe/code2:v_0 . 				#to build image for demo code for attack with timing data.

4. docker run -it --rm=true cracktfhe/code1:v_0 /bin/bash 		#to build image for demo code for attack without timing data.
or
4. docker run -it --rm=true cracktfhe/code2:v_0 /bin/bash 		#to build image for demo code for attack with timing data.

The entire process of cloning the repository and building the image takes around 20 to 25 minutes depending upon the system.

Once the Docker image is built and running, the demo can be run using the following set of commands in the given sequence:

These commands are used to compile our scripts

1. gcc user_testing.c -o user_testing -ltfhe-spqlios-fma 		#user_testing.c contains the code to automate the entire end-to-end process.

2. gcc cloud_testing.c -o cloud_testing -ltfhe-spqlios-fma		#cloud_testing.c contains the code that is run on the server to perform a homomorphic NAND operation on the obtained ciphertexts.

3. gcc verify_testing.c -o verify_testing -ltfhe-spqlios-fma		#verify_testing.c contains the code run by client to perform decryption and send feedback to server in case the result is incorrect.

This command automates the end-to-end process of generating random plaintext pairs, encrypting them, sending them to the server where homomorphic operations are performed on the same and the tampered result is sent back to client where it is decrypted and a reaction is sent if the decryption leads to an incorrect result.

4. ./user_testing

This takes around 1 to 1.5 hours to complete. In case the above command gets terminated before completion or the experiment needs to be repeated, please run the fifth command to clear out any temporary files created before running the fourth command again.

5. python clean_temporary_files.py

If the fourth command runs successfully, please run the sixth command to solve the system of equations created earlier and to obtain the secret key. The command automatically checks whether the obtained key is correct or not.

6. sage -python equation_solver.py

The above script uses SageMath to perform Gaussian Elimination method of solving the system of equations.


