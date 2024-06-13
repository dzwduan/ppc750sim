/***************************************************************************
                          LinkRegister.cpp  -  description
                             -------------------
    begin                : Fri Jun 1 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <LinkRegister.h>

void LinkRegister::ReadRenameLR()
{
	int i;
	
	for(i = 0; i < nReadRenameLRPorts; i++)
	{
		tag_t tag = inReadRenameLRNumber[i];

#ifdef DEBUG
		if(Debug(DebugLinkRegister))
		{
			cout << name() << ": Reading ";
			WriteHex(cout, renameLR[tag]);
			cout << " from LR rename buffer #" << (int) tag << endl;
		}
		if(tag < 0 || tag >= nLRRenameBuffers)
		{
			cout << name() << ": Error : LR rename buffer number out of range" << endl;
			ABORT();
		}
#endif		
		outReadRenameLRData[i] = renameLR[tag];
	}
}

void LinkRegister::WriteBack()
{
	int i;
	
	for(i = 0; i < nWriteBackLRPorts; i++)
	{
		if((Dummy<bool>) inWriteBackLR[i])
		{
			tag_t tag = inWriteBackLRRenameNumber[i];
		
#ifdef DEBUG
		if(Debug(DebugLinkRegister))
			cout << name() << ": Moving LR rename buffer #" << (int) tag << " to LR" << endl;
		if(tag < 0 || tag >= nLRRenameBuffers)
		{
			cout << name() << ": Error : LR rename buffer number out of range" << endl;
			ABORT();
		}
#endif		
			lr = renameLR[tag];
			outLR = lr;
		}
	}
}

void LinkRegister::WriteRenameLR()
{
	int i;
	
	for(i = 0; i < nWriteRenameLRPorts; i++)
	{
		if(inWriteRenameLR[i])
		{
			tag_t tag = inWriteRenameLRNumber[i];
			
#ifdef DEBUG
		if(Debug(DebugLinkRegister))
		{
			cout << name() << ": Writing ";
			WriteHex(cout, inWriteRenameLRData[i]);
			cout << " to LR rename buffer #" << (int) tag << endl;
		}
		if(tag < 0 || tag >= nLRRenameBuffers)
		{
			cout << name() << ": LR rename buffer number out of range" << endl;
			ABORT();
		}
#endif		
			renameLR[tag] = inWriteRenameLRData[i];
		}
	}
	
	for(i = 0; i < nReadRenameLRPorts; i++)
	{
		tag_t tag = inReadRenameLRNumber[i];

#ifdef DEBUG
		if(tag < 0 || tag >= nLRRenameBuffers)
		{
			cout << name() << ": Error : LR rename buffer number out of range" << endl;
			ABORT();
		}
#endif		
		outReadRenameLRData[i] = renameLR[tag];
	}
}

void LinkRegister::OnFallingEdge()
{
	WriteRenameLR();
}

void LinkRegister::Write(UInt32 value)
{
	lr = value;
	outLR = lr;
#ifdef DEBUG
	if(Debug(DebugLinkRegister))
	{
		cout << name() << ": Writing "; WriteHex(cout, lr); cout << endl;
	}
#endif
}
