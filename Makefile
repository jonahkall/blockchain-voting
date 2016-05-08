CXX = clang++
CXXFLAGS = -std=c++11 -Wno-deprecated-declarations -stdlib=libc++ -O3 -lssl -lcrypto -pthread -funroll-loops
#CXXFLAGS = -std=c++11 -O3 -funroll-loops

default: peer

run: peer
	./runpeer

peer: processor.o communication.o peer.cpp peer.hpp rsa.cpp rsa.hpp
	$(CXX) $(CXXFLAGS) -c peer.hpp peer.cpp rsa.cpp rsa.hpp 
	$(CXX) $(CXXFLAGS) -o runpeer processor.o communication.o peer.o rsa.o

processor.o: processor.cpp processor.hpp
	$(CXX) $(CXXFLAGS) -c processor.cpp

communication.o: communication.cpp communication.hpp
	$(CXX) $(CXXFLAGS) -c communication.cpp

clean:
	$(RM) *.o *.gch runpeer