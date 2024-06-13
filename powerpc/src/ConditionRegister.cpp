/***************************************************************************
                          ConditionRegister.cpp  -  description
                             -------------------
    begin                : Thu May 31 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@CRi.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <ConditionRegister.h>

void ConditionRegister::ReadRenameCR()
{
	int i;
	
	for(i = 0; i < nReadRenameCRPorts; i++)
	{
		tag_t tag = inReadRenameCRNumber[i];
#ifdef DEBUG
		if(Debug(DebugConditionRegister))
			cout << name() << ": Reading CR rename buffer #" << (int) tag << endl;
		if(tag < 0 || tag >= nCRRenameBuffers)
		{
			cout << name() << ": Error : CR rename buffer number out of range" << endl;
			ABORT();
		}			
#endif
		outReadRenameCRData[i] = renameCR[tag];
	}
}

void ConditionRegister::WriteBack()
{
	int i;
	
	for(i = 0; i < nWriteBackCRPorts; i++)
	{
		if((Dummy<bool>) inWriteBackCR[i])
		{
			tag_t tag = inWriteBackCRRenameNumber[i];
			fieldnum_t crf = inWriteBackCRFieldNumber[i];
#ifdef DEBUG
			if(Debug(DebugConditionRegister))
			{
				cout << name() << ": moving CR rename buffer #" << (int) tag << " ("; WriteHex(cout, renameCR[tag]); cout << ") into CR" << (int) crf << endl;
			}
			if(tag < 0 || tag >= nCRRenameBuffers)
			{
				cout << name() << ": Error : CR rename buffer number out of range" << endl;
				ABORT();
			}
			if(crf < 0 || crf >= 8)
			{
				cout << name() << ": Error : CR field number out of range" << endl;
				ABORT();
			}
#endif
		
			UInt32 mask = ~(0xf << (4 * (7 - crf)));
			if(inSummaryOverflow) renameCR[tag] |= 1;
			cr = (cr & mask) | (renameCR[tag] << (4 * (7 - crf)));
			outCR = cr;
#ifdef DEBUG
			if(Debug(DebugConditionRegister))
			{
				cout << name() << ": CR = "; WriteHex(cout, cr); cout << endl;
			}
#endif
		}
	}
}

void ConditionRegister::WriteRenameCR()
{
	int i;
	
	for(i = 0; i < nWriteRenameCRPorts; i++)
	{
		if(inWriteRenameCR[i])
		{
			tag_t tag = inWriteRenameCRNumber[i];
#ifdef DEBUG
			if(Debug(DebugConditionRegister))
				cout << name() << ": Writing " << inWriteRenameCRData[i] << " into CR rename buffer #" << (int) tag << endl;
			if(tag < 0 || tag >= nCRRenameBuffers)
			{
				cout << name() << ": Error : CR rename buffer number out of range" << endl;
				ABORT();
			}
#endif			
			renameCR[tag] = inWriteRenameCRData[i];
		}
	}
	
	for(i = 0; i < nReadRenameCRPorts; i++)
	{
		tag_t tag = inReadRenameCRNumber[i];
#ifdef DEBUG
		if(tag < 0 || tag >= nCRRenameBuffers)
		{
			cout << name() << ": Error : CR rename buffer number out of range" << endl;
			ABORT();
		}			
#endif
		outReadRenameCRData[i] = renameCR[tag];
	}
}

void ConditionRegister::OnFallingEdge()
{
	WriteRenameCR();
}

void ConditionRegister::Write(UInt32 value)
{
	cr = value;
	outCR = cr;
#ifdef DEBUG
	if(Debug(DebugConditionRegister))
	{
		cout << name() << ": Writing "; WriteHex(cout, cr); cout << "(non masked write)" << endl;
	}
#endif
}

void ConditionRegister::WriteCR()
{
	if(inWriteCR)
	{
		UInt32 mask = 0;
		UInt8 crm = inCRM;
		if(crm & 1) mask |= 0xf;
		if(crm & 2) mask |= 0xf << 4;
		if(crm & 4) mask |= 0xf << 8;
		if(crm & 8) mask |= 0xf << 12;
		if(crm & 16) mask |= 0xf << 16;
		if(crm & 32) mask |= 0xf << 20;
		if(crm & 64) mask |= 0xf << 24;
		if(crm & 128) mask |= 0xf << 28;
		cr = (cr & (~mask)) | (inCR & mask);
		outCR = cr;
#ifdef DEBUG
		if(Debug(DebugConditionRegister))
		{
			cout << name() << ": Writing "; WriteHex(cout, cr); cout << " (masked write)" << endl;
		}
#endif
	}
}
