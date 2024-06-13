/***************************************************************************
                          CountRegister.cpp  -  description
                             -------------------
    begin                : Fri Jun 1 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <CountRegister.h>

void CountRegister::ReadRenameCTR()
{
	int i;
	
	for(i = 0; i < nReadRenameCTRPorts; i++)
	{
		tag_t tag = inReadRenameCTRNumber[i];
#ifdef DEBUG
		if(Debug(DebugCountRegister))
		{
			cout << name() << ": Reading ";
			WriteHex(cout, renameCTR[tag]);
			cout << " into CTR rename buffer #" << (int) tag << endl;
		}
		if(tag < 0 || tag >= nCTRRenameBuffers)
		{
			cout << name() << ": Error : CTR rename buffer number out of range" << endl;
			ABORT();
		}
#endif
		outReadRenameCTRData[i] = renameCTR[tag];
	}
}

void CountRegister::WriteBack()
{
	int i;
	
	for(i = 0; i < nWriteBackCTRPorts; i++)
	{
		if((Dummy<bool>) inWriteBackCTR[i])
		{
			tag_t tag = inWriteBackCTRRenameNumber[i];
#ifdef DEBUG
			if(Debug(DebugCountRegister))
				cout << name() << ": Moving CTR rename buffer #" << (int) tag << " to CTR" << endl;
			if(tag < 0 || tag >= nCTRRenameBuffers)
			{
				cout << name() << ": Error : CTR rename buffer number out of range" << endl;
				ABORT();
			}
#endif		
			ctr = renameCTR[tag];
			outCTR = ctr;
		}
	}
}

void CountRegister::WriteRenameCTR()
{
	int i;
	
	for(i = 0; i < nWriteRenameCTRPorts; i++)
	{
		if(inWriteRenameCTR[i])
		{
			tag_t tag = inWriteRenameCTRNumber[i];
#ifdef DEBUG
			if(Debug(DebugCountRegister))
			{
				cout << name() << ": Writing ";
				WriteHex(cout, inWriteRenameCTRData[i]);
				cout << " to CTR rename buffer #" << (int) tag << endl;
			}
			if(tag < 0 || tag >= nCTRRenameBuffers)
			{
				cout << name() << ": CTR rename buffer number out of range" << endl;
				ABORT();
			}
#endif		
			renameCTR[tag] = inWriteRenameCTRData[i];
		}
	}
	
	for(i = 0; i < nReadRenameCTRPorts; i++)
	{
		tag_t tag = inReadRenameCTRNumber[i];
#ifdef DEBUG
		if(tag < 0 || tag >= nCTRRenameBuffers)
		{
			cout << name() << ": Error : CTR rename buffer number out of range" << endl;
			ABORT();
		}
#endif
		outReadRenameCTRData[i] = renameCTR[tag];
	}
}

void CountRegister::OnFallingEdge()
{
	WriteRenameCTR();
}

void CountRegister::Write(UInt32 value)
{
	ctr = value;
	outCTR = ctr;
#ifdef DEBUG
	if(Debug(DebugCountRegister))
	{
		cout << name() << ": Writing "; WriteHex(cout, ctr); cout << endl;
	}
#endif
}

