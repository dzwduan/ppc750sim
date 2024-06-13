SHELL=/bin/sh

######################################################
#                                                    #
# SYSTEMC_DIR = directory where SystemC is installed #
#                                                    #
######################################################

#SYSTEMC_DIR=/usr/local/systemc-1.0.2
SYSTEMC_DIR=/usr/local/systemc-3.0.0

######################################################
#                                                    #
# ARCH = architecture running the simulator          #
#                                                    #
# could be one of the following :                    #
#  - gccsparcOS5                                     #
#  - hpux10                                          #
#  - linux                                           #
#  - sparcOS5                                        #
#                                                    #
######################################################

ARCH=linux

######################################################
#                                                    #
# PPCEMUL_DIR = directory where the powerpc          #
#               emulator is installed                #
#                                                    #
######################################################

PPCEMUL_DIR=../../ppcemul

TARGET=../bin/powerpc
CXX=g++

######################################################
#                                                    #
# CXXFLAGS = flags for the C++ compiler              #
# You must add -DEMULATE if you want to check the    #
# results of simulator.                              #
# You must add -DDEBUG if you want some debugging    #
# informations during the simulation                 #
# You must add -DTRACE if you want to dump an XML    #
# trace of the execution                             #
#                                                    #
######################################################

################## For SystemC 1.x ###################
#CXXFLAGS=-O3 -fomit-frame-pointer -fexpensive-optimizations -fstrength-reduce -fschedule-insns2 -funroll-loops
#LIBS=-L$(SYSTEMC_DIR)/lib-$(ARCH) -L$(PPCEMUL_DIR)/lib -lsystemc -lnumeric_bit -lqt -lppcemul -lstdc++
######################################################

################## For SystemC 2.x ###################
CXXFLAGS=-DSC2 -O3 -fomit-frame-pointer -fexpensive-optimizations -fstrength-reduce -fschedule-insns2 -funroll-loops
LIBS=-L$(SYSTEMC_DIR)/lib/x86_64-linux-gnu -L$(PPCEMUL_DIR)/lib -lsystemc -lppcemul -lstdc++
######################################################

INCLUDES=-I. -I../include -I$(SYSTEMC_DIR)/include -I$(PPCEMUL_DIR)/include

SOURCES= \
./BHT.cpp \
./DCache.cpp \
./LoadStoreUnit.cpp \
./BranchPredictionUnit.cpp \
./DispatchUnit.cpp \
./PowerPC.cpp \
./BranchRS.cpp \
./FetchUnit.cpp \
./RegisterFile.cpp \
./BranchUnit.cpp \
./FloatingPointRS.cpp \
./RegistersBinding.cpp \
./BTIC.cpp \
./FloatingPointUnit.cpp \
./ReorderBuffer.cpp \
./BusInterfaceUnit.cpp \
./ICache.cpp \
./StoreQueue.cpp \
./Cache.cpp \
./InstructionQueue.cpp \
./SystemRegisterRS.cpp \
./common.cpp \
./IntegerRS.cpp \
./SystemRegisterUnit.cpp \
./CompletionUnit.cpp \
./IntegerUnit.cpp \
./XERRegister.cpp \
./ConditionRegister.cpp \
./LinkRegister.cpp \
./CountRegister.cpp \
./LoadStoreRS.cpp \
./trace.cpp \
./Simulator.cpp \
./main.cpp

OBJS=$(SOURCES:.cpp=.o)
DEPS=$(SOURCES:.cpp=.d)

all: $(TARGET)
	@echo "The PowerPC Simulator has been successfully built"

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)

%.d: %.cpp
	set -e; $(CXX) -M $(CXXFLAGS) $(INCLUDES) $< \
                  | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
                [ -s $@ ] || rm -f $@

include $(DEPS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@


$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LIBS) -o $(TARGET)
