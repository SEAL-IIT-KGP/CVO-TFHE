#include <iostream>
#include <cstdlib>
#include<string.h>
#include<stdlib.h>
#include "LWE.h"
#include "FHEW.h"
#include "distrib.h"

#include "cacheutils.h"

using namespace std;

void help(char* cmd) {
  cerr << "\nusage: " << cmd << " n\n\n" 
  << "  Generate a secret key sk and evaluation key ek, and repeat the following test n times:\n"
  << "   - generate random bits b1,b2,b3,b4\n"
  << "   - compute ciphertexts c1, c2, c3 and c4 encrypting b1, b2, b3 and b4  under sk\n"
  << "   - homomorphically compute the encrypted (c1 NAND c2) NAND (c3 NAND c4) \n"
  << "   - decrypt all the intermediate results and check correctness \n"
  << "\n If any of the tests fails, print ERROR and stop immediately.\n\n";
  exit(0);
}

int cleartext_gate(int v1, int v2, BinGate gate){
  switch(gate)
  {
    case OR: return v1 || v2;
    case AND: return v1 && v2;
    case NOR: return not(v1 || v2);
    case NAND: return not(v1 && v2);
    default: cerr << "\n This gate does not exists \n"; exit(1); return 0;
  }
}

void cerr_gate(BinGate gate){
  switch(gate)
  {
    case OR: cerr << " OR\t"; return;
    case AND: cerr << " AND\t"; return;
    case NOR: cerr << " NOR\t"; return;
    case NAND: cerr << " NAND\t"; return;
  }
}

int pos_threshold = 63;

int error = -1;
int count;

int get_error_if_positive(LWE::CipherText& e12, int start, int end, LWE::SecretKey& key, int ans) {

    int mid = (start + end)/2;
    
    int ci, distance_from_threshold;

    if(start == end-1) { //base case for recursion
        return error;
    }
    
    distance_from_threshold = pos_threshold - mid;
    
    e12.b = (e12.b + distance_from_threshold) % q;
    
    ci = LWE::Decrypt(key, e12);
    count = count + 1;
    e12.b = (e12.b - distance_from_threshold) % q;
    
    if(ci == ans) {//if decrypted correctly
        error = mid;
    	get_error_if_positive(e12, start, mid, key, ans);
    }
    else {
    	get_error_if_positive(e12, mid, end, key, ans);
    }
}

void get_sign(LWE::CipherText& e12, int pos_min, int pos_max, LWE::SecretKey& key, FHEW::EvalKey& EK, int ans) {
    
    int distance_from_threshold = pos_threshold - pos_min;
    FILE* samples;
    
    e12.b = (e12.b + (distance_from_threshold)) % q;
    
    int ai = LWE::Decrypt(key, e12);
    count = count + 1;
    
    e12.b = (e12.b - (distance_from_threshold)) % q;
    
    if(ans != ai) {
    
    	samples = fopen("samples.csv","a");
    	fprintf(samples, "%d,", ai);
    	fclose(samples);
    	
    	LWE::CipherText ne12, e0;
    	
    	FHEW::HomNOT(&ne12, e12);
    	BinGate gate = static_cast<BinGate>(1);
    	FHEW::HomGate(&e0, gate, EK, e12, ne12);
  
    	int bi = LWE::Decrypt(key, e0);
    	count = count + 1;

    	if(ans != bi) {
    
    	    samples = fopen("samples.csv","a");
            fprintf(samples, "%d,", bi);
            fclose(samples);
    
            int error_1 = -1;
    
    	    if(ans == 1) {
    	        distance_from_threshold = pos_threshold - pos_max;
    	        e12.b = (e12.b + (distance_from_threshold)) % q;
    
                int ci = LWE::Decrypt(key, e12);
                count = count + 1;
    
                e12.b = (e12.b - (distance_from_threshold)) % q;
                if(ci == 1) {//error is less than e_max
    			error_1 = get_error_if_positive(e12, pos_min, pos_max, key, ans);
    			samples = fopen("samples.csv","a");
    			fprintf(samples, "%d\n", error_1);
    			fclose(samples);
    			if(error_1 != -1) //If the recovered error is negative one, the sample will be rejected as such a sample would have already been rejected previously, implying some error has occured
    			{
    		    		FILE* gaussian_elimination = fopen("gaussian_elimination.txt","a");
    		    		fprintf(gaussian_elimination, "recovered_error: %d m: %d b: %d a: ", error_1, 128, e12.b);
    		    		for (int32_t i = 0; i < n; ++i)
    		    		{
    		        		fprintf(gaussian_elimination, "%d,", e12.a[i]);
    		    		}
    		    		fprintf(gaussian_elimination, "\n");
    		    		fclose(gaussian_elimination);
    			}
    		}
    	    }
	    else {
	 	samples = fopen("samples.csv","a");
    	    	fprintf(samples, "\n");
    	    	fclose(samples);
	    }
    	}
	else {
	    samples = fopen("samples.csv","a");
    	    fprintf(samples, "\n");
    	    fclose(samples);
	}
    }
    else {
	samples = fopen("samples.csv","a");
    	fprintf(samples, "\n");
    	fclose(samples);
    }

    FILE* count_of_oracle_accesses = fopen("count_of_oracle_accesses.csv","a");
    fprintf(count_of_oracle_accesses, "%d\n", count);
    fclose(count_of_oracle_accesses);
}


int main(int argc, char *argv[]) {

  srand(2022);

  cerr << "Setting up FHEW \n";
  FHEW::Setup();
  cerr << "Generating secret key ... ";
  LWE::SecretKey LWEsk;
  LWE::KeyGen(LWEsk);
  
  FILE *secret_key = fopen("secret_key.txt","a");
  for (int i = 0; i < n; ++i) {
    fprintf(secret_key, "%d ", LWEsk[i]);
  }
  fprintf(secret_key, "\n");
  fclose(secret_key);
  
  cerr << " Done.\n";
  cerr << "Generating evaluation key ... this may take a while ... ";
  FHEW::EvalKey EK;
  FHEW::KeyGen(&EK, LWEsk);
  cerr << " Done.\n\n";

  int v1,v2;
  LWE::CipherText e1, e2, e12;

  int pos_min;
  int pos_max;

  for(int i = 1; i <= 3000; i++) {
  
    v1 = rand()%2;
    v2 = rand()%2;
    
    FILE* data_points = fopen("data_points.csv","a");
    fprintf(data_points, "%d,%d\n", v1, v2);
    fclose(data_points);
  
    cerr << "Running iteration " << i << endl;
    
    LWE::Encrypt(&e1, LWEsk, v1);
    LWE::Encrypt(&e2, LWEsk, v2);
    
    for(int i = 1; i <= 20; i++) {
    
        BinGate gate = static_cast<BinGate>(3);
        
        //start time
        uint64_t start = rdtsc_begin();
        
        FHEW::HomGate(&e12, gate, EK, e1, e2);
        
        //end time
        uint64_t end = rdtsc_end();

        //time difference
        int timing = end - start;
    
        int MU;
    
        if(gate == 0 || gate == 3) {MU = 128;} //If gate is OR or NAND
        else {MU = 0;} //If gate is AND or NOR
        
        /*if(gate == 0 || gate == 1) {MU = 128;} //If gate is OR or AND
        else {MU = 0;} //If gate is NAND or NOR*/
        
        int *key = new int[n];
    
        char line[] = "1 0 1 1 0 0 0 1 0 1 0 1 0 0 0 0 0 -1 0 -1 0 -1 1 1 0 -1 0 1 0 0 0 0 -1 0 0 0 1 1 0 0 1 0 0 0 1 1 -1 0 -1 0 1 0 0 0 0 -1 1 1 1 0 0 0 -1 0 0 0 1 0 0 0 -1 -1 1 0 0 1 0 0 0 0 1 0 1 0 -1 0 0 1 -1 0 0 0 -1 0 -1 0 1 -1 0 -1 0 0 0 0 0 1 -1 -1 0 0 0 0 -1 -1 1 -1 0 0 -1 1 -1 0 0 -1 -1 0 1 1 0 0 -1 1 0 0 -1 0 0 0 0 0 1 1 1 1 1 1 -1 0 0 -1 0 0 0 0 1 0 -1 1 0 0 -1 1 0 1 0 1 -1 0 0 0 -1 0 0 -1 0 0 1 0 1 0 0 0 -1 -1 1 1 1 -1 0 0 1 0 -1 0 0 0 0 0 -1 1 -1 0 -1 0 0 0 1 0 0 1 1 -1 -1 1 0 -1 0 -1 -1 -1 0 -1 1 0 0 0 -1 -1 1 0 -1 -1 0 -1 0 1 0 0 0 0 0 -1 0 0 1 1 0 0 1 0 0 -1 1 0 1 0 1 -1 0 0 0 0 0 1 1 0 1 0 1 -1 0 0 -1 -1 0 1 1 -1 0 1 1 1 1 0 0 1 -1 -1 1 1 1 0 0 0 -1 0 -1 1 0 1 1 0 -1 1 0 1 1 0 -1 -1 0 -1 -1 0 1 0 -1 1 1 -1 0 1 0 -1 0 0 0 0 0 -1 0 0 0 0 -1 0 0 -1 1 0 0 -1 1 0 0 0 -1 0 0 -1 1 0 1 -1 0 0 0 -1 1 -1 0 -1 0 1 1 0 -1 0 0 -1 -1 -1 -1 1 0 1 0 1 0 -1 1 0 0 0 0 -1 1 0 -1 1 0 0 1 0 0 1 0 0 -1 -1 0 0 -1 1 0 0 0 1 0 -1 -1 -1 0 1 1 -1 1 1 0 -1 0 1 0 0 0 -1 0 1 0 0 1 0 -1 -1 0 0 0 -1 0 -1 -1 0 0 0 -1 0 1 -1 -1 -1 -1 1 -1 1 0 0 1 1 0 0 0 0 -1 0 0 0 1 -1 0 -1 -1 1 0 0 0 1 1 0 1 1 0 -1 1 0 -1 -1 -1 1 -1 0 0 0 1 1 -1 0 0 -1 0 1";
    
        int k = 0;

        char* tok = strtok(line, " ");

        while (tok != NULL)
        {
	    key[k++] = atoi(tok);
	    tok = strtok(NULL, " ");
        }
        
        int b = e12.b;
    
        for (int i = 0; i < n; ++i)
        {
            b = (b - (e12.a[i]*key[i])) % q;
        }
    
        int temp = (b - MU) % q;
        if(temp < -64)
        {
    	    temp = temp + q; //Modulus problem so rounding to correct value
        }
        else if(temp > 63)
        {
    	    temp = temp - q; //Modulus problem so rounding to correct value
        }
        int error = temp;
        int count = 0;
        while(temp != 0)
        {
	    count = count + (temp%2);
	    temp = temp/2;
        }
    
        free(key);
   
        FILE* plaintext_ciphertext = fopen("plaintext_ciphertext.txt","a");
        fprintf(plaintext_ciphertext, "For HomNAND: message: %d error: %d hamming_weight: %d timing: %d b: %d a: ", MU, error, count, timing, e12.b);
        for (int32_t i = 0; i < n; ++i)
        {
    	    fprintf(plaintext_ciphertext, "%d,", e12.a[i]);
        }
        fprintf(plaintext_ciphertext, "\n");
        fclose(plaintext_ciphertext);
    
    }

    system("python obtain_error_using_decryption_failure.py");
    
    char line[1000];
    FILE *fptr;
    
    int lines;
    
    if ((fptr = fopen("ranges.txt", "r")) == NULL) {
    	lines = 0;
    }
    else {
    	lines = fscanf(fptr, "%[^\n]", line);
    	fclose(fptr);
    }
    
    fclose(fopen("ranges.txt", "w"));
    
    if(lines == 1) {
    
    int *return_values = new int[2];
    int j = 0;

    char* tok = strtok(line, " ");

    while (tok != NULL)
    {
	return_values[j++] = atoi(tok);
	tok = strtok(NULL, " ");
    }

    pos_min = return_values[0];
    pos_max = return_values[1];
    
    free(return_values);
    
    FILE* samples = fopen("samples.csv","a");
    fprintf(samples, "%d,", i);
    fclose(samples);
    
    int ans = LWE::Decrypt(LWEsk, e12);

    samples = fopen("samples.csv","a");
    fprintf(samples, "%d,", ans);
    fclose(samples);
    
    count = 0;
    get_sign(e12, pos_min, pos_max, LWEsk, EK, ans);
    
    }
    
    fclose(fopen("plaintext_ciphertext.txt","w"));
    
  }
  
}


