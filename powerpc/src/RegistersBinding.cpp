/***************************************************************************
                          RegistersBinding.cpp  -  Register renaming
                             -------------------
    begin                : Fri Apr 6 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <systemc.h>
#include <RegistersBinding.h>
#include <stdlib.h>

#ifdef DEBUG
#define CONDITION
#endif

RegistersBinding::RegistersBinding()
{
	Reset();	
}

void RegistersBinding::Reset()
{
	regnum_t num;
	tag_t tag;
	fieldnum_t crf;

	nfree = nRenameBuffers;
	
	for(num = 0; num < nRegisters; num++)
	{
		renames[num] = -1;
	}
	
	for(tag = 0; tag < nRenameBuffers; tag++)
	{
		renameBuffers[tag].renamedRegister = -1;
		renameBuffers[tag].valid = false;
		renameBuffers[tag].prev = -1;
		renameBuffers[tag].next = -1;
	}
	
	nfpfree = nFloatingPointRenameBuffers;
	
	for(num = 0; num < nFloatingPointRegisters; num++)
	{
		fpRenames[num] = -1;
	}
	
	for(tag = 0; tag < nFloatingPointRenameBuffers; tag++)
	{
		fpRenameBuffers[tag].renamedRegister = -1;
		fpRenameBuffers[tag].valid = false;
		fpRenameBuffers[tag].prev = -1;
		fpRenameBuffers[tag].next = -1;
	}
	
	crFreeRenames = nCRRenameBuffers;
	for(crf = 0; crf < 8; crf++)
	{
		crRenames[crf] = -1;
	}
	
	for(tag = 0; tag < nCRRenameBuffers; tag++)
	{
		crRenameBuffers[tag].crfield = -1;
		crRenameBuffers[tag].valid = false;
		crRenameBuffers[tag].prev = -1;
		crRenameBuffers[tag].next = -1;
	}

	lrFreeRenames = nLRRenameBuffers;
	lrRename = -1;
	
	for(tag = 0; tag < nLRRenameBuffers; tag++)
	{
		lrRenameBuffers[tag].renameLR = false;
		lrRenameBuffers[tag].valid = false;
		lrRenameBuffers[tag].prev = -1;
		lrRenameBuffers[tag].next = -1;
	}

	ctrFreeRenames = nCTRRenameBuffers;
	ctrRename = -1;
	
	for(tag = 0; tag < nCTRRenameBuffers; tag++)
	{
		ctrRenameBuffers[tag].renameCTR = false;
		ctrRenameBuffers[tag].valid = false;
		ctrRenameBuffers[tag].prev = -1;
		ctrRenameBuffers[tag].next = -1;
	}
}

tag_t RegistersBinding::AllocateRenameBuffer(regnum_t regNum)
{
	tag_t tag;
#ifdef CONDITION
	if(!Precondition())
	{
		cout << "Precondition not valid before AllocateRenameBuffer" << endl;
		cout << *this;
		StopSimulation();
	}
#endif
	
	for(tag = 0; tag < nRenameBuffers; tag++)
	{
		if(renameBuffers[tag].renamedRegister < 0)
		{
			renameBuffers[tag].renamedRegister = regNum;
			renameBuffers[tag].prev = renames[regNum];
			if(renames[regNum] >= 0)
				renameBuffers[renames[regNum]].next = tag;
			renameBuffers[tag].next = -1;
			renameBuffers[tag].valid = false;
			renames[regNum] = tag;
			nfree--;
#ifdef DEBUG
			if(Debug(DebugRegistersBinding))
				cout << "Renaming register #" << (int) regNum << " by rename buffer #" << (int) tag << endl;
#endif
#ifdef CONDITION
			if(!Precondition())
			{
				cout << "Postcondition not valid after AllocateRenameBuffer" << endl;
				cout << *this;
				StopSimulation();
			}
#endif
			return tag;
		}
	}
	return -1;
}

tag_t RegistersBinding::AllocateFloatingPointRenameBuffer(regnum_t regNum)
{
	tag_t tag;
	
	for(tag = 0; tag < nFloatingPointRenameBuffers; tag++)
	{
		if(fpRenameBuffers[tag].renamedRegister < 0)
		{
			fpRenameBuffers[tag].renamedRegister = regNum;
			fpRenameBuffers[tag].prev = fpRenames[regNum];
			if(fpRenames[regNum] >= 0)
				fpRenameBuffers[fpRenames[regNum]].next = tag;
			fpRenameBuffers[tag].next = -1;
			fpRenameBuffers[tag].valid = false;
			fpRenames[regNum] = tag;
			nfpfree--;
#ifdef DEBUG
			if(Debug(DebugRegistersBinding))
				cout << "Renaming floating point register #" << (int) regNum << " by floating point rename buffer #" << (int) tag << endl;
#endif
			return tag;
		}
	}
	return -1;
}

regnum_t RegistersBinding::GetRegister(tag_t tag) const
{
	return renameBuffers[tag].renamedRegister;
}

regnum_t RegistersBinding::GetFloatingPointRegister(tag_t tag) const
{
	return fpRenameBuffers[tag].renamedRegister;
}

tag_t RegistersBinding::GetRenameBuffer(regnum_t regNum) const
{
	return renames[regNum];
}

tag_t RegistersBinding::GetFloatingPointRenameBuffer(regnum_t regNum) const
{
	return fpRenames[regNum];
}

void RegistersBinding::FreeRenameBuffer(tag_t tag)
{
#ifdef CONDITION
	if(!Precondition())
	{
		cout << "Precondition not valid before FreeRenameBuffer" << endl;
		cout << *this;
		ABORT();
	}
	if(renameBuffers[tag].prev >= 0)
	{
		cout << "FreeRenameBuffer (tag = " << (int) tag << ") is not called in order" << endl;
		cout << *this;
		ABORT();
	}
#endif
	if(renameBuffers[tag].renamedRegister >= 0)
	{
#ifdef DEBUG
		if(Debug(DebugRegistersBinding))
			cout << "Freeing rename buffer #" << (int) tag << endl;
#endif
		if(renameBuffers[tag].next >= 0)
		{
			renameBuffers[renameBuffers[tag].next].prev = -1;
		}
		else
		{
			renames[renameBuffers[tag].renamedRegister] = -1;
		}
		renameBuffers[tag].renamedRegister = -1;
		renameBuffers[tag].valid = false;
		nfree++;
	}

#ifdef CONDITION
	if(!Precondition())
	{
		cout << "Postcondition not valid after FreeRenameBuffer" << endl;
		cout << *this;
		ABORT();
	}
#endif
}

void RegistersBinding::FreeFloatingPointRenameBuffer(tag_t tag)
{
	if(fpRenameBuffers[tag].renamedRegister >= 0)
	{
#ifdef DEBUG
		if(Debug(DebugRegistersBinding))
			cout << "Freeing floating point rename buffer #" << (int) tag << endl;
#endif
		if(fpRenameBuffers[tag].next >= 0)
		{
			fpRenameBuffers[fpRenameBuffers[tag].next].prev = -1;
		}
		else
		{
			fpRenames[fpRenameBuffers[tag].renamedRegister] = -1;
		}
		fpRenameBuffers[tag].renamedRegister = -1;
		fpRenameBuffers[tag].valid = false;
		nfpfree++;
	}
}

void RegistersBinding::FreeRenameBufferReverseOrder(tag_t tag)
{
#ifdef CONDITION
	if(!Precondition())
	{
		cout << "Precondition not valid before FreeRenameBufferReverseOrder" << endl;
		cout << *this;
		ABORT();
	}
	if(renameBuffers[tag].next >= 0)
	{
		cout << "FreeRenameBufferReverseOrder (tag = " << (int) tag << ") is not called in reverse order" << endl;
		cout << *this;
		ABORT();
	}
#endif
	if(renameBuffers[tag].renamedRegister >= 0)
	{
#ifdef DEBUG
		if(Debug(DebugRegistersBinding))
			cout << "Freeing rename buffer #" << (int) tag << endl;
#endif
		renames[renameBuffers[tag].renamedRegister] = renameBuffers[tag].prev;
		if(renameBuffers[tag].prev >= 0)
			renameBuffers[renameBuffers[tag].prev].next = -1;
		renameBuffers[tag].renamedRegister = -1;
		renameBuffers[tag].valid = false;
		nfree++;
	}
#ifdef CONDITION
	if(!Precondition())
	{
		cout << "Postcondition not valid after FreeRenameBufferReverseOrder" << endl;
		cout << *this;
		ABORT();
	}
#endif
}

void RegistersBinding::FreeFloatingPointRenameBufferReverseOrder(tag_t tag)
{
	if(fpRenameBuffers[tag].renamedRegister >= 0)
	{
#ifdef DEBUG
		if(Debug(DebugRegistersBinding))
			cout << "Freeing floating point rename buffer #" << (int) tag << endl;
#endif
		fpRenames[fpRenameBuffers[tag].renamedRegister] = fpRenameBuffers[tag].prev;
		if(fpRenameBuffers[tag].prev >= 0)
			fpRenameBuffers[fpRenameBuffers[tag].prev].next = -1;
		fpRenameBuffers[tag].renamedRegister = -1;
		fpRenameBuffers[tag].valid = false;
		nfpfree++;
	}
}

tag_t RegistersBinding::AllocateCRRenameBuffer(fieldnum_t crf)
{
	tag_t tag;
	
	if(crf < 0) return -1;
	
	for(tag = 0; tag < nCRRenameBuffers; tag++)
	{
		if(crRenameBuffers[tag].crfield < 0)
		{
			crRenameBuffers[tag].crfield = crf;
			crRenameBuffers[tag].prev = crRenames[crf];
			if(crRenames[crf] >= 0)
				crRenameBuffers[crRenames[crf]].next = tag;
			crRenameBuffers[tag].next = -1;
			crRenameBuffers[tag].valid = false;
			crRenames[crf] = tag;
			crFreeRenames--;

#ifdef DEBUG
			if(Debug(DebugRegistersBinding))
				cout << "Renaming CR by rename buffer #" << (int) tag << endl;
#endif
			return tag;
		}
	}
	return -1;
}

fieldnum_t RegistersBinding::GetField(tag_t tag) const
{
	return crRenameBuffers[tag].crfield;
}

tag_t RegistersBinding::AllocateLRRenameBuffer()
{
	tag_t tag;
	
	for(tag = 0; tag < nLRRenameBuffers; tag++)
	{
		if(!lrRenameBuffers[tag].renameLR)
		{
			lrRenameBuffers[tag].renameLR = true;
			lrRenameBuffers[tag].prev = lrRename;
			if(lrRename >= 0)
				lrRenameBuffers[lrRename].next = tag;
			lrRenameBuffers[tag].next = -1;
			lrRenameBuffers[tag].valid = false;
			lrRename = tag;
			lrFreeRenames--;

#ifdef DEBUG
			if(Debug(DebugRegistersBinding))
				cout << "Renaming LR by rename buffer #" << (int) tag << endl;
#endif
			return tag;
		}
	}
	return -1;
}

tag_t RegistersBinding::AllocateCTRRenameBuffer()
{
	int tag;
	
	for(tag = 0; tag < nCTRRenameBuffers; tag++)
	{
		if(!ctrRenameBuffers[tag].renameCTR)
		{
			ctrRenameBuffers[tag].renameCTR = true;
			ctrRenameBuffers[tag].prev = ctrRename;
			if(ctrRename >= 0)
				ctrRenameBuffers[ctrRename].next = tag;
			ctrRenameBuffers[tag].next = -1;
			ctrRenameBuffers[tag].valid = false;
			ctrRename = tag;
			ctrFreeRenames--;

#ifdef DEBUG
			if(Debug(DebugRegistersBinding))
				cout << "Renaming CTR by rename buffer #" << (int) tag << endl;
#endif
			return tag;
		}
	}
	return -1;
}

tag_t RegistersBinding::GetCRRenameBuffer(fieldnum_t crf) const
{
	   return crRenames[crf];
}

tag_t RegistersBinding::GetLRRenameBuffer() const
{
	return lrRename;
}

tag_t RegistersBinding::GetCTRRenameBuffer() const
{
	return ctrRename;
}

void RegistersBinding::FreeCRRenameBuffer(tag_t tag)
{
	if(crRenameBuffers[tag].crfield >= 0)
	{
#ifdef DEBUG
		if(Debug(DebugRegistersBinding))
			cout << "Freeing CR rename buffer #" << (int) tag << endl;
#endif
		if(crRenameBuffers[tag].next >= 0)
		{
			crRenameBuffers[crRenameBuffers[tag].next].prev = -1;
		}
		else
		{
			crRenames[crRenameBuffers[tag].crfield] = -1;
		}
		crRenameBuffers[tag].crfield = -1;
		crRenameBuffers[tag].valid = false;
		crFreeRenames++;
	}
}

void RegistersBinding::FreeCRRenameBufferReverseOrder(tag_t tag)
{
	if(crRenameBuffers[tag].crfield >= 0)
	{
#ifdef DEBUG
		if(Debug(DebugRegistersBinding))
			cout << "Freeing CR rename buffer #" << (int) tag << endl;
#endif
		crRenames[crRenameBuffers[tag].crfield] = crRenameBuffers[tag].prev;
		if(crRenameBuffers[tag].prev >= 0)
			crRenameBuffers[crRenameBuffers[tag].prev].next = -1;
		crRenameBuffers[tag].crfield = -1;
		crRenameBuffers[tag].valid = false;
		crFreeRenames++;
	}
}

void RegistersBinding::FreeLRRenameBuffer(tag_t tag)
{
	if(lrRenameBuffers[tag].renameLR)
	{
#ifdef DEBUG
		if(Debug(DebugRegistersBinding))
			cout << "Freeing LR rename buffer #" << (int) tag << endl;
#endif
		if(lrRenameBuffers[tag].next >= 0)
		{
			lrRenameBuffers[lrRenameBuffers[tag].next].prev = -1;
		}
		else
		{
			lrRename = -1;
		}
		lrRenameBuffers[tag].renameLR = false;
		lrRenameBuffers[tag].valid = false;
		lrFreeRenames++;
	}
}

void RegistersBinding::FreeLRRenameBufferReverseOrder(tag_t tag)
{
	if(lrRenameBuffers[tag].renameLR)
	{
#ifdef DEBUG
		if(Debug(DebugRegistersBinding))
			cout << "Freeing LR rename buffer #" << (int) tag << endl;
#endif
		lrRename = lrRenameBuffers[tag].prev;
		if(lrRenameBuffers[tag].prev >= 0)
			lrRenameBuffers[lrRenameBuffers[tag].prev].next = -1;
		lrRenameBuffers[tag].renameLR = false;
		lrRenameBuffers[tag].valid = false;
		lrFreeRenames++;
	}
}

void RegistersBinding::FreeCTRRenameBuffer(tag_t tag)
{
	if(ctrRenameBuffers[tag].renameCTR)
	{
#ifdef DEBUG
		if(Debug(DebugRegistersBinding))
			cout << "Freeing CTR rename buffer #" << (int) tag << endl;
#endif
		if(ctrRenameBuffers[tag].next >= 0)
		{
			ctrRenameBuffers[ctrRenameBuffers[tag].next].prev = -1;
		}
		else
		{
			ctrRename = -1;
		}
		ctrRenameBuffers[tag].renameCTR = false;
		ctrRenameBuffers[tag].valid = false;
		ctrFreeRenames++;
	}
}

void RegistersBinding::FreeCTRRenameBufferReverseOrder(tag_t tag)
{
	if(ctrRenameBuffers[tag].renameCTR)
	{
#ifdef DEBUG
		if(Debug(DebugRegistersBinding))
			cout << "Freeing CTR rename buffer #" << (int) tag << endl;
#endif
		ctrRename = ctrRenameBuffers[tag].prev;
		if(ctrRenameBuffers[tag].prev >= 0)
			ctrRenameBuffers[ctrRenameBuffers[tag].prev].next = -1;
		ctrRenameBuffers[tag].renameCTR = false;
		ctrRenameBuffers[tag].valid = false;
		ctrFreeRenames++;
	}
}

void RegistersBinding::ValidateRenameBuffer(tag_t tag)
{
	renameBuffers[tag].valid = true;
}

void RegistersBinding::ValidateFloatingPointRenameBuffer(tag_t tag)
{
	fpRenameBuffers[tag].valid = true;
}

void RegistersBinding::ValidateCRRenameBuffer(tag_t tag)
{
	crRenameBuffers[tag].valid = true;
}

void RegistersBinding::ValidateLRRenameBuffer(tag_t tag)
{
	lrRenameBuffers[tag].valid = true;
}

void RegistersBinding::ValidateCTRRenameBuffer(tag_t tag)
{
	ctrRenameBuffers[tag].valid = true;
}

bool RegistersBinding::RenameBufferValid(tag_t tag) const
{
	return renameBuffers[tag].valid;
}

bool RegistersBinding::FloatingPointRenameBufferValid(tag_t tag) const
{
	return fpRenameBuffers[tag].valid;
}

bool RegistersBinding::CRRenameBufferValid(tag_t tag) const
{
	return crRenameBuffers[tag].valid;
}

bool RegistersBinding::LRRenameBufferValid(tag_t tag) const
{
	return lrRenameBuffers[tag].valid;
}

bool RegistersBinding::CTRRenameBufferValid(tag_t tag) const
{
	return ctrRenameBuffers[tag].valid;
}

ostream& operator << (ostream& os, const RegistersBinding& registersBinding)
{
	int i;
	
	for(i = 0; i < nRegisters; i++)
	{
		if(registersBinding.renames[i] >= 0)
		{
			os << "r" << i << " renamed by rr" << (int) registersBinding.renames[i] << endl;
		}
	}
	for(i = 0; i < nFloatingPointRegisters; i++)
	{
		if(registersBinding.fpRenames[i] >= 0)
		{
			os << "r" << i << " renamed by rfp" << (int) registersBinding.fpRenames[i] << endl;
		}
	}
	for(i = 0; i < 8; i++)
	{
		if(registersBinding.crRenames[i] >= 0)
		{
			os << "CR" << i << " renamed by CR rename buffer #" << (int) registersBinding.crRenames[i] << endl;
		}
	}
	if(registersBinding.lrRename >= 0)
		os << "LR renamed by LR rename buffer #" << registersBinding.lrRename << endl;
	if(registersBinding.ctrRename >= 0)
		os << "CTR renamed by CTR rename buffer #" << registersBinding.ctrRename << endl;
	
	for(i = 0; i < nRenameBuffers; i++)
	{
		if(registersBinding.renameBuffers[i].renamedRegister >= 0)
		{
			os << "rr" << i << " renames r" << (int) registersBinding.renameBuffers[i].renamedRegister << "(";
			if(registersBinding.renameBuffers[i].valid)
				os << "valid";
			else
				os << "not valid";
			os << ")";
			if(registersBinding.renameBuffers[i].prev >= 0)
				os << " previously renamed by rr" << (int) registersBinding.renameBuffers[i].prev;
			os << endl;
		}
	}
	
	for(i = 0; i < nFloatingPointRenameBuffers; i++)
	{
		if(registersBinding.fpRenameBuffers[i].renamedRegister >= 0)
		{
			os << "rfp" << i << " renames fp" << (int) registersBinding.fpRenameBuffers[i].renamedRegister << "(";
			if(registersBinding.fpRenameBuffers[i].valid)
				os << "valid";
			else
				os << "not valid";
			os << ")";
			if(registersBinding.fpRenameBuffers[i].prev >= 0)
				os << " previously renamed by rfp" << (int) registersBinding.fpRenameBuffers[i].prev;
			os << endl;
		}
	}
	
	for(i = 0; i < nCRRenameBuffers; i++)
	{
		if(registersBinding.crRenameBuffers[i].crfield >= 0)
		{
			os << "CR rename buffer #" << i << " renames CR" << (int) registersBinding.crRenameBuffers[i].crfield << "(";
			
			if(registersBinding.crRenameBuffers[i].valid)
				os << "valid";
			else
				os << "not valid";
			os << ")";
			
			if(registersBinding.crRenameBuffers[i].prev >= 0)
				os << " previously renamed by CR rename buffer #" << (int) registersBinding.crRenameBuffers[i].prev;
			os << endl;
		}
	}
	
	for(i = 0; i < nLRRenameBuffers; i++)
	{
		if(registersBinding.lrRenameBuffers[i].renameLR)
		{
			os << "LR rename buffer #" << i << " renames LR (";
			
			if(registersBinding.lrRenameBuffers[i].valid)
				os << "valid";
			else
				os << "not valid";
			os << ")";
			
			if(registersBinding.lrRenameBuffers[i].prev >= 0)
				os << " previously renamed by LR rename buffer #" << (int) registersBinding.lrRenameBuffers[i].prev;
			os << endl;
		}
	}
	
	for(i = 0; i < nCTRRenameBuffers; i++)
	{
		if(registersBinding.ctrRenameBuffers[i].renameCTR)
		{
			os << "CTR rename buffer #" << i << " renames CTR (";
			
			if(registersBinding.ctrRenameBuffers[i].valid)
				os << "valid";
			else
				os << "not valid";
			os << ")";
			
			if(registersBinding.ctrRenameBuffers[i].prev >= 0)
				os << " previously renamed by CTR rename buffer #" << (int) registersBinding.ctrRenameBuffers[i].prev;
			os << endl;
		}
	}
	return os;
}

bool RegistersBinding::Precondition()
{
	bool visited[nRenameBuffers];
	
	tag_t tag;
	for(tag = 0; tag < nRenameBuffers; tag++)
	{
		visited[tag] = false;
	}
	
	regnum_t num;
	for(num = 0; num < nRegisters; num++)
	{
		tag_t tag = renames[num];
		
		while(tag >= 0)
		{
			if(visited[tag]) return false;
			if(renameBuffers[tag].renamedRegister  != num) return false;
			visited[tag] = true;
			tag = renameBuffers[tag].prev;
		}
	}
	
	for(tag = 0; tag < nRenameBuffers; tag++)
	{
		if(!visited[tag])
		{
			if(renameBuffers[tag].renamedRegister >= 0) return false;
		}
	}
	return true;
}
