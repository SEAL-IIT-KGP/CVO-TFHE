#define _POSIX_C_SOURCE 200809L

#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>

int main() {

    //reads the cloud key from file
    FILE* cloud_key = fopen("cloud.key","rb");
    TFheGateBootstrappingCloudKeySet* bk = new_tfheGateBootstrappingCloudKeySet_fromFile(cloud_key);
    fclose(cloud_key);
 
    //if necessary, the params are inside the key
    const TFheGateBootstrappingParameterSet* params = bk->params;

    //read the 2x16 ciphertexts
    LweSample* ciphertext1 = new_gate_bootstrapping_ciphertext_array(1, params);
    LweSample* ciphertext2 = new_gate_bootstrapping_ciphertext_array(1, params);

    //reads the 2x16 ciphertexts from the cloud file
    FILE* cloud_data = fopen("cloud.data","rb");
    for (int i=0; i<1; i++) import_gate_bootstrapping_ciphertext_fromFile(cloud_data, &ciphertext1[i], params);
    for (int i=0; i<1; i++) import_gate_bootstrapping_ciphertext_fromFile(cloud_data, &ciphertext2[i], params);
    fclose(cloud_data);

    LweSample* result = new_gate_bootstrapping_ciphertext_array(1, params);

    bootsNAND(result, &ciphertext1[0], &ciphertext2[0], bk);

    //export the 32 ciphertexts to a file (for the cloud)
    FILE* answer_data = fopen("answer.data","wb");
    for (int i=0; i<1; i++) export_gate_bootstrapping_ciphertext_toFile(answer_data, &result[i], params);
    fclose(answer_data);

    //clean up all pointers
    delete_gate_bootstrapping_ciphertext_array(1, result);
    delete_gate_bootstrapping_ciphertext_array(1, ciphertext2);
    delete_gate_bootstrapping_ciphertext_array(1, ciphertext1);
    delete_gate_bootstrapping_cloud_keyset(bk);

}

