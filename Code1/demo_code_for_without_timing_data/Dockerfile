FROM python:3.8

RUN apt-get update && apt-get -y install cmake protobuf-compiler
RUN git clone https://github.com/tfhe/tfhe.git
RUN cd tfhe && make && make install
RUN apt-get -y install python2
RUN apt-get -y install python3
RUN apt-get -y install sagemath

COPY user_testing.c cloud_testing.c verify_testing.c secret_key_testing.txt equation_solver.py clean_temporary_files.py ./

RUN gcc user_testing.c -o user_testing -ltfhe-spqlios-fma
RUN gcc cloud_testing.c -o cloud_testing -ltfhe-spqlios-fma
RUN gcc verify_testing.c -o verify_testing -ltfhe-spqlios-fma
