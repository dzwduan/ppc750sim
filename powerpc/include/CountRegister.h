/***************************************************************************
                          CountRegister.h  -  description
                             -------------------
    begin                : Fri Jun 1 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __COUNTREGISTER_H__
#define __COUNTREGISTER_H__

#include <systemc.h>
#include <common.h>

SC_MODULE(CountRegister)
{
	sc_in<bool> inClock;
	
	sc_in<tag_t> inReadRenameCTRNumber[nReadRenameCTRPorts];
	sc_out<UInt32> outReadRenameCTRData[nReadRenameCTRPorts];
	
	sc_in<bool> inWriteRenameCTR[nWriteRenameCTRPorts];
	sc_in<tag_t> inWriteRenameCTRNumber[nWriteRenameCTRPorts];
	sc_in<UInt32> inWriteRenameCTRData[nWriteRenameCTRPorts];
	sc_out<UInt32> outCTR;
	
	sc_in<Dummy<bool> > inWriteBackCTR[nWriteBackCTRPorts];
	sc_in<tag_t> inWriteBackCTRRenameNumber[nWriteBackCTRPorts];
	
	UInt32 ctr;
	UInt32 renameCTR[nCTRRenameBuffers];
	
	void ReadRenameCTR();
	
	void WriteBack();
	void WriteRenameCTR();
	void OnFallingEdge();
	
	void Write(UInt32 value);

	SC_CTOR(CountRegister)
	{
		int i;
	
		SC_METHOD(ReadRenameCTR);
		for(i = 0; i < nReadRenameCTRPorts; i++)
			sensitive << inReadRenameCTRNumber[i];
			
		SC_METHOD(OnFallingEdge);
		sensitive_neg << inClock;
		
		SC_METHOD(WriteBack);
		for(i = 0; i < nWriteBackCTRPorts; i++)
			sensitive << inWriteBackCTR[i];
	
		ctr = 0;
		for(i = 0; i < nCTRRenameBuffers; i++)
			renameCTR[i] = 0;
	}
};

#endif
