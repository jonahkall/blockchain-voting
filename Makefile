CXX = clang++
CXXFLAGS = -std=c++11 -stdlib=libc++ -O3 -lssl -pthread -funroll-loops
#CXXFLAGS = -std=c++11 -O3 -funroll-loops

default: peer

run: peer
	./runpeer

peer: processor.o communication.o peer.cpp peer.hpp
	$(CXX) $(CXXFLAGS) -c peer.hpp peer.cpp
	$(CXX) $(CXXFLAGS) -o runpeer processor.o communication.o peer.o

processor.o: processor.cpp processor.hpp
	$(CXX) $(CXXFLAGS) -c processor.cpp

communication.o: communication.cpp communication.hpp
	$(CXX) $(CXXFLAGS) -c communication.cpp

clean:
	$(RM) *.o *.gch runpeer