Below is a demo of how the server can introduce perturbations in the resulting ciphertext of a homomorphic gate computation by choosing a bound of the error present in the resultant ciphertexts. One such bound is between 0 to 63, as it will contain all possible positive errors as 63 is the threshold of error on the positive side.

Before proceeding with the demo code, please run the following command to install the necessary tools to run this demo:

1. sudo bash setup.sh		#setup.sh is a bash script containing all the commands to install required tools. In case the 					script does not run for some reason, please run the commands individually to install the tools.

Once the above tools are downloaded and installed, please copy the following files into FHEW directory:

i. fhewTest.cpp 
ii. FHEW.cpp	#The only modification that has been made in this file is to disable the check whether a gate is being run on 			a set of related ciphertexts (either both are same or one is complement of other). Since this check is being 			done on server side, the server is free to disable it.
iii. clean_temporary_files.py 
iv. equation_solver.py

These commands are used to compile the FHEW library and our script. Open a terminal window inside the FHEW folder to run them:

1. make clean			#to remove any build version of FHEW library.

2. make				#to build the library.

3. ./fhewTest			#fhewTest.cpp contains the entire code for the end-to-end attack. To test with a different 				key, replace the value inside srand() function from current value of 1073 to some other value. 					This command automates the end-to-end process of generating random plaintext pairs, encrypting 					them, sending them to the server where homomorphic operations are performed on the same and 					the tampered result is sent back to client where it is decrypted and a reaction is sent if the 					decryption leads to an incorrect result.

This takes around 0.5 hours to complete. In case the above command gets terminated before completion or the experiment needs to be repeated, please run the fourth command to clear out any temporary files created before running the third command again.

4. python clean_temporary_files.py

If the third command runs successfully, please run the fifth command to solve the system of equations created earlier and to obtain the secret key. The command outputs the number of secret key bits recovered successfully, or if the system of equations has no solutions.

5. sage -python equation_solver.py

The above script uses SageMath to perform Gaussian Elimination method of solving the system of equations.


