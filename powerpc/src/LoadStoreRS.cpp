/***************************************************************************
                          LoadStoreRS.cpp  -  description
                             -------------------
    begin                : Fri Mar 16 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@cea.fr, mouchard@lri.fr
 ***************************************************************************/

#include <LoadStoreRS.h>

LoadStoreRS::LoadStoreRS()
{
	branchCounter = 0;
	memset(&operation, 0, sizeof(operation));
	memset(&tags, -1, sizeof(tags));
}

void LoadStoreRS::UpdateIntegerRegister(int tag, UInt32 data)
{
	int i; /* numéro de l'opérande */

	if(!operation.floatingPoint)
	{
		if(!operand0.valid && operand0.tag == tag)
		{
#ifdef DEBUG
			if(Debug(DebugLoadStoreRS))
			{
				cout << "LoadStoreRS : operand 0 get ";
				WriteHex(cout, data);
				cout << " from rr" << (int) tag << endl;
			}
#endif
			operand0.valid = true;
			operand0.data = data;
		}
	}
	
	if(!operand1.valid && operand1.tag == tag)
	{
#ifdef DEBUG
		if(Debug(DebugLoadStoreRS))
		{
			cout << "LoadStoreRS : operand 1 get ";
			WriteHex(cout, data);
			cout << " from rr" << (int) tag << endl;
		}
#endif
		operand1.valid = true;
		operand1.data = data;
	}
	
	if(!operand2.valid && operand2.tag == tag)
	{
#ifdef DEBUG
		if(Debug(DebugLoadStoreRS))
		{
			cout << "LoadStoreRS : operand 2 get ";
			WriteHex(cout, data);
			cout << " from rr" << (int) tag << endl;
		}
#endif
		operand2.valid = true;
		operand2.data = data;
	}
}

void LoadStoreRS::UpdateFloatingPointRegister(int tag, UInt64 data)
{
	if(operation.floatingPoint)
	{	
		if(!operand0.valid && operand0.tag == tag)
		{
#ifdef DEBUG
			if(Debug(DebugLoadStoreRS))
			{
				cout << "LoadStoreRS : operand0 get ";
				WriteHex(cout, data);
				cout << " from rfp" << tag << endl;
			}
#endif
			operand0.valid = true;
			operand0.data = data;
		}
	}
}

bool LoadStoreRS::Valid()
{
	if(operand0.tag >= 0 && !operand0.valid) return false;
	if(operand1.tag >= 0 && !operand1.valid) return false;
	if(operand2.tag >= 0 && !operand2.valid) return false;
	return true;
}

ostream& operator << (ostream& os, const LoadStoreRS& rs)
{
	int i;
	cout << "LoadStoreRS(";
	if(rs.operand0.tag >= 0)
	{
		if(rs.operand0.valid)
		{
			WriteHex(cout, rs.operand0.data);
		}
		else
		{
			if(rs.operation.floatingPoint)
				os << "rfp" << (int) rs.operand0.tag;
			else
				os << "rr" << (int) rs.operand0.tag;
		}
	}
	else WriteHex(cout, rs.operand0.data);
	
	cout << ", ";
	
	if(rs.operand1.tag >= 0)
	{
		if(rs.operand1.valid)
			WriteHex(cout, rs.operand1.data);
		else
			cout << "rr" << (int) rs.operand1.tag;
	}
	else WriteHex(cout, rs.operand1.data);
		
	cout << ", ";
	
	if(rs.operand2.tag >= 0)
	{
		if(rs.operand2.valid)
			WriteHex(cout, rs.operand2.data);
		else
			cout << "rr" << (int) rs.operand2.tag;
	}
	else WriteHex(cout, rs.operand2.data);
		
	cout << ")";
	return os;
}
	
/*** LoadStoreRSQueue ***/

void LoadStoreRSQueue::UpdateIntegerRegister(int tag, UInt32 data)
{
	int i; /* numéro de station de réservation */
	int n;
	
	for(i = tail, n = size; n > 0; i = (i + 1) % nLoadStoreRS, n--)
	{
		queue[i].UpdateIntegerRegister(tag, data);
	}
}

void LoadStoreRSQueue::UpdateFloatingPointRegister(int tag, UInt64 data)
{
	int i; /* numéro de station de réservation */
	int n;
	
	for(i = tail, n = size; n > 0; i = (i + 1) % nLoadStoreRS, n--)
	{
		queue[i].UpdateFloatingPointRegister(tag, data);
	}
}

void LoadStoreRSQueue::DecrementBranchCounter()
{
	int i; /* numéro de station de réservation */
	int n;
	
	for(i = tail, n = size; n > 0; i = (i + 1) % nLoadStoreRS, n--)
	{
		if(queue[i].branchCounter > 0) queue[i].branchCounter--;
	}
}
