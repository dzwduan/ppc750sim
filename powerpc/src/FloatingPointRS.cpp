/***************************************************************************
                          FloatingPointRS.cpp  -  description
                             -------------------
    begin                : Mon Jul 30 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include "FloatingPointRS.h"
#include "misc.h"
#include <string.h>

/*** FloatingPointRS ***/

FloatingPointRS::FloatingPointRS()
{
	executionSerialized = false;
	branchCounter = 0;
	memset(&operation, 0, sizeof(operation));
	memset(&tags, -1, sizeof(tags));
	full = false;
	prev = next = 0;
}

bool FloatingPointRS::Valid()
{
	unsigned int k;
	
	for(k = 0; k < sizeof(operands) / sizeof(operands[0]); k++)
	{
		if(operands[k].tag >= 0 && !operands[k].valid) return false;
	}
	return true;
}

void FloatingPointRS::UpdateFloatingPointRegister(int tag, UInt64 data)
{
	unsigned int i; /* num�ro de l'op�rande */
	
#ifdef DEBUG
	if(Debug(DebugFloatingPointUnit))
		cout << *this << endl;
#endif
	for(i = 0; i < sizeof(operands) / sizeof(operands[0]); i++)
	{
		if(!operands[i].valid && operands[i].tag == tag)
		{
#ifdef DEBUG
			if(Debug(DebugFloatingPointUnit))
			{
				cout << "FloatingPointRS : operand " << i << " get ";
				WriteHex(cout, data);
				cout << " from rf" << tag << endl;
			}
#endif
			operands[i].valid = true;
			operands[i].data = data;
		}
	}
}

ostream& operator << (ostream& os, const FloatingPointRS& rs)
{
	int i;
	cout << "FloatingPointRS(";
	for(i = 0; i < 3; i++)
	{
		if(rs.operands[i].tag >= 0)
		{
			if(rs.operands[i].valid)
				WriteHex(cout, rs.operands[i].data);
			else
				cout << "rf" << (int) rs.operands[i].tag;
		}
		else
			WriteHex(cout, rs.operands[i].data);
		if(i != 2) cout << ", ";
	}
	cout << ")";
	return os;
}

/*** FloatingPointRSQueue ***/

void FloatingPointRSQueue::UpdateFloatingPointRegister(int tag, UInt64 data)
{
	FloatingPointRS *cur_rs;
	
	for(cur_rs = oldest; cur_rs; cur_rs = cur_rs->next)
	{
		cur_rs->UpdateFloatingPointRegister(tag, data);
	}
}
