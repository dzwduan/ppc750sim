
#ifndef __REGISTERFILE_H__
#define __REGISTERFILE_H__

#include <systemc.h>
#include <common.h>

SC_MODULE(RegisterFile)
{
	sc_in<bool> inClock;

	
	UInt32 registers[nRegisters];
	UInt32 renames[nRenameBuffers];
	
	
	UInt64 floatingPointRegisters[nFloatingPointRegisters];
	UInt64 floatingPointRenames[nFloatingPointRenameBuffers];
	
	sc_in<bool> inWriteRename[nWriteRenamePorts];
	sc_in<Dummy<bool> > inWriteBack[nWriteBackPorts];
	
	sc_in<regnum_t> inReadRegisterNumber[nReadRegisterPorts];
	sc_out<UInt32> outReadRegisterData[nReadRegisterPorts];
	
	sc_in<tag_t> inReadRenameNumber[nReadRenamePorts];
	sc_out<UInt32> outReadRenameData[nReadRenamePorts];
	
	sc_in<tag_t> inWriteRenameNumber[nWriteRenamePorts];
	sc_in<UInt32> inWriteRenameData[nWriteRenamePorts];
	
	sc_in<regnum_t> inWriteBackRegisterNumber[nWriteBackPorts];
	sc_in<tag_t> inWriteBackRenameNumber[nWriteBackPorts];
	
	sc_in<bool> inWriteFloatingPointRename[nWriteFloatingPointRenamePorts];
	sc_in<Dummy<bool> > inWriteBackFloatingPoint[nWriteBackFloatingPointPorts];
	
	sc_in<regnum_t> inReadFloatingPointRegisterNumber[nReadFloatingPointRegisterPorts];
	sc_out<UInt64> outReadFloatingPointRegisterData[nReadFloatingPointRegisterPorts];
	
	sc_in<tag_t> inReadFloatingPointRenameNumber[nReadFloatingPointRenamePorts];
	sc_out<UInt64> outReadFloatingPointRenameData[nReadFloatingPointRenamePorts];
	
	sc_in<tag_t> inWriteFloatingPointRenameNumber[nWriteFloatingPointRenamePorts];
	sc_in<UInt64> inWriteFloatingPointRenameData[nWriteFloatingPointRenamePorts];
	
	sc_in<regnum_t> inWriteBackFloatingPointRegisterNumber[nWriteBackFloatingPointPorts];
	sc_in<tag_t> inWriteBackFloatingPointRenameNumber[nWriteBackFloatingPointPorts];

	void ReadRegister();
	void ReadFloatingPointRegister();
	void ReadRename();
	void ReadFloatingPointRename();
	void OnFallingEdge();
	
	void WriteRename();
	void WriteFloatingPointRename();
	void WriteBack();
	void WriteBackFloatingPoint();

	friend  ostream& operator << (ostream& os, const RegisterFile& rf);
	
	UInt32& operator [] (int regnum)
	{
		return registers[regnum];
	}
	
	SC_CTOR(RegisterFile)
	{
		int i;
		
		SC_METHOD(ReadRegister);
		for(i = 0; i < nReadRegisterPorts; i++)
			sensitive << inReadRegisterNumber[i];
			
		SC_METHOD(ReadFloatingPointRegister);
		for(i = 0; i < nReadFloatingPointRegisterPorts; i++)
			sensitive << inReadFloatingPointRegisterNumber[i];
		
		SC_METHOD(ReadRename);
		for(i = 0; i < nReadRenamePorts; i++)
			sensitive << inReadRenameNumber[i];
			
		SC_METHOD(ReadFloatingPointRename);
		for(i = 0; i < nReadFloatingPointRenamePorts; i++)
			sensitive << inReadFloatingPointRenameNumber[i];
		
		SC_METHOD(OnFallingEdge);
		sensitive_neg << inClock;
		
		SC_METHOD(WriteBack);
		for(i = 0; i < nWriteBackPorts; i++)
			sensitive << inWriteBack[i];
		
		SC_METHOD(WriteBackFloatingPoint);
		for(i = 0; i < nWriteBackFloatingPointPorts; i++)
			sensitive << inWriteBackFloatingPoint[i];
		
		for(i = 0; i < nRegisters; i++)
			registers[i] = 0;
	
		for(i = 0; i < nRenameBuffers; i++)
			renames[i] = 0;
	}
};

#endif
