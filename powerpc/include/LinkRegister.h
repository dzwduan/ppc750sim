/***************************************************************************
                          LinkRegister.h  -  description
                             -------------------
    begin                : Fri Jun 1 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __LINKREGISTER_H__
#define __LINKREGISTER_H__

#include <systemc.h>
#include <common.h>

SC_MODULE(LinkRegister)
{
	sc_in<bool> inClock;
	
	sc_in<tag_t> inReadRenameLRNumber[nReadRenameLRPorts];
	sc_out<UInt32> outReadRenameLRData[nReadRenameLRPorts];
	
	sc_in<bool> inWriteRenameLR[nWriteRenameLRPorts];
	sc_in<tag_t> inWriteRenameLRNumber[nWriteRenameLRPorts];
	sc_in<UInt32> inWriteRenameLRData[nWriteRenameLRPorts];
	sc_out<UInt32> outLR;
	
	sc_in<Dummy<bool> > inWriteBackLR[nWriteBackLRPorts];
	sc_in<tag_t> inWriteBackLRRenameNumber[nWriteBackLRPorts];
	
	UInt32 lr;
	UInt32 renameLR[nLRRenameBuffers];
	
	void ReadRenameLR();
	
	void WriteBack();
	void WriteRenameLR();
	void OnFallingEdge();

	void Write(UInt32 value);
	
	SC_CTOR(LinkRegister)
	{
		int i;
	
		SC_METHOD(ReadRenameLR);
		for(i = 0; i < nReadRenameLRPorts; i++)
			sensitive << inReadRenameLRNumber[i];
			
		SC_METHOD(OnFallingEdge);
		sensitive_neg << inClock;
		
		SC_METHOD(WriteBack);
		for(i = 0; i < nWriteBackLRPorts; i++)
			sensitive << inWriteBackLR[i];
		
		lr = 0;
		for(i = 0; i < nLRRenameBuffers; i++)
			renameLR[i] = 0;
	}
};

#endif
