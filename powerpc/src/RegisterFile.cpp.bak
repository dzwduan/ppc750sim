/***************************************************************************
                          RegisterFile.cpp  -  description
                             -------------------
    begin                : Fri Apr 20 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <RegisterFile.h>

void RegisterFile::ReadRegister()
{
	int i;
	
	for(i = 0; i < nReadRegisterPorts; i++)
	{
#ifdef DEBUG
		if(Debug(DebugRegisterFile))
		{
			cout << name() << ": Reading ";
			WriteHex(cout, registers[inReadRegisterNumber[i]]);
			cout << " from r" << (int) inReadRegisterNumber[i] << endl;
			if(inReadRegisterNumber[i] < 0 || inReadRegisterNumber[i] >= nRegisters)
			{
				cout << name() << ": Error : register number out of range" << endl;
				ABORT();
			}
		}
#endif
		outReadRegisterData[i] = registers[inReadRegisterNumber[i]];
	}
}

void RegisterFile::ReadFloatingPointRegister()
{
	int i;
	
	for(i = 0; i < nReadFloatingPointRegisterPorts; i++)
	{
#ifdef DEBUG
		if(Debug(DebugRegisterFile))
		{
			cout << name() << ": Reading ";
			WriteHex(cout, floatingPointRegisters[inReadFloatingPointRegisterNumber[i]]);
			cout << " from r" << (int) inReadFloatingPointRegisterNumber[i] << endl;
		}
		if(inReadFloatingPointRegisterNumber[i] < 0 || inReadFloatingPointRegisterNumber[i] >= nFloatingPointRegisters)
		{
			cout << name() << ": Error : floating point register number out of range" << endl;
			ABORT();
		}
#endif
		outReadFloatingPointRegisterData[i] = floatingPointRegisters[inReadFloatingPointRegisterNumber[i]];
	}
}

void RegisterFile::ReadRename()
{
	int i;
	
	for(i = 0; i < nReadRenamePorts; i++)
	{
		tag_t tag = inReadRenameNumber[i];
		
#ifdef DEBUG
		if(Debug(DebugRegisterFile))
		{
			cout << name() << ": Reading ";
			WriteHex(cout, renames[tag]);
			cout << " from rr" << (int) tag << endl;
		}
		if(tag < 0 || tag >= nRenameBuffers)
		{
			cout << name() << ": Error : rename buffer number out of range" << endl;
			ABORT();
		}
#endif
		outReadRenameData[i] = renames[tag];
	}
}

void RegisterFile::ReadFloatingPointRename()
{
	int i;
	
	for(i = 0; i < nReadFloatingPointRenamePorts; i++)
	{
		tag_t tag = inReadFloatingPointRenameNumber[i];
		
#ifdef DEBUG
		if(Debug(DebugRegisterFile))
		{
			cout << name() << ": Reading ";
			WriteHex(cout, floatingPointRenames[tag]);
			cout << " from rfp" << (int) tag << endl;
		}
		if(tag < 0 || tag >= nFloatingPointRenameBuffers)
		{
			cout << name() << ": Error : floating point rename buffer number out of range" << endl;
			ABORT();
		}
#endif
		outReadFloatingPointRenameData[i] = floatingPointRenames[tag];
	}
}

void RegisterFile::WriteRename()
{
	int i;
	
	for(i = 0; i < nWriteRenamePorts; i++)
	{
		if(inWriteRename[i])
		{
			tag_t tag = inWriteRenameNumber[i];
			
#ifdef DEBUG
			if(Debug(DebugRegisterFile))
			{
				cout << name() << ": Writing ";
				WriteHex(cout, inWriteRenameData[i]);
				cout << " to rr" << (int) tag << endl;
			}
			if(tag < 0 || tag >= nRenameBuffers)
			{
				cout << name() << ": Error : rename buffer number out of range" << endl;
				ABORT();
			}
#endif
			renames[tag] = inWriteRenameData[i];
		}
	}
	for(i = 0; i < nReadRenamePorts; i++)
	{
#ifdef DEBUG
		if(inReadRenameNumber[i] < 0 || inReadRenameNumber[i] >= nRenameBuffers)
		{
			cout << name() << ": Error : rename buffer number out of range" << endl;
			ABORT();
		}
#endif
		outReadRenameData[i] = renames[inReadRenameNumber[i]];
	}
}

void RegisterFile::WriteFloatingPointRename()
{
	int i;
	
	for(i = 0; i < nWriteFloatingPointRenamePorts; i++)
	{
		if(inWriteFloatingPointRename[i])
		{
			tag_t tag = inWriteFloatingPointRenameNumber[i];
			
#ifdef DEBUG
			if(Debug(DebugRegisterFile))
			{
				cout << name() << ": Writing ";
				WriteHex(cout, inWriteFloatingPointRenameData[i]);
				cout << " to rfp" << (int) tag << endl;
			}
			if(tag < 0 || tag >= nFloatingPointRenameBuffers)
			{
				cout << name() << ": Error : floating point rename buffer number out of range" << endl;
				ABORT();
			}
#endif
			floatingPointRenames[tag] = inWriteFloatingPointRenameData[i];
		}
	}
	for(i = 0; i < nReadFloatingPointRenamePorts; i++)
	{
#ifdef DEBUG
		if(inReadFloatingPointRenameNumber[i] < 0 || inReadFloatingPointRenameNumber[i] >= nFloatingPointRenameBuffers)
		{
			cout << name() << ": Error : floating point rename buffer number out of range" << endl;
			ABORT();
		}
#endif
		outReadFloatingPointRenameData[i] = floatingPointRenames[inReadFloatingPointRenameNumber[i]];
	}
}

void RegisterFile::WriteBack()
{
	int i;
	
	for(i = 0; i < nWriteBackPorts; i++)
	{
		if((Dummy<bool>) inWriteBack[i])
		{
			regnum_t num = inWriteBackRegisterNumber[i];
			tag_t tag = inWriteBackRenameNumber[i];
#ifdef DEBUG
			if(tag < 0 || tag >= nRenameBuffers)
			{
				cout << name() << ": Error : rename buffer number out of range" << endl;
				ABORT();
			}			
			if(num < 0 || num >= nRegisters)
			{
				cout << name() << ": Error : register number out of range" << endl;
				ABORT();
			}
			if(Debug(DebugRegisterFile))
			{
				cout << name() << ": moving rr" << (int) tag << " (";
				WriteHex(cout, renames[tag]);
				cout << ") to r" << (int) num << " (port " << i << ")" << endl;
			}
#endif
			registers[num] = renames[tag];
		}
	}
	
	for(i = 0; i < nReadRegisterPorts; i++)
	{
#ifdef DEBUG
		if(inReadRegisterNumber[i] < 0 || inReadRegisterNumber[i] >= nRegisters)
		{
			cout << name() << ": Error : register number out of range" << endl;
			ABORT();
		}
#endif
		outReadRegisterData[i] = registers[inReadRegisterNumber[i]];
	}
}

void RegisterFile::WriteBackFloatingPoint()
{
	int i;
	
	for(i = 0; i < nWriteBackFloatingPointPorts; i++)
	{
		if((Dummy<bool>) inWriteBackFloatingPoint[i])
		{
			regnum_t num = inWriteBackFloatingPointRegisterNumber[i];
			tag_t tag = inWriteBackFloatingPointRenameNumber[i];
#ifdef DEBUG
			if(Debug(DebugRegisterFile))
			{
				cout << "fp" << (int) num << "<- rfp" << (int) tag << endl;
			}
			if(tag < 0 || tag >= nFloatingPointRenameBuffers)
			{
				cout << name() << ": Error : floating point rename buffer number out of range (write back)" << endl;
				ABORT();
			}			
			if(num < 0 || num >= nFloatingPointRegisters)
			{
				cout << name() << ": Error : floating point register number out of range (write back)" << endl;
				ABORT();
			}
			if(Debug(DebugRegisterFile))
			{
				cout << name() << ": moving rfp" << (int) tag << " (";
				WriteHex(cout, floatingPointRenames[tag]);
				cout << ") to fp" << (int) num << " (port " << i << ")" << endl;
			}
#endif
			floatingPointRegisters[num] = floatingPointRenames[tag];
		}
	}
	
	for(i = 0; i < nReadFloatingPointRegisterPorts; i++)
	{
#ifdef DEBUG
		if(inReadFloatingPointRegisterNumber[i] < 0 || inReadFloatingPointRegisterNumber[i] >= nFloatingPointRegisters)
		{
			cout << name() << ": Error : floating point register number out of range (write back)" << endl;
			ABORT();
		}
#endif
		outReadFloatingPointRegisterData[i] = floatingPointRegisters[inReadFloatingPointRegisterNumber[i]];
	}
}

void RegisterFile::OnFallingEdge()
{
	WriteRename();
	WriteFloatingPointRename();
}

ostream& operator << (ostream& os, const RegisterFile& rf)
{
	int i;
	cout << "--- Registers --------" << endl;
	for(i = 0; i < nRegisters; i++)
	{
		cout << "r" << i << " = ";
		WriteHex(cout, rf.registers[i]);
		cout  << endl;
	}
	for(i = 0; i < nFloatingPointRegisters; i++)
	{
		cout << "f" << i << " = ";
		WriteHex(cout, rf.floatingPointRegisters[i]);
		cout  << endl;
	}
	cout << "--- Rename buffers -------" << endl;
	for(i = 0; i < nRenameBuffers; i++)
	{
		cout << "rr" << i << " = ";
		WriteHex(cout, rf.renames[i]);
		cout  << endl;
	}
	for(i = 0; i < nFloatingPointRenameBuffers; i++)
	{
		cout << "rf" << i << " = ";
		WriteHex(cout, rf.floatingPointRenames[i]);
		cout  << endl;
	}
	return os;
}
