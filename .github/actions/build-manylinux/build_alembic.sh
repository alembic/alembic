#!/bin/bash

# Debugging options
set -e -x

# Download dependencies from package server
yum update -y && yum install -y wget cmake3 git zlib-devel curl openssl-devel

# change into home dir
cd /home/

# Download and build dependencies of alembic

# Download, build and install python
wget https://www.python.org/ftp/python/2.7.18/Python-2.7.18.tgz
tar -xzf Python-2.7.18.tgz
cd Python-2.7.18
./configure --enable-optimizations --enable-shared --enable-unicode=ucs4
make altinstall
cd ..
curl "https://bootstrap.pypa.io/get-pip.py" -o "get-pip.py"
python2.7 get-pip.py
python2.7 -m pip install numpy

# Boost 1.55
wget https://sourceforge.net/projects/boost/files/boost/1.55.0/boost_1_55_0.tar.gz/download -O boost.tar.gz
tar -xzf boost.tar.gz
cd boost_1_55_0
./bootstrap.sh --with-libraries=program_options,python --with-python=/usr/local/bin/python2.7
./b2 install
cd .. && rm -rf boost_1_55_0 boost.tar.gz

# Ilmbase
wget http://download.savannah.gnu.org/releases/openexr/ilmbase-2.2.0.tar.gz
tar -xzf ilmbase-2.2.0.tar.gz
cd ilmbase-2.2.0
./configure
make install -j 4
cd .. && rm -rf ilmbase-2.2.0 ilmbase.2.2.0.tar.gz

# PyIlmBase
wget http://download.savannah.gnu.org/releases/openexr/pyilmbase-2.2.0.tar.gz
tar -xzf pyilmbase-2.2.0.tar.gz
cd pyilmbase-2.2.0
PYTHON=/usr/local/bin/python2.7 ./configure 
make -j 4 && make install
cd .. && rm -rf pyilmbase-2.2.0 pyilmbase.2.2.0.tar.gz

# Build Alembic
cd /github/workspace/  # Default location of packages in docker action
python2.7 setup.py bdist_wheel

# Bundle external shared libraries into the wheels
find . -type f -iname "*-linux*.whl" -execdir sh -c "auditwheel repair '{}' -w ./ --plat '${PLAT}' || { echo 'Repairing wheels failed.'; auditwheel show '{}'; exit 1; }" \;

echo "Succesfully build wheels:"
find . -type f -iname "*-manylinux*.whl"
