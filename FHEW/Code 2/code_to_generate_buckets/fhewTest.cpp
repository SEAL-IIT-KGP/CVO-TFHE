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

int main(int argc, char *argv[]) {

  srand(100);

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
  
  v1 = 1;
  v2 = 0;
  
  FILE* plaintext_ciphertext;

  for(int i = 1; i <= 1; i++) {
  
    plaintext_ciphertext = fopen("plaintext_ciphertext.txt","a");
    fprintf(plaintext_ciphertext, "For data # %d:", i);
    fprintf(plaintext_ciphertext, "\n");
    fclose(plaintext_ciphertext);
  
    cerr << "Running iteration " << i << endl;
    
    LWE::Encrypt(&e1, LWEsk, v1);
    LWE::Encrypt(&e2, LWEsk, v2);
    
    for(int i = 1; i <= 20; i++) {
    
        plaintext_ciphertext = fopen("plaintext_ciphertext.txt","a");
        fprintf(plaintext_ciphertext, "For iteration # %d:", i);
        fprintf(plaintext_ciphertext, "\n");
        fclose(plaintext_ciphertext);
    
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
        
        int *key = new int[n];
    
        char line[] = "-1 0 0 0 1 -1 0 -1 1 0 0 0 1 0 1 0 0 1 0 -1 1 1 0 0 0 0 0 0 0 0 -1 0 -1 0 -1 -1 0 0 -1 0 1 1 -1 1 0 1 0 0 1 -1 1 0 1 0 1 0 -1 0 0 0 0 0 -1 0 0 0 0 -1 0 1 0 0 1 0 0 -1 0 -1 1 0 -1 0 -1 1 1 -1 0 1 0 1 0 -1 -1 0 1 1 1 0 -1 0 0 0 0 0 0 -1 0 -1 -1 0 0 -1 1 0 -1 1 1 0 0 0 0 -1 0 0 1 -1 0 1 0 0 0 -1 0 1 0 1 1 1 1 1 -1 0 -1 -1 -1 -1 1 -1 1 0 0 0 1 0 -1 0 1 0 0 0 0 1 0 0 0 -1 0 0 -1 0 0 0 1 0 0 -1 1 0 -1 1 1 0 -1 0 -1 -1 0 -1 1 1 0 -1 0 -1 1 0 0 0 1 0 1 0 0 1 0 -1 1 0 0 -1 -1 -1 0 0 1 0 0 0 1 0 -1 0 0 1 0 0 -1 1 1 -1 -1 1 0 1 0 1 1 0 0 -1 0 0 0 -1 0 0 1 0 0 0 0 0 1 -1 0 0 0 0 0 0 1 0 0 0 0 -1 0 -1 1 0 -1 0 -1 0 0 1 0 0 -1 0 -1 0 -1 -1 1 0 0 0 -1 1 -1 1 0 0 0 0 1 1 0 0 0 1 1 0 0 0 0 -1 1 0 0 -1 0 0 -1 0 -1 0 1 0 0 -1 0 -1 0 0 0 0 0 0 -1 1 0 -1 0 1 0 -1 1 0 0 0 1 -1 -1 1 0 -1 0 -1 0 0 -1 1 0 1 0 -1 0 1 0 0 1 -1 1 0 1 0 0 1 0 0 0 -1 0 0 1 0 0 0 1 1 -1 -1 0 0 -1 -1 0 0 -1 -1 0 1 -1 -1 0 -1 1 -1 1 -1 0 -1 -1 1 0 -1 0 1 1 0 0 1 0 0 -1 1 0 1 0 0 1 1 0 -1 -1 1 -1 1 1 1 1 0 -1 -1 0 0 -1 1 -1 1 0 1 0 -1 -1 1 1 0 0 0 1 0 -1 0 1 -1 -1 -1 -1 1 -1 -1 0 -1 -1 0 0 0 0 0 -1 1 0 1 -1 0 1 1 0 0 0 -1 0 0 0 -1 0 0 -1 1 0 0 1 1 0 1 0 1";
    
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
   
        plaintext_ciphertext = fopen("plaintext_ciphertext.txt","a");
        fprintf(plaintext_ciphertext, "For HomNAND: message: %d error: %d hamming_weight: %d timing: %d b: %d a: ", MU, error, count, timing, e12.b);
        for (int32_t i = 0; i < n; ++i)
        {
    	    fprintf(plaintext_ciphertext, "%d,", e12.a[i]);
        }
        fprintf(plaintext_ciphertext, "\n");
        fclose(plaintext_ciphertext);
    
    }
    
  }
  
}


