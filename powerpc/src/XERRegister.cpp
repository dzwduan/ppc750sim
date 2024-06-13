/***************************************************************************
                          XERRegister.cpp  -  description
                             -------------------
    begin                : Fri Jun 1 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <XERRegister.h>

void XERRegister::Write()
{
	if(inWriteCarry)
	{
		carry = inCarry;
		outCarry = carry;
	}
	
	if(inWriteOverflow)
	{
		overflow = inOverflow;
		if(overflow) summaryOverflow = true;
		outOverflow = overflow;
		outSummaryOverflow = summaryOverflow;
	}
}

void XERRegister::WriteOverflow(bool value)
{
	overflow = value;
	outOverflow = overflow;
}

void XERRegister::WriteSummaryOverflow(bool value)
{
	summaryOverflow = value;
	outSummaryOverflow = summaryOverflow;
}

void XERRegister::WriteCarry(bool value)
{
	carry = value;
	outCarry = carry;
}
