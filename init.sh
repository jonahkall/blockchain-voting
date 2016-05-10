cd ..
yum install build-essential autoconf libtool gcc-c++ openssl-devel
git clone https://github.com/grpc/grpc.git
cd grpc
git submodule update --init
make
cd third_party/protobuf/
make install
cd ../../
make install
export PKG_CONFIG_PATH=/home/ec2-user/grpc/libs/opt/pkgconfig/
export LD_LIBRARY_PATH=/usr/local/lib
cd ../blockchain-voting
make clean
make all
