#
# Copyright 2015, Google Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

CXX = g++ -g
CPPFLAGS += -I/usr/local/include -pthread
CXXFLAGS += -std=c++11
LDFLAGS += -L/usr/local/lib `pkg-config --libs grpc++` -lpthread -ldl -lssl -lcrypto
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

PROTOS_PATH = protos

vpath %.proto $(PROTOS_PATH)

default: miner voting-booth

all: miner voting-booth

deps: node.pb.o node.grpc.pb.o server.cpp server.hpp processor.cpp processor.hpp encoding_helpers.cpp encoding_helpers.hpp client.cpp client.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c server.cpp server.hpp processor.cpp processor.hpp encoding_helpers.cpp encoding_helpers.hpp client.cpp client.hpp

miner: deps peer.cpp peer.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c peer.cpp peer.hpp
	$(CXX) -o runpeer2 $(LDFLAGS) peer.o client.o server.o processor.o encoding_helpers.o node.pb.o node.grpc.pb.o -lprotobuf

voting-booth: deps voting-booth.cpp voting-booth.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c voting-booth.cpp voting-booth.hpp
	$(CXX) -o voting-booth $(LDFLAGS) voting-booth.o client.o server.o processor.o encoding_helpers.o node.pb.o node.grpc.pb.o -lprotobuf
# all: node.pb.o node.grpc.pb.o server.cpp server.hpp processor.cpp processor.hpp encoding_helpers.cpp encoding_helpers.hpp peer.cpp peer.hpp client.cpp client.hpp
# 	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c server.cpp server.hpp processor.cpp processor.hpp encoding_helpers.cpp encoding_helpers.hpp peer.cpp peer.hpp client.cpp client.hpp
# 	$(CXX) -o runpeer2 $(LDFLAGS) peer.o client.o server.o processor.o encoding_helpers.o node.pb.o node.grpc.pb.o -lprotobuf
#dummy: server client dummy.cpp
#	$(CXX) -c dummy.cpp
#	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o dummy dummy.o server.o client.o

#client: node.pb.o node.grpc.pb.o client.o encoding_helpers.o
#	$(CXX) $^ $(LDFLAGS) -o $@

#server: node.pb.o node.grpc.pb.o server.o encoding_helpers.o
#	$(CXX) $^ $(LDFLAGS) -o $@

#encoding_helpers.o: encoding_helpers.cpp encoding_helpers.hpp
#	$(CXX) $^ $(LDFLAGS) $(CXXFLAGS) -c encoding_helpers.cpp encoding_helpers.hpp



.PRECIOUS: %.grpc.pb.cc
%.grpc.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=. $<

clean:
	rm -f *.o *.pb.cc *.pb.h *.gch client server


# The following is to test your system and ensure a smoother experience.
# They are by no means necessary to actually compile a grpc-enabled software.

PROTOC_CMD = which $(PROTOC)
PROTOC_CHECK_CMD = $(PROTOC) --version | grep -q libprotoc.3
PLUGIN_CHECK_CMD = which $(GRPC_CPP_PLUGIN)
HAS_PROTOC = $(shell $(PROTOC_CMD) > /dev/null && echo true || echo false)
ifeq ($(HAS_PROTOC),true)
HAS_VALID_PROTOC = $(shell $(PROTOC_CHECK_CMD) 2> /dev/null && echo true || echo false)
endif
HAS_PLUGIN = $(shell $(PLUGIN_CHECK_CMD) > /dev/null && echo true || echo false)

SYSTEM_OK = false
ifeq ($(HAS_VALID_PROTOC),true)
ifeq ($(HAS_PLUGIN),true)
SYSTEM_OK = true
endif
endif

system-check:
ifneq ($(HAS_VALID_PROTOC),true)
	@echo " DEPENDENCY ERROR"
	@echo
	@echo "You don't have protoc 3.0.0 installed in your path."
	@echo "Please install Google protocol buffers 3.0.0 and its compiler."
	@echo "You can find it here:"
	@echo
	@echo "   https://github.com/google/protobuf/releases/tag/v3.0.0-beta-2"
	@echo
	@echo "Here is what I get when trying to evaluate your version of protoc:"
	@echo
	-$(PROTOC) --version
	@echo
	@echo
endif
ifneq ($(HAS_PLUGIN),true)
	@echo " DEPENDENCY ERROR"
	@echo
	@echo "You don't have the grpc c++ protobuf plugin installed in your path."
	@echo "Please install grpc. You can find it here:"
	@echo
	@echo "   https://github.com/grpc/grpc"
	@echo
	@echo "Here is what I get when trying to detect if you have the plugin:"
	@echo
	-which $(GRPC_CPP_PLUGIN)
	@echo
	@echo
endif
ifneq ($(SYSTEM_OK),true)
	@false
endif
