#!/bin/bash
apt-get update
apt-get -y install cmake protobuf-compiler
git clone https://github.com/tfhe/tfhe.git
apt-get -y install python3
apt-get -y install sagemath
