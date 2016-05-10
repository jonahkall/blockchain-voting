cd ..
sudo yum install build-essential autoconf libtool gcc-c++ openssl-devel
git clone https://github.com/grpc/grpc.git
cd grpc
git submodule update --init
make
cd third_party/protobuf/
sudo make install
cd ../../
sudo make install
cd ../blockchain-voting
export PKG_CONFIG_PATH=/home/ec2-user/grpc/libs/opt/pkgconfig/
export LD_LIBRARY_PATH=/usr/local/lib
make clean
make all
