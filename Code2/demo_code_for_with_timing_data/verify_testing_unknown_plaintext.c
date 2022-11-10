#define _POSIX_C_SOURCE 200809L

#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdlib.h>
#include <string.h>

//Below thresholds are for NAND gate. For other gates these values will differ. The values are taken in unsigned int representation as TFHE treats them as such.
long neg_threshold = 536870911;
long pos_threshold = 10200547327;
uint32_t MU = 536870912; //Torus equivalent of 1

int error = 0;
int count = 0;

int get_error_if_positive(LweSample *sample, int start, int end, TFheGateBootstrappingSecretKeySet* key, int ans) {

    int mid = (start + end)/2;
    
    if(start == end-1) { //base case for recursion, when only one element is left
    	printf("%d\n",error);
    	return error; //mid will contain correct error
    }
    
    long distance_from_threshold = pos_threshold - mid;
    
    sample->b += distance_from_threshold; //adding perturbation to the resulting ciphertext
    int ci = bootsSymDecrypt(sample, key); count = count + 1;
    sample->b -= distance_from_threshold; //restoring back the original error value, which is necessary as it is this error that we are trying to recover
    
    if(ci == ans) {//if decrypted correctly
        error = mid;
    	get_error_if_positive(sample, start, mid, key, ans);
    }
    else {
    	get_error_if_positive(sample, mid, end, key, ans);
    }
}

int get_sign(LweSample *sample, int pos_min, int pos_max, TFheGateBootstrappingSecretKeySet* key, int ans) {
    long distance_from_threshold = pos_threshold - pos_min;
    
    FILE* samples;
    
    sample->b += distance_from_threshold; //adding perturbation to the resulting ciphertext
    int ai = bootsSymDecrypt(sample, key); printf("ai: %d\n", ai); count = count + 1;
    sample->b -= distance_from_threshold; //restoring back the original error value, which is necessary as it is this error that we are trying to recover
    
    if(ans != ai) {//Did not decrypt correctly as underlying message flipped
    	samples = fopen("samples.csv","a");
    	fprintf(samples, "%d,", ai);
    	fclose(samples);
    
    	sample->b -= (2*MU); //adding perturbation to the resulting ciphertext
    	int bi = bootsSymDecrypt(sample, key); printf("bi: %d\n", bi); count = count + 1;
    	sample->b += (2*MU); //restoring back the original error value, which is necessary as it is this error that we are trying to recover
    	
    	if(ans != bi) {//Did not decrypt correctly as underlying message flipped
    
    	    samples = fopen("samples.csv","a");
            fprintf(samples, "%d,", bi);
            fclose(samples);
    
            int error_1 = 0;
    
    	    if(ans == 1) {//We will proceed with recovering the error only when the original message is an encryption of 1 and the error in it is positive. We are targeting an encryption of 1 due to the biasness of NAND gate towards it.
    		error_1 = get_error_if_positive(sample, pos_min, pos_max, key, ans);
    		samples = fopen("samples.csv","a");
    		fprintf(samples, "%d\n", error_1);
    		fclose(samples);
    		if(error_1 != 0) {//If the recovered error is zero, the sample will be rejected as it is highly unlikely that a resultant ciphertext of a homomorphic gate computation will be error-free
    		    FILE* plaintext_ciphertext = fopen("gaussian_elimination.txt","a");
    		    fprintf(plaintext_ciphertext, "recovered_error: %d m: %u b: %u a: ", error_1, MU, sample->b);
    		    for (int32_t i = 0; i < key->params->in_out_params->n; ++i)
    		    {
    		        fprintf(plaintext_ciphertext, "%u,", sample->a[i]);
    		    }
    		    fprintf(plaintext_ciphertext, "\n");
    		    fclose(plaintext_ciphertext);
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

int main( int argc, char *argv[] ) {

    //reads the cloud key from file
    FILE* secret_key = fopen("secret.key","rb");
    TFheGateBootstrappingSecretKeySet* key = new_tfheGateBootstrappingSecretKeySet_fromFile(secret_key);
    fclose(secret_key);

    //if necessary, the params are inside the key
    const TFheGateBootstrappingParameterSet* params = key->params;

    //read the 1 ciphertext of the result
    LweSample* answer = new_LweSample(params->in_out_params);
    
    int neg_max = atoi(argv[1]);
    int neg_min = atoi(argv[2]);
    int pos_min = atoi(argv[3]);
    int pos_max = atoi(argv[4]);
    char* end;
    answer->b = strtoul(argv[5], &end, 10);

    char *temp_a = argv[6];
    
    int i = 0;

    char* tok = strtok(temp_a, ",");

    while (tok != NULL)
    {
        char* end;
        answer->a[i] = strtoul(tok, &end, 10);
        i = i + 1;
        tok = strtok(NULL, ",");
    }
    
    FILE* samples = fopen("samples.csv","a");
    int ans = bootsSymDecrypt(answer, key); printf("ans: %d\n", ans);
    fprintf(samples, "%d,", ans);
    fclose(samples);
    
    int sign = get_sign(answer, pos_min, pos_max, key, ans);
    printf("Sign of error is: %s\n", (sign == 0 ? "positive" : "negative"));

    //clean up all pointers
    delete_gate_bootstrapping_ciphertext_array(1, answer);
    delete_gate_bootstrapping_secret_keyset(key);
}

