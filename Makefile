CXX = clang++
CXXFLAGS = -O3 -lssl -lcrypto -funroll-loops
LDFLAGS = -Wno-deprecated-declarations -std=c++11 -stdlib=libc++

default: peer

run: peer
	./runpeer

peer: processor.o communication.o peer.cpp peer.hpp rsa.cpp rsa.hpp
	$(CXX) $(LDFLAGS) -c peer.hpp peer.cpp rsa.cpp rsa.hpp 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o runpeer processor.o communication.o peer.o rsa.o

processor.o: processor.cpp processor.hpp
	$(CXX) $(LDFLAGS) -c processor.cpp

communication.o: communication.cpp communication.hpp
	$(CXX) $(LDFLAGS) -c communication.cpp

clean:
	$(RM) *.o *.gch runpeer