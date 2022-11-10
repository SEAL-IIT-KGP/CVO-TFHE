#define _POSIX_C_SOURCE 200809L

#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>
#include <stdlib.h>

long pos_threshold = 10200547327;
uint32_t MU = 536870912;

int error = 0;
int count = 0;

int get_error_if_positive(LweSample *sample, int start, int end, TFheGateBootstrappingSecretKeySet* key, int ans) {
    
    int mid = (start + end)/2;

    int ci;
    long distance_from_threshold;
    
    if(start == end-1) { //base case for recursion
        distance_from_threshold = pos_threshold - error;
        sample->b += distance_from_threshold; 
    	ci = bootsSymDecrypt(sample, key); count = count + 1;
    	sample->b -= distance_from_threshold;
    	if(ci == ans) {
    	    printf("%d\n",error);
    	    return error; //mid will contain correct error
    	}
	else {
	    return 0;
	}
    }
    
    distance_from_threshold = pos_threshold - mid;
    
    sample->b += distance_from_threshold; 
    ci = bootsSymDecrypt(sample, key); count = count + 1;
    sample->b -= distance_from_threshold;
    
    if(ci == ans) {//if decrypted correctly
        error = mid;
    	get_error_if_positive(sample, start, mid, key, ans);
    }
    else {
    	get_error_if_positive(sample, mid, end, key, ans);
    }
}

void get_sign(LweSample *sample, int pos_min, int pos_max, TFheGateBootstrappingSecretKeySet* key, int ans) {
    long distance_from_threshold = pos_threshold - pos_min;
    FILE* samples;
    sample->b += distance_from_threshold;
    int ai = bootsSymDecrypt(sample, key); printf("ai: %d\n", ai); count = count + 1;
    sample->b -= distance_from_threshold;

    if(ans != ai) {
    
    	samples = fopen("samples.csv","a");
    	fprintf(samples, "%d,", ai);
    	fclose(samples);
    
    	sample->b -= (2*MU);
    	int bi = bootsSymDecrypt(sample, key); printf("bi: %d\n", bi); count = count + 1;
    	sample->b += (2*MU);

    	if(ans != bi) {
    
    	    samples = fopen("samples.csv","a");
            fprintf(samples, "%d,", bi);
            fclose(samples);
    
            int error_1 = 0;
    
    	    if(ans == 1) {
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
    LweSample* answer = new_gate_bootstrapping_ciphertext_array(1, params);

    //import the 32 ciphertexts from the answer file
    FILE* answer_data = fopen("answer.data","rb");

    for (int i=0; i<1; i++) 
        import_gate_bootstrapping_ciphertext_fromFile(answer_data, &answer[i], params);
    fclose(answer_data);

    long pos_min = 0;
    long pos_max = 2147483648;
 
    int ans = bootsSymDecrypt(answer, key); printf("ans: %d\n", ans);
    FILE* samples = fopen("samples.csv","a");
    fprintf(samples, "%d,", ans);
    fclose(samples);
    
    get_sign(answer, pos_min, pos_max, key, ans);

    //clean up all pointers
    delete_gate_bootstrapping_ciphertext_array(1, answer);
    delete_gate_bootstrapping_secret_keyset(key);
}
