/***************************************************************************
                          XERRegister.h  -  description
                             -------------------
    begin                : Fri Jun 1 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __XERREGISTER_H__
#define __XERREGISTER_H__

#include <systemc.h>

SC_MODULE(XERRegister)
{
	sc_in<bool> inClock;

	sc_in<bool> inWriteCarry;
	sc_in<bool> inCarry;
	sc_in<bool> inWriteOverflow;
	sc_in<bool> inOverflow;
	
	sc_out<bool> outCarry;
	sc_out<bool> outOverflow;
	sc_out<bool> outSummaryOverflow;
	
	bool carry;	/* XER[CA] */
	bool overflow;	/* XER[OV] */
	bool summaryOverflow;	/* XER[SO] */
	
	void Write();
	
	void WriteOverflow(bool value);
	void WriteSummaryOverflow(bool value);
	void WriteCarry(bool value);

	SC_CTOR(XERRegister)
	{
		SC_METHOD(Write);
		sensitive_neg << inClock;
	
		carry = false;
		overflow = false;
		summaryOverflow = false;
	}	
};

#endif
