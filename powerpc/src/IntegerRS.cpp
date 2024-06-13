/***************************************************************************
                          IntegerRS.cpp  -  description
                             -------------------
    begin                : Mon Mar 12 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@cea.fr, mouchard@lri.fr
 ***************************************************************************/

#include <IntegerRS.h>

/*** IntegerRS ***/

IntegerRS::IntegerRS()
{
	executionSerialized = false;
	branchCounter = 0;
	memset(&operation, 0, sizeof(operation));
	memset(&tags, -1, sizeof(tags));
	unitNumber = 0;
	full = false;
	prev = next = 0;
}

bool IntegerRS::Valid()
{
	unsigned int k;
	
	for(k = 0; k < sizeof(operands) / sizeof(operands[0]); k++)
	{
		if(operands[k].tag >= 0 && !operands[k].valid) return false;
	}
	return true;
}

void IntegerRS::UpdateIntegerRegister(int tag, UInt32 data)
{
	unsigned int i; /* num�ro de l'op�rande */
	
#ifdef DEBUG
	if(Debug(DebugIntegerRS))
		cout << *this << endl;
#endif
	for(i = 0; i < sizeof(operands) / sizeof(operands[0]); i++)
	{
		if(!operands[i].valid && operands[i].tag == tag)
		{
#ifdef DEBUG
			if(Debug(DebugIntegerRS))
			{
				cout << "IntegerRS : operand " << i << " get ";
				WriteHex(cout, data);
				cout << " from rr" << tag << endl;
			}
#endif
			operands[i].valid = true;
			operands[i].data = data;
		}
	}
}

ostream& operator << (ostream& os, const IntegerRS& rs)
{
	int i;
	cout << "IntegerRS(";
	for(i = 0; i < 2; i++)
	{
		if(rs.operands[i].tag >= 0)
		{
			if(rs.operands[i].valid)
				WriteHex(cout, rs.operands[i].data);
			else
				cout << "rr" << (int) rs.operands[i].tag;
		}
		else
			WriteHex(cout, rs.operands[i].data);
		if(i != 1) cout << ", ";
	}
	cout << ")";
	return os;
}

/*** IntegerRSQueue ***/

void IntegerRSQueue::UpdateIntegerRegister(int tag, UInt32 data)
{
	IntegerRS *cur_rs;
	
	for(cur_rs = oldest; cur_rs; cur_rs = cur_rs->next)
	{
		cur_rs->UpdateIntegerRegister(tag, data);
	}
}
