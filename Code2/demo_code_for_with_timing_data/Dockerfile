FROM python:3.8

RUN apt-get update && apt-get -y install cmake protobuf-compiler
RUN git clone https://github.com/tfhe/tfhe.git
RUN cd tfhe && make && make install
RUN apt-get -y install python2
RUN apt-get -y install python3
RUN apt-get -y install sagemath

COPY keygen.c verify_testing_unknown_plaintext.c template_error_clusters_for_NAND.txt testing_data_for_HomNAND_without_AVX.txt secret_key_testing.txt obtain_error_using_decryption_failure.py equation_solver.py clean_temporary_files.py ./

RUN gcc keygen.c -o keygen -ltfhe-spqlios-fma
RUN gcc verify_testing_unknown_plaintext.c -o verify_testing -ltfhe-spqlios-fma
