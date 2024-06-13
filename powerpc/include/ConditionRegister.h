/***************************************************************************
                          ConditionRegister.h  -  description
                             -------------------
    begin                : Thu May 31 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __CONDITIONREGISTER_H__
#define __CONDITIONREGISTER_H__

#include <systemc.h>
#include <common.h>

SC_MODULE(ConditionRegister)
{
	sc_in<bool> inClock;

	sc_in<tag_t> inReadRenameCRNumber[nReadRenameCRPorts];
	sc_out<UInt8> outReadRenameCRData[nReadRenameCRPorts];
	
	sc_in<bool> inWriteRenameCR[nWriteRenameCRPorts];
	sc_in<tag_t> inWriteRenameCRNumber[nWriteRenameCRPorts];
	sc_in<UInt8> inWriteRenameCRData[nWriteRenameCRPorts];
	
	sc_out<UInt32> outCR;
	
	sc_in<Dummy<bool> > inWriteBackCR[nWriteBackCRPorts];
	sc_in<tag_t> inWriteBackCRRenameNumber[nWriteBackCRPorts];
	sc_in<fieldnum_t> inWriteBackCRFieldNumber[nWriteBackCRPorts];
	
	UInt32 cr;
	UInt8 renameCR[nCRRenameBuffers];
	
	sc_in<bool> inSummaryOverflow; /* XER[SO] */
	
	sc_in<bool> inWriteCR;
	sc_in<UInt32> inCR;
	sc_in<UInt8> inCRM;
	
	void ReadRenameCR();
	void WriteRenameCR();
	void WriteBack();
	void OnFallingEdge();
	void WriteCR();
	void Write(UInt32 value);
	
	SC_CTOR(ConditionRegister)
	{
		int i;
		
		SC_METHOD(OnFallingEdge);
		sensitive_neg << inClock;
		
		SC_METHOD(ReadRenameCR);
		for(i = 0; i < nReadRenameCRPorts; i++)
			sensitive << inReadRenameCRNumber[i];
			
		SC_METHOD(WriteCR);
		sensitive_neg << inClock;
		
		SC_METHOD(WriteBack);
		for(i = 0; i < nWriteBackCRPorts; i++)
			sensitive << inWriteBackCR[i];
		
		cr = 0;
		for(i = 0; i < nCRRenameBuffers; i++)
			renameCR[i] = 0;
	}
};

#endif
