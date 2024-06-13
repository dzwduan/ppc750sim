/***************************************************************************
                          DispatchUnit.cpp  -  Unite de dispatch
                             -------------------
    begin                : Fri Apr 6 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <DispatchUnit.h>
#include <FloatingPointUnit.h>

DispatchUnit::DispatchUnit(const sc_module_name& name, ReorderBuffer *rob, RegistersBinding *registersBinding)
{
	SC_HAS_PROCESS(DispatchUnit);
	
	SC_METHOD(Decode);
	sensitive << inDecode;
		
	SC_METHOD(Dispatch);
	sensitive_neg << inClock;
	
	this->rob = rob;
	this->registersBinding = registersBinding;
	
	Reset();		
}


/* This method returns the integer unit number which is able to execute the class of instruction and which is the less loaded unit */
int DispatchUnit::GetLessLoadedIntegerRSQueue(int ic /* instruction class */)
{
	int j;
	int i = -1;
	int n = 0;
	for(j = 0; j < nIntegerUnit; j++)
	{
		if(intflags[j] & ic)
		{
			int nfree = integerRSQueue[j].GetFreeSpace();
			
			if(i >= 0)
			{
				if(nfree > n)
				{
					i = j;
					n = nfree;
				}
			}
			else
			{
				i = j;
				n = nfree;
			}
		}
	}
	return i;
}

/* This method returns the floating point unit number which is able to execute the class of instruction and which is the less loaded unit */
int DispatchUnit::GetLessLoadedFloatingPointRSQueue()
{
	int j;
	int i = -1;
	int n = 0;
	for(j = 0; j < nFloatingPointUnits; j++)
	{
		int nfree = floatingPointRSQueue[j].GetFreeSpace();
			
		if(i >= 0)
		{
			if(nfree > n)
			{
				i = j;
				n = nfree;
			}
		}
		else
		{
			i = j;
			n = nfree;
		}
	}
	return i;
}

/* This methods allocates an integer reservation station */
IntegerRS *DispatchUnit::AllocateIntegerRS(int ic /* instruction class */)
{
	int j = GetLessLoadedIntegerRSQueue(ic);
		
	if(j >= 0)
	{
		if(integerRSQueue[j].Full())
		{
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << name() << ": No remaining integer RS" << endl;
#endif
			return 0;
		}
		else
		{
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << name() << ": Allocating one integer RS (integer unit " << j << ")" << endl;
#endif
			IntegerRS& rs = integerRSQueue[j].Allocate();
			rs.unitNumber = j;
			return &rs;
		}
	}
		
	return 0;
}

/* This methods allocates a floating point reservation station */
FloatingPointRS *DispatchUnit::AllocateFloatingPointRS()
{
	int j = GetLessLoadedFloatingPointRSQueue();
		
	if(j >= 0)
	{
		if(floatingPointRSQueue[j].Full())
		{
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << name() << ": No remaining floating point RS" << endl;
#endif
			return 0;
		}
		else
		{
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << name() << ": Allocating one floating point RS (floating point unit " << j << ")" << endl;
#endif
			FloatingPointRS& rs = floatingPointRSQueue[j].Allocate();
			rs.unitNumber = j;
			return &rs;
		}
	}
		
	return 0;
}

/* this method allocates a load/store reservation station */
LoadStoreRS *DispatchUnit::AllocateLoadStoreRS()
{
	if(!loadStoreRSQueue.Full())
	{
#ifdef DEBUG
		if(Debug(DebugDispatchUnit))
			cout << name() << ": Allocating one load/store RS" << endl;
#endif
		return &loadStoreRSQueue.Allocate();
	}
	else
	{
#ifdef DEBUG
		if(Debug(DebugDispatchUnit))
			cout << name() << ": No remaining load/store RS" << endl;
#endif
		return 0;
	}
}

/* This method allocates a branch reservation station */
BranchRS *DispatchUnit::AllocateBranchRS()
{
	if(!branchRSQueue.Full())
	{
#ifdef DEBUG
		if(Debug(DebugDispatchUnit))
			cout << name() << ": Allocating one branch RS" << endl;
#endif
		return &branchRSQueue.Allocate();
	}
	else
	{
#ifdef DEBUG
		if(Debug(DebugDispatchUnit))
			cout << name() << ": No remaining branch RS" << endl;
#endif
		return 0;
	}
}

/* This methods allocates a system register unit reservation station */
SystemRegisterRS *DispatchUnit::AllocateSystemRegisterRS()
{
	if(!systemRegisterRSQueue.Full())
	{
#ifdef DEBUG
		if(Debug(DebugDispatchUnit))
			cout << name() << ": Allocating one system register RS" << endl;
#endif
		return &systemRegisterRSQueue.Allocate();
	}
	else
	{
#ifdef DEBUG
		if(Debug(DebugDispatchUnit))
			cout << name() << ": No remaining system register RS" << endl;
#endif
		return 0;
	}
}

/* This method tries to allocate n reservation stations
   (type of reservation station depends on the instruction which have been previously decoded).
   It returns the number of reservation stations that have been successfully allocated */
int DispatchUnit::AllocateRS(int n)
{
	int i;
	
	for(i = 0; i < n; i++)
	{	
		switch(units[i])
		{
			case IntegerUnitIdent:
				rs[i].integer = AllocateIntegerRS(decodedInstructions[i].iclass);
				if(!rs[i].integer) return i;
				break;
				
			case LoadStoreUnitIdent:
				rs[i].loadStore = AllocateLoadStoreRS();
				if(!rs[i].loadStore) return i;
				break;
				
			case BranchUnitIdent:
				rs[i].branch = AllocateBranchRS();
				if(!rs[i].branch) return i;
				break;
				
			case SystemRegisterUnitIdent:
				rs[i].systemRegister = AllocateSystemRegisterRS();
				if(!rs[i].systemRegister) return i;
				break;
				
			case FloatingPointUnitIdent:
				rs[i].floatingPoint = AllocateFloatingPointRS();
				if(!rs[i].floatingPoint) return i;
				break;
		}
	}
	return i;
}

/* This methods checks the ressources necessary to dispatch nInstructions instructions (those instructions that have been previously decoded).
   It returns the number of instructions that can be dispatched according to the available ressources (rename buffer, ROB, etc...) */
int DispatchUnit::CheckResources(int nInstructions)
{
	int j;
	int nInstructionsToDispatch;
	int nFreeROBEntries = rob->GetFreeSpace();
	int nFreeRenameBuffers = registersBinding->GetFreeRenameBuffers();
	int nFreeFloatingPointRenameBuffers = registersBinding->GetFreeFloatingPointRenameBuffers();
	int nFreeCRRenameBuffers = registersBinding->GetFreeCRRenameBuffers();
	int nFreeLRRenameBuffers = registersBinding->GetFreeLRRenameBuffers();
	int nFreeCTRRenameBuffers = registersBinding->GetFreeCTRRenameBuffers();
	
#ifdef DEBUG
	if(Debug(DebugDispatchUnit))
	{
		cout << name() << ": " << nFreeROBEntries << " Free ROB Entries" << endl;
		cout << name() << ": " << nFreeRenameBuffers << " Free Rename Buffers" << endl;
		cout << name() << ": " << nFreeFloatingPointRenameBuffers << " Free Floating Point Rename Buffers" << endl;
		cout << name() << ": " << nFreeCRRenameBuffers << " Free CR Rename Buffers" << endl;
		cout << name() << ": " << nFreeLRRenameBuffers << " Free LR Rename Buffers" << endl;
		cout << name() << ": " << nFreeCTRRenameBuffers << " Free CTR Rename Buffers" << endl;
	}
#endif
	
	for(nInstructionsToDispatch = 0; nInstructionsToDispatch < nInstructions && nFreeROBEntries > 0; nInstructionsToDispatch++)
	{
		switch(units[nInstructionsToDispatch])
		{
			case IntegerUnitIdent:
				nFreeROBEntries--;
				if(decodedInstructions[nInstructionsToDispatch].io.integer.dstReg >= 0)
					if(--nFreeRenameBuffers < 0)
					{
#ifdef DEBUG
						if(Debug(DebugDispatchUnit))
							cout << name() << ": Not enough free rename buffers to dispatch IQ[" << nInstructionsToDispatch << "]" << endl;
#endif
						return nInstructionsToDispatch;
					}
					
				if(decodedInstructions[nInstructionsToDispatch].io.integer.outCR)
					if(--nFreeCRRenameBuffers < 0)
					{
#ifdef DEBUG
						if(Debug(DebugDispatchUnit))
							cout << name() << ": Not enough free cr rename buffers to dispatch instruction IQ[" << nInstructionsToDispatch << "]" << endl;
#endif
						return nInstructionsToDispatch;
					}
				break;
				
			case FloatingPointUnitIdent:
				nFreeROBEntries--;
				if(decodedInstructions[nInstructionsToDispatch].io.floatingPoint.dstReg >= 0)
					if(--nFreeFloatingPointRenameBuffers < 0)
					{
#ifdef DEBUG
						if(Debug(DebugDispatchUnit))	
							cout << name() << ": Not enough free floating point rename buffers to dispatch IQ[" << nInstructionsToDispatch << "]" << endl;
#endif
						return nInstructionsToDispatch;
					}
					
				if(decodedInstructions[nInstructionsToDispatch].io.floatingPoint.outCR)
					if(--nFreeCRRenameBuffers < 0)
					{
#ifdef DEBUG	
						if(Debug(DebugDispatchUnit))
							cout << name() << ": Not enough free cr rename buffers to dispatch instruction IQ[" << nInstructionsToDispatch << "]" << endl;
#endif
						return nInstructionsToDispatch;
					}
				break;
			
			case LoadStoreUnitIdent:
				nFreeROBEntries--;
				
				if(decodedInstructions[nInstructionsToDispatch].iclass & ICIntLoadStore)
				{
					for(j = 0; j < 2; j++)
					{
						if(decodedInstructions[nInstructionsToDispatch].io.loadStore.dstReg[j] >= 0)
							if(--nFreeRenameBuffers < 0)
							{
#ifdef DEBUG	
								if(Debug(DebugDispatchUnit))
									cout << name() << ": Not enough free rename buffers to dispatch instruction IQ[" << nInstructionsToDispatch << "]" << endl;
#endif
								return nInstructionsToDispatch;
							}
					}
				}
				else if(decodedInstructions[nInstructionsToDispatch].iclass & ICFloatLoadStore)
				{
					if(decodedInstructions[nInstructionsToDispatch].io.loadStore.dstReg[0] >= 0)
						if(--nFreeFloatingPointRenameBuffers < 0)
						{
#ifdef DEBUG	
							if(Debug(DebugDispatchUnit))
								cout << name() << ": Not enough free floating point rename buffers to dispatch instruction IQ[" << nInstructionsToDispatch << "]" << endl;
#endif
							return nInstructionsToDispatch;
						}
					
					for(j = 1; j < 2; j++)
					{
						if(decodedInstructions[nInstructionsToDispatch].io.loadStore.dstReg[j] >= 0)
							if(--nFreeRenameBuffers < 0)
							{
#ifdef DEBUG	
								if(Debug(DebugDispatchUnit))
									cout << name() << ": Not enough free rename buffers to dispatch instruction IQ[" << nInstructionsToDispatch << "]" << endl;
#endif
								return nInstructionsToDispatch;
							}
					}
				}
				break;
				
			case BranchUnitIdent:
				nFreeROBEntries--;
				if(decodedInstructions[nInstructionsToDispatch].io.branch.outLR)
					if(--nFreeLRRenameBuffers < 0)
					{
#ifdef DEBUG	
						if(Debug(DebugDispatchUnit))
							cout << name() << ": Not enough free lr rename buffers to dispatch instruction IQ[" << nInstructionsToDispatch << "]" <<endl;
#endif
						return nInstructionsToDispatch;
					}
					
				if(decodedInstructions[nInstructionsToDispatch].io.branch.outCTR)
					if(--nFreeCTRRenameBuffers < 0)
					{
#ifdef DEBUG	
						if(Debug(DebugDispatchUnit))
							cout << name() << ": Not enough free ctr rename buffers to dispatch instruction IQ[" << nInstructionsToDispatch << "]" << endl;
#endif
						return nInstructionsToDispatch;
					}
				break;
				
			case SystemRegisterUnitIdent:
				nFreeROBEntries--;
				switch(decodedInstructions[nInstructionsToDispatch].iclass)
				{
					case ICMoveToSpr:
						if(decodedInstructions[nInstructionsToDispatch].io.mtspr.dstSpr == 8)
							if(--nFreeLRRenameBuffers < 0)
							{
#ifdef DEBUG	
								if(Debug(DebugDispatchUnit))
									cout << name() << ": Not enough free lr rename buffers to dispatch instruction IQ[" << nInstructionsToDispatch << "]" << endl;
#endif
								return nInstructionsToDispatch;
							}
				
						if(decodedInstructions[nInstructionsToDispatch].io.mtspr.dstSpr == 9)
							if(--nFreeCTRRenameBuffers < 0)
							{
#ifdef DEBUG	
								if(Debug(DebugDispatchUnit))
									cout << name() << ": Not enough free ctr rename buffers to dispatch instruction IQ[" << nInstructionsToDispatch << "]" << endl;
#endif
								return nInstructionsToDispatch;
							}
							
						break;
						
					case ICMoveFromSpr:
						if(decodedInstructions[nInstructionsToDispatch].io.mfspr.dstReg >= 0)
							if(--nFreeRenameBuffers < 0)
							{
#ifdef DEBUG	
								if(Debug(DebugDispatchUnit))
									cout << name() << ": Not enough free rename buffers to dispatch instruction IQ[" << nInstructionsToDispatch << "]" << endl;
#endif
								return nInstructionsToDispatch;
							}
						break;
						
					case ICMoveFromTbr:
						if(decodedInstructions[nInstructionsToDispatch].io.mftb.dstReg >= 0)
							if(--nFreeRenameBuffers < 0)
							{
#ifdef DEBUG	
								if(Debug(DebugDispatchUnit))
									cout << name() << ": Not enough free rename buffers to dispatch instruction IQ[" << nInstructionsToDispatch << "]" << endl;
#endif
								return nInstructionsToDispatch;
							}
						break;
					
					case ICCRLogical:
						if(--nFreeCRRenameBuffers < 0)
						{
#ifdef DEBUG
							if(Debug(DebugDispatchUnit))
								cout << name() << ": Not enough free CR rename buffers to dispatch instruction IQ[" << nInstructionsToDispatch << "]" << endl;
#endif
							return nInstructionsToDispatch;
						}
						break;
					
					case ICMoveToCrf:
						if(decodedInstructions[nInstructionsToDispatch].io.mtcrf.dstCrf < 0)
						{
							/* this instruction set the interlock bit, no further instruction will be dispatched */
							return nInstructionsToDispatch + 1;
						}
						else
						{
							if(--nFreeCRRenameBuffers < 0)
							{
#ifdef DEBUG
								if(Debug(DebugDispatchUnit))
									cout << name() << ": Not enough free CR rename buffers to dispatch instruction IQ[" << nInstructionsToDispatch << "]" << endl;
#endif
								return nInstructionsToDispatch;
							}
						}
						break;
						
					case ICMoveCrf:
						if(--nFreeCRRenameBuffers < 0)
						{
#ifdef DEBUG
							if(Debug(DebugDispatchUnit))
								cout << name() << ": Not enough free CR rename buffers to dispatch instruction IQ[" << nInstructionsToDispatch << "]" << endl;
#endif
							return nInstructionsToDispatch;
						}
						break;
					
					case ICSystemCall:
						/* this instruction set the interlock bit, no further instruction will be dispatched */
						return nInstructionsToDispatch + 1;
				}	
				break;
		}
	}
	return nInstructionsToDispatch;	
}

void DispatchUnit::Decode()
{
#ifdef DEBUG
	if(Debug(DebugDispatchUnit))
	{
		cout << name() << ": branch counter = " << branchCounter << endl;
	}
#endif
	
	/* If the interlock bit is set then do not try to decode instructions */
	/* Just return instead */
	if(interlock)
	{
#ifdef DEBUG
		if(Debug(DebugDispatchUnit))
			cout << name() << ": Interlock set" << endl;
#endif
		nDecodedInstructions = 0;
		outDispatchedInstructions = 0;
		outDispatchedBranchs = 0;
		return;
	}

	/* Decode instructions in the instruction queue as much as possible */
	/* As much as nIQ instructions can be decoded */
	int i, j, k;
	int nInstructions, nBranchs;
	int nInstructionsToDecode = inInstructionsInQueue;
	int nBranchsInQueue = inBranchsInQueue;
	int nBranchsToDecode = nBranchsInQueue;
	if(nBranchsToDecode > nIQBranchs) nBranchsToDecode = nIQBranchs;
	if(nInstructionsToDecode > nIQ) nInstructionsToDecode = nIQ;
	int nFreeROBEntries = rob->GetFreeSpace();
	if(nInstructionsToDecode > nFreeROBEntries) nInstructionsToDecode = nFreeROBEntries;
	bool branchDetected = false;
	int nNonBranchInstructionsToDispatch = 0;
	int nBranchInstructionsToDispatch = 0;
	InstructionQueueEntry iq[nIQ];
	const InstructionQueueEntry *queue[nIQ + nIQBranchs];
	
#ifdef DEBUG
	if(Debug(DebugDispatchUnit))
		cout << name() << ": " << nInstructionsToDecode << " instructions to decode" << endl;
	if(Debug(DebugDispatchUnit))
		cout << name() << ": " << nBranchsToDecode << " branchs to decode" << endl;
#endif

	bool decodeBranchFollow = branchFollow;
	bool stopDecoding = false;
	
	for(i = 0, j = 0, k = 0; !stopDecoding && i < nIQ + nIQBranchs; i++)
	{
		bool newInstruction = false;
		
		if(k < nBranchsInQueue)
		{
			const InstructionQueueEntry& branchEntry = inBranchQueue[k];
			if((branchEntry.priority || decodeBranchFollow) && branchEntry.branchCounter <= nMaxBranchsBeforeDispatchStall)
			{
				if(k < nBranchsToDecode)
				{
					k++;
					queue[i] = &branchEntry;
					newInstruction = true;
					decodeBranchFollow = false;
				}
			}
		}
		
		if(!decodeBranchFollow && !newInstruction)
		{
			if(j < nInstructionsToDecode)
			{
				iq[j] = inIQ[j];
				InstructionQueueEntry& instructionEntry = iq[j];
				bool ok = false;

				if(!ppc_is_a_branch(instructionEntry.instruction))
				{
					ok = true;
				}
				else
				{
					if(instructionEntry.branchCounter <= nMaxBranchsBeforeDispatchStall)
					{
						if((!ppc_is_unconditional_branch(instructionEntry.instruction) && !ppc_is_conditional_branch(instructionEntry.instruction)) || instructionEntry.branchPredicted)
						{
							ok = true;
						}
					}
				}
				
				if(ok)
				{
					queue[i] = &instructionEntry;
					j++;
					newInstruction = true;
					decodeBranchFollow = instructionEntry.branchFollow;
				}
			}
		}
		
		if(!newInstruction) break;

		const InstructionQueueEntry& entry = *queue[i];
		
#ifdef DEBUG
		if(Debug(DebugDispatchUnit))
		{
			cout << name() << ": IQ[" << i << "] contains (" << entry.dbgInstructionCounter << ")  ";
			/* Emulator code begins */
			ppc_disassemble_to_stream(entry.instruction, entry.pc, stdout);
			/* Emulator code ends */
			cout << endl;
		}
#endif
		
		UInt32 instructionWord = entry.instruction;
		/* Emulator code begins */
		ppc_translate(entry.instruction, &decodedInstructions[i]);
	      /* Emulator code ends */
		
		if(decodedInstructions[i].iclass & (ICIntALU | ICIntMul | ICIntDiv))
		{
#ifdef DEBUG
		if(Debug(DebugDispatchUnit))
			cout << name() << ": Instruction will go to an integer unit" << endl;
#endif
			units[i] = IntegerUnitIdent;
		}
		else if(decodedInstructions[i].iclass & ICFloat)
		{
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << name() << ": Instruction will go to a floating point unit" << endl;
#endif
			units[i] = FloatingPointUnitIdent;
		}
		else if(decodedInstructions[i].iclass & ICLoadStore)
		{
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << name() << ": Instruction will go to the load/store unit" << endl;
#endif
			units[i] = LoadStoreUnitIdent;
		}
		else if(decodedInstructions[i].iclass & ICBranch)
		{
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << name() << ": Instruction will go to the branch unit" << endl;
#endif
	/*
			if(!queue[i]->branchPredicted)
			{
				i = i > 0 ? i - 1 : 0;
				break;
			}
	*/
			units[i] = BranchUnitIdent;
			branchDetected = true;
		} else if(decodedInstructions[i].iclass & (ICMoveToSpr | ICMoveFromSpr | ICCRLogical | ICMoveFromCR | ICMoveToCrf | ICSystemCall | ICMoveCrf | ICMoveFromTbr))
		{
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << name() << ": Instruction will go to the system register unit" << endl;
#endif
			units[i] = SystemRegisterUnitIdent;
		} else
		{
			/* Do not generate an error if the instruction is invalid and some branchs are not yet resolved */
			if(branchCounter > 0 || branchDetected)
			{
				break;
			}
			else
			{
				cout << name() << ": Error : not a valid instruction (time stamp = " << sim_time_stamp() << ")" << endl;
#ifdef EMULATE
				ppc_disassemble_to_stream(entry.instruction, entry.pc, stdout);
				cout << endl;
#endif				
				ABORT();
			}
		}
	}
	
	/* Get the number of instructions that has been decoded */
	nDecodedInstructions = i;
#ifdef DEBUG
	if(Debug(DebugDispatchUnit))
	{
		cout << name() << ": " << nDecodedInstructions << " decoded instructions" << endl;
		cout << name() << ": Registers Binding" << endl << *registersBinding;
	}
#endif	

	if(nDecodedInstructions > 0)
	{
		/* Check and Allocate ressources in order to dispatch the decoded instructions */
		nInstructionsToDispatch = AllocateRS(CheckResources(nDecodedInstructions));
		
#ifdef TRACE
		if(Trace(TraceDispatchUnit))
		{
			trace_file->Begin("dispatch");
		}
#endif	
		
		/* Now, we launch the readings on the register file, allocate some reorder buffer entries, and rename the registers */
		int readRegisterPort = 0;
		int readFloatingPointRegisterPort = 0;
		int readRenamePort = 0;
		int readFloatingPointRenamePort = 0;
		int readRenameCRPort = 0;
		int readRenameLRPort = 0;
		int readRenameCTRPort = 0;
	
		for(i = 0; i < nInstructionsToDispatch; i++)
		{
			branchFollow = queue[i]->branchFollow;
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
			{
				cout << name() << ": branch counter = " << branchCounter << endl;
			}
#endif
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
			{
				cout << name() << ": Instruction #" << i << " :" << endl;
				{
					const InstructionQueueEntry& instructionQueueEntry = *queue[i];
					cout << name() << ": (" << instructionQueueEntry.dbgInstructionCounter << ")  ";
					ppc_disassemble_to_stream(instructionQueueEntry.instruction, instructionQueueEntry.pc, stdout);
					cout << endl;
				}
			}
#endif
#ifdef TRACE
			if(Trace(TraceDispatchUnit))
			{
				trace_file->Begin("instruction");
			}
#endif
			/* Allocate a reorder buffer entry and fill in the entry */
			ReorderBufferEntry *robEntry;
#if defined(EMULATE)
			robEntry = &rob->Allocate();
#else
			robEntry = (units[i] == BranchUnitIdent && !decodedInstructions[i].io.branch.outLR && !decodedInstructions[i].io.branch.outCTR) ? 0 : &rob->Allocate();
//			robEntry = &rob->Allocate();
#endif
			
			if(robEntry)
			{
				robEntry->unitIdent = units[i];
				robEntry->finished = false;
			
				/* Emulator code begins */
				const InstructionQueueEntry& instructionQueueEntry = *queue[i];
				robEntry->inst = instructionQueueEntry.inst;
				robEntry->instPC = instructionQueueEntry.pc;
				/* Emulator code ends */
				robEntry->nia = instructionQueueEntry.nia;
			}
			
			switch(units[i])
			{
				case IntegerUnitIdent:
					{
						nNonBranchInstructionsToDispatch++;
#ifdef DEBUG
						if(Debug(DebugDispatchUnit))
							cout << "unit : integer" << endl;
#endif
#ifdef TRACE
						if(Trace(TraceDispatchUnit))
						{
							trace_file->Value("number", instructionQueueEntry.dbgInstructionCounter);
							trace_file->Value("pc", instructionQueueEntry.pc);
							trace_file->Value("unit", "integer");
							trace_file->Value("unit_number", rs[i].integer->unitNumber);
						}
#endif
						for(j = 0; j < 2; j++)
						{
							regnum_t srcReg = decodedInstructions[i].io.integer.srcReg[j]; /* Get the source register number */
					
							if(srcReg >= 0)
							{
								/* Instruction has a source register number */
#ifdef DEBUG
								if(Debug(DebugDispatchUnit))
									cout << "source register : r" << (int) srcReg << endl;
#endif
								tag_t tag = registersBinding->GetRenameBuffer(srcReg);	/* Get the tag of the source register */
								rs[i].integer->operands[j].tag = tag;		/* store the tag of the operand */
								rs[i].integer->operands[j].valid = false;	/* make the reservation station to wait for the operand */
								
								if(tag < 0)
								{
									 /* The source register is not renamed */
#ifdef DEBUG
									if(Debug(DebugDispatchUnit))
									{
										cout << name() << ": Reading r" << (int) srcReg << " value from register file" << endl;
										if(readRegisterPort >= nReadRegisterPorts)
										{
											cout << name() << ": not enough read register ports" << endl;
											ABORT();
										}
									}
#endif
									integerReadingRegisters[i][j] = srcReg;				/* To remember that we are reading the source register */
									integerReadingRenameBuffers[i][j] = -1;				/* No rename buffer is being read */
									outReadRegisterNumber[readRegisterPort++] = srcReg;	/* Ask to the register file the value of the source register */
								}
								else
								{
									/* The source register is renamed */
									if(registersBinding->RenameBufferValid(tag))
									{
										/* The rename buffer yet contains the right value */									
#ifdef DEBUG
										if(Debug(DebugDispatchUnit))
										{
											cout << name() << ": Reading r" << (int) srcReg << " value from rr" << (int) tag << endl;
											if(readRenamePort >= nReadRenamePorts)
											{
												cout << name() << ": not enough read rename buffer ports" << endl;
												ABORT();
											}
										}
#endif
										integerReadingRegisters[i][j] = -1;				/* No register is being read */
										integerReadingRenameBuffers[i][j] = tag;		/* To remember that we are reading the source register value from the rename buffers */
										outReadRenameNumber[readRenamePort++] = tag;	/* Ask to the register file the value of the rename buffer containing the value of the source register */
									}
									else
									{
#ifdef DEBUG
										if(Debug(DebugDispatchUnit))
											cout << name() << ": Waiting for r" << (int) srcReg << " value hold by rr" << (int) tag << endl;
#endif
										integerReadingRegisters[i][j] = -1;		/* No register is being read */
										integerReadingRenameBuffers[i][j] = -1;	/* No rename buffer is being read */
										/* Just wait for the value to be produce by functional units */
									}
								}
							}
							else
							{
								/* No source register */
								integerReadingRegisters[i][j] = -1;
								integerReadingRenameBuffers[i][j] = -1;
								rs[i].integer->operands[j].tag = -1;
							}
						}
					
						/* Store the integer operation into the reservation station */
						rs[i].integer->operation = decodedInstructions[i].operation.integer;
#if defined(DEBUG) || defined(TRACE)
						{
							const InstructionQueueEntry& instructionQueueEntry = *queue[i];
							rs[i].integer->dbgInstructionCounter = instructionQueueEntry.dbgInstructionCounter;
							rs[i].integer->dbgProgramCounter = instructionQueueEntry.pc;
							rs[i].integer->dbgInstruction = instructionQueueEntry.instruction;
						}
#endif	
						if(decodedInstructions[i].hasImmed)
							rs[i].integer->operands[1].data = decodedInstructions[i].immed;
						
						/* Fill the reservation station with tag of the destination registers */
						regnum_t dstReg = decodedInstructions[i].io.integer.dstReg;
						
						if(dstReg >= 0)
						{
							/* Instruction has a destination register number */
#ifdef DEBUG
							if(Debug(DebugDispatchUnit))
								cout << "destination register : r" << (int) dstReg << endl;
#endif
							/* Allocate a rename buffer and store its tag into the reservation station */
							rs[i].integer->tags.resultTag = registersBinding->AllocateRenameBuffer(dstReg);
						}
						else
						{
							/* Instruction has no destination register */
							rs[i].integer->tags.resultTag = -1;
						}
						
						if(decodedInstructions[i].io.integer.outCR)
						{
							/* Instruction has a destination CR field */
							fieldnum_t crf = decodedInstructions[i].io.integer.crf;
#ifdef DEBUG
							if(Debug(DebugDispatchUnit))
								cout << "destination register : CR" << (int) crf << endl;
#endif
							/* Allocate a rename buffer and store its tag into the reservation station */
							rs[i].integer->tags.CRTag = registersBinding->AllocateCRRenameBuffer(crf);
						}
						else
						{
							/* Instruction has no destination CR field */
							rs[i].integer->tags.CRTag = -1;
						}
						
						/* Store the instruction tag into the reservation station */
						rs[i].integer->tags.tag = rob->GetIndex(*robEntry);
						
						rs[i].integer->branchCounter = branchCounter;
					
						robEntry->extra.integer.tags = rs[i].integer->tags;
					
						if(decodedInstructions[i].io.integer.inCarry)
						{
							/* Instruction reads the carry bit XER[CA], so it is execution serialized */
							robEntry->executionSerialized = true;
							rs[i].integer->executionSerialized = true;
						}
						else
						{
							/* Instruction does not read the carry bit XER[CA], so it is not execution serialized */
							robEntry->executionSerialized = false;
							rs[i].integer->executionSerialized = false;
						}
				
						/* To remember whether the instruction modifies the overflow bit XER[OV], or the carry bit XER[CA] */
						if(decodedInstructions[i].io.integer.outOverflow)
							robEntry->extra.integer.writeOverflow = true;
						else
							robEntry->extra.integer.writeOverflow = false;
				
						if(decodedInstructions[i].io.integer.outCarry)
							robEntry->extra.integer.writeCarry = true;
						else
							robEntry->extra.integer.writeCarry = false;
	
						/* Fill the reorder buffer entry with some other usefull informations */
						robEntry->branchCounter = rs[i].integer->branchCounter;
						robEntry->extra.integer.unitNumber = rs[i].integer->unitNumber;
#if defined(DEBUG) || defined(TRACE)
						robEntry->dbgInstructionCounter = rs[i].integer->dbgInstructionCounter;
						robEntry->dbgProgramCounter = rs[i].integer->dbgProgramCounter;
						robEntry->dbgInstruction = rs[i].integer->dbgInstruction;
#endif
					}
					break;
					
				case FloatingPointUnitIdent:
					{
						nNonBranchInstructionsToDispatch++;
#ifdef DEBUG
						if(Debug(DebugDispatchUnit))
							cout << "unit : floating point" << endl;
#endif
#ifdef TRACE
						if(Trace(TraceDispatchUnit))
						{
							trace_file->Value("number", instructionQueueEntry.dbgInstructionCounter);
							trace_file->Value("pc", instructionQueueEntry.pc);
							trace_file->Value("unit", "floating point");
							trace_file->Value("unit_number", rs[i].floatingPoint->unitNumber);
						}
#endif
						for(j = 0; j < 3; j++)
						{
							regnum_t srcReg = decodedInstructions[i].io.floatingPoint.srcReg[j]; /* Get the source register number */
					
							if(srcReg >= 0)
							{
								/* Instruction a source register */
#ifdef DEBUG
								if(Debug(DebugDispatchUnit))
									cout << "source register : fp" << (int) srcReg << endl;
#endif
								tag_t tag = registersBinding->GetFloatingPointRenameBuffer(srcReg);	/* Get the tag of the source register */
								rs[i].floatingPoint->operands[j].tag = tag;					/* store the tag of the operand */
								rs[i].floatingPoint->operands[j].valid = false;				/* make the reservation station to wait for the operand */
								
								if(tag < 0)
								{
									 /* The source register is not renamed */
#ifdef DEBUG
									if(Debug(DebugDispatchUnit))
									{
										cout << name() << ": Reading rfp" << (int) srcReg << " value from register file" << endl;
										if(readFloatingPointRegisterPort >= nReadFloatingPointRegisterPorts)
										{
											cout << name() << ": not enough read floating point register ports" << endl;
											ABORT();
										}
									}
#endif
									floatingPointReadingFloatingPointRegisters[i][j] = srcReg;				/* To remember that we are reading the source register */
									floatingPointReadingFloatingPointRenameBuffers[i][j] = -1;				/* No rename buffer is being read */
									outReadFloatingPointRegisterNumber[readFloatingPointRegisterPort++] = srcReg;	/* Ask to the register file the value of the source register */
								}
								else
								{
									/* The source register is renamed */
									if(registersBinding->FloatingPointRenameBufferValid(tag))
									{
										/* The rename buffer yet contains the right value */
#ifdef DEBUG
										if(Debug(DebugDispatchUnit))
										{
											cout << name() << ": Reading fp" << (int) srcReg << " value from rfp" << (int) tag << endl;
											if(readFloatingPointRenamePort >= nReadFloatingPointRenamePorts)
											{
												cout << name() << ": not enough read floating point rename buffer ports" << endl;
												ABORT();
											}
										}
#endif
										floatingPointReadingFloatingPointRegisters[i][j] = -1;		/* No register is being read */
										floatingPointReadingFloatingPointRenameBuffers[i][j] = tag;	/* Remember that we are reading the source register value from the rename buffers */
										outReadFloatingPointRenameNumber[readFloatingPointRenamePort++] = tag;	/* Ask to register file the value of the rename buffer containing the value of the source register */
									}
									else
									{
#ifdef DEBUG
										if(Debug(DebugDispatchUnit))
											cout << name() << ": Waiting for fp" << (int) srcReg << " value hold by rfp" << (int) tag << endl;
#endif
										floatingPointReadingFloatingPointRegisters[i][j] = -1;		/* No register is being read */
										floatingPointReadingFloatingPointRenameBuffers[i][j] = -1;	/* No rename buffer is being read */
										/* Just wait for the value to be produce by functional units */
									}
								}
							}
							else
							{
								/* No source register */
								floatingPointReadingFloatingPointRegisters[i][j] = -1;
								floatingPointReadingFloatingPointRenameBuffers[i][j] = -1;
								rs[i].floatingPoint->operands[j].tag = -1;
							}
						}
					
						/* Store the floating point operation into the reservation station */
						rs[i].floatingPoint->operation = decodedInstructions[i].operation.floatingPoint;
#if defined(DEBUG) || defined(TRACE)
						{
							const InstructionQueueEntry& instructionQueueEntry = *queue[i];
							rs[i].floatingPoint->dbgInstructionCounter = instructionQueueEntry.dbgInstructionCounter;
							rs[i].floatingPoint->dbgProgramCounter = instructionQueueEntry.pc;
							rs[i].floatingPoint->dbgInstruction = instructionQueueEntry.instruction;
						}
#endif									
						/* Fill the reservation station with tag of the destination registers */
						regnum_t dstReg = decodedInstructions[i].io.floatingPoint.dstReg;
						
						if(dstReg >= 0)
						{
							/* Instruction has a destination register */
#ifdef DEBUG
							if(Debug(DebugDispatchUnit))
								cout << "destination register : fp" << (int) dstReg << endl;
#endif
							/* Allocate a floating point rename buffer and store its tag into the reservation station */
							rs[i].floatingPoint->tags.resultTag = registersBinding->AllocateFloatingPointRenameBuffer(dstReg);
						}
						else
						{
							/* Instruction has no destination register */
							rs[i].floatingPoint->tags.resultTag = -1;
						}
						
						if(decodedInstructions[i].io.floatingPoint.outCR)
						{
							/* Instruction has a destination CR field */
							fieldnum_t crf = decodedInstructions[i].io.floatingPoint.crf;
#ifdef DEBUG
							if(Debug(DebugDispatchUnit))
								cout << "destination register : CR" << (int) crf << endl;
#endif
							/* Allocate a rename buffer and store its tag into the reservation station */
							rs[i].floatingPoint->tags.CRTag = registersBinding->AllocateCRRenameBuffer(crf);
						}
						else
						{
							/* Instruction has no destination CR field */
							rs[i].floatingPoint->tags.CRTag = -1;
						}
						
						/* Store the instruction tag into the reservation station */
						rs[i].floatingPoint->tags.tag = rob->GetIndex(*robEntry);
						rs[i].floatingPoint->branchCounter = branchCounter;
										
						robEntry->extra.floatingPoint.tags = rs[i].floatingPoint->tags;
					
						if(decodedInstructions[i].io.floatingPoint.inFPSCR)
							robEntry->executionSerialized = true;
						else
							robEntry->executionSerialized = false;
						
						/* Fill the reorder buffer entry with some other usefull informations */
						robEntry->branchCounter = rs[i].floatingPoint->branchCounter;
						robEntry->extra.integer.unitNumber = rs[i].floatingPoint->unitNumber;
#if defined(DEBUG) || defined(TRACE)
						robEntry->dbgInstructionCounter = rs[i].floatingPoint->dbgInstructionCounter;
						robEntry->dbgProgramCounter = rs[i].floatingPoint->dbgProgramCounter;
						robEntry->dbgInstruction = rs[i].floatingPoint->dbgInstruction;
#endif					
					}		
					break;
				
				case LoadStoreUnitIdent:
					{
						nNonBranchInstructionsToDispatch++;
#ifdef DEBUG
						if(Debug(DebugDispatchUnit))
							cout << "unit : load/store" << endl;
#endif
#ifdef TRACE
						if(Trace(TraceDispatchUnit))
						{
							trace_file->Value("number", instructionQueueEntry.dbgInstructionCounter);
							trace_file->Value("pc", instructionQueueEntry.pc);
							trace_file->Value("unit", "load/store");
						}
#endif
						regnum_t srcReg;
					
						/* First operand can be either a floating point register or a general purpose register */
						if(decodedInstructions[i].operation.loadStore.floatingPoint)
						{
							srcReg = decodedInstructions[i].io.loadStore.srcReg[0];
							
							if(srcReg >= 0)
							{
								/* First operand is a floating point register */
#ifdef DEBUG
								if(Debug(DebugDispatchUnit))
									cout << "source register : fp" << (int) srcReg << endl;
#endif
								tag_t tag = registersBinding->GetFloatingPointRenameBuffer(srcReg);	/* Get the floating point rename buffer */
								rs[i].loadStore->operand0.tag = tag;			/* and store its tag into the reservation station */
								rs[i].loadStore->operand0.valid = false;		/* make the reservation to wait for the rename buffer value */
							
								if(tag < 0)
								{
									 /* The source register is not renamed */
#ifdef DEBUG	
									if(Debug(DebugDispatchUnit))
									{
										cout << name() << ": Reading fp" << (int) srcReg << " value from register file" << endl;
										if(readFloatingPointRegisterPort >= nReadFloatingPointRegisterPorts)
										{
											cout << name() << ": not enough read floating point register ports" << endl;
											ABORT();
										}
									}
#endif
									loadStoreReadingFloatingPointRegister[i] = srcReg;	/* Remember that we are reading the source floating point register */
									loadStoreReadingFloatingPointRenameBuffer[i] = -1;	/* No floating point rename buffer is being read */
									loadStoreReadingRegisters[i][0] = -1;				/* No general purpose register is being read */
									loadStoreReadingRenameBuffers[i][0] = -1;			/* No rename buffer is being read */
									outReadFloatingPointRegisterNumber[readFloatingPointRegisterPort++] = srcReg;	/* Ask to the register file the value of the floating point source register hold by the floating point rename buffer */
								}
								else
								{
									/* The source register is renamed */
								
									if(registersBinding->FloatingPointRenameBufferValid(tag))
									{
										/* The floating point rename buffer yet contains the right value */
#ifdef DEBUG
										if(Debug(DebugDispatchUnit))
										{
											cout << name() << ": Reading fp" << (int) srcReg << " value from rfp" << (int) tag << endl;
											if(readFloatingPointRenamePort >= nReadFloatingPointRenamePorts)
											{
												cout << name() << ": not enough read floating point rename buffer ports" << endl;
												ABORT();
											}
										}
#endif
										loadStoreReadingFloatingPointRegister[i] = -1;		/* No floating point register is being read */
										loadStoreReadingFloatingPointRenameBuffer[i] = tag;	/* Remember that we are reading the floating point rename buffer */
										loadStoreReadingRegisters[i][0] = -1;    			/* No general purpose register is being read */
										loadStoreReadingRenameBuffers[i][0] = -1;  			/* No rename buffer is being read */
										outReadFloatingPointRenameNumber[readFloatingPointRenamePort++] = tag;	/* Ask to the register file the value of the floating point rename buffer that holds the source operand value */
									}
									else
									{
#ifdef DEBUG
										if(Debug(DebugDispatchUnit))
											cout << name() << ": Waiting for fp" << (int) srcReg << " value hold by rfp" << (int) tag << endl;
#endif		
										loadStoreReadingFloatingPointRegister[i] = -1;
										loadStoreReadingFloatingPointRenameBuffer[i] = -1;
										loadStoreReadingRegisters[i][0] = -1;
										loadStoreReadingRenameBuffers[i][0] = -1;
										/* Nothing to read from register file, just wait */
									}
								}
							}
							else
							{
								/* No source register */
								loadStoreReadingFloatingPointRegister[i] = -1;
								loadStoreReadingFloatingPointRenameBuffer[i] = -1;
								loadStoreReadingRegisters[i][0] = -1;
								loadStoreReadingRenameBuffers[i][0] = -1;
								rs[i].loadStore->operand0.tag = -1;
							}
						}
						else
						{
							loadStoreReadingFloatingPointRegister[i] = -1;
							loadStoreReadingFloatingPointRenameBuffer[i] = -1;
						
							srcReg = decodedInstructions[i].io.loadStore.srcReg[0];
						
							if(srcReg >= 0)
							{
								/* First operand is a general purpose register */
#ifdef DEBUG
								if(Debug(DebugDispatchUnit))
									cout << "source register : r" << (int) srcReg << endl;
#endif
								tag_t tag = registersBinding->GetRenameBuffer(srcReg);	/* Get the rename buffer */
								rs[i].loadStore->operand0.tag = tag;		/* and store its tag into the reservation station */
								rs[i].loadStore->operand0.valid = false;	/* make the reservation to wait for the rename buffer */
								
								if(tag < 0)
								{
									 /* The source register is not renamed */
#ifdef DEBUG	
									if(Debug(DebugDispatchUnit))
									{
										cout << name() << ": Reading r" << (int) srcReg << " value from register file" << endl;
										if(readRegisterPort >= nReadRegisterPorts)
										{
											cout << name() << ": not enough read register ports" << endl;
											ABORT();
										}
									}
#endif
									loadStoreReadingRegisters[i][0] = srcReg;	/* Remember that we are reading the source register */
									loadStoreReadingRenameBuffers[i][0] = -1;	/* No rename buffer is being read */
									outReadRegisterNumber[readRegisterPort++] = srcReg;	/* Ask to the register file the source register value */
								}
								else
								{
									/* The source register is renamed */
									
									if(registersBinding->RenameBufferValid(tag))
									{
										/* The rename buffer yet contains the right value */
#ifdef DEBUG
										if(Debug(DebugDispatchUnit))
										{
											cout << name() << ": Reading r" << (int) srcReg << " value from rr" << (int) tag << endl;
											if(readRenamePort >= nReadRenamePorts)
											{
												cout << name() << ": not enough read rename buffer ports" << endl;
												ABORT();
											}
										}
#endif
										loadStoreReadingRegisters[i][0] = -1;			/* No register is being read */
										loadStoreReadingRenameBuffers[i][0] = tag;		/* Remember that we are reading a rename buffer */
										outReadRenameNumber[readRenamePort++] = tag;	/* Ask to the register file the value of the rename buffer */
									}
									else
									{
#ifdef DEBUG
										if(Debug(DebugDispatchUnit))
											cout << name() << ": Waiting for r" << (int) srcReg << " value hold by rr" << (int) tag << endl;
#endif	
										loadStoreReadingRegisters[i][0] = -1;			/* No register is being read */
										loadStoreReadingRenameBuffers[i][0] = -1;		/* No rename buffer is being read */
										/* Just wait for the value to be produced by functional units */
									}
								}
							}
							else
							{
								/* No source register */
								loadStoreReadingRegisters[i][0] = -1;
								loadStoreReadingRenameBuffers[i][0] = -1;
								rs[i].loadStore->operand0.tag = -1;
							}
						}
					
						// operand 1
						srcReg = decodedInstructions[i].io.loadStore.srcReg[1];
						
						if(srcReg >= 0)
						{
							/* Instruction has a source register */
#ifdef DEBUG
							if(Debug(DebugDispatchUnit))
								cout << "source register : r" << (int) srcReg << endl;
#endif
							tag_t tag = registersBinding->GetRenameBuffer(srcReg);	/* Get the rename buffer, */
							rs[i].loadStore->operand1.tag = tag;		/* and store its tag into the reservation station */
							rs[i].loadStore->operand1.valid = false;	/* make the reservation station to wait for the rename buffer */
								
							if(tag < 0)
							{
								 /* The source register is not renamed */
#ifdef DEBUG	
								if(Debug(DebugDispatchUnit))
								{
									cout << name() << ": Reading r" << (int) srcReg << " value from register file" << endl;
									if(readRegisterPort >= nReadRegisterPorts)
									{
										cout << name() << ": not enough read register ports" << endl;
										ABORT();
									}
								}
#endif
								loadStoreReadingRegisters[i][1] = srcReg;	/* Remember that we are reading a register */
								loadStoreReadingRenameBuffers[i][1] = -1;	/* No rename buffer is being read */
								outReadRegisterNumber[readRegisterPort++] = srcReg;	/* Ask to the register the value of the register */
							}
							else
							{
								/* The source register is renamed */
									
								if(registersBinding->RenameBufferValid(tag))
								{
									/* The rename yet contains the right value */
#ifdef DEBUG
									if(Debug(DebugDispatchUnit))
									{
										cout << name() << ": Reading r" << (int) srcReg << " value from rr" << (int) tag << endl;
										if(readRenamePort >= nReadRenamePorts)
										{
											cout << name() << ": not enough read rename buffer ports" << endl;
											ABORT();
										}
									}
#endif
									loadStoreReadingRegisters[i][1] = -1;			/* No register is being read */
									loadStoreReadingRenameBuffers[i][1] = tag;		/* Remember that we are reading the rename buffer */
									outReadRenameNumber[readRenamePort++] = tag;	/* Ask to the register the value of the rename buffer */
								}
								else
								{
#ifdef DEBUG
									if(Debug(DebugDispatchUnit))
										cout << name() << ": Waiting for r" << (int) srcReg << " value hold by rr" << (int) tag << endl;
#endif	
									loadStoreReadingRegisters[i][1] = -1;
									loadStoreReadingRenameBuffers[i][1] = -1;
									/* Just wait for the value to be produced by functional units */
								}
							}
						}
						else
						{
							/* No source register */
							loadStoreReadingRegisters[i][1] = -1;
							loadStoreReadingRenameBuffers[i][1] = -1;
							rs[i].loadStore->operand1.tag = -1;
						}

						// operand 2
						srcReg = decodedInstructions[i].io.loadStore.srcReg[2];
						
						if(srcReg >= 0)
						{
							/* Instruction has a source register */
#ifdef DEBUG
							if(Debug(DebugDispatchUnit))
								cout << "source register : r" << (int) srcReg << endl;
#endif
							tag_t tag = registersBinding->GetRenameBuffer(srcReg);	/* Get the rename buffer, */
							rs[i].loadStore->operand2.tag = tag;		/* and store its tag into the reservation station */
							rs[i].loadStore->operand2.valid = false;	/* make the reservation station to wait for the rename buffer */
								
							if(tag < 0)
							{
								 /* The source register is not renamed */
#ifdef DEBUG	
								if(Debug(DebugDispatchUnit))
								{
									cout << name() << ": Reading r" << (int) srcReg << " value from register file" << endl;
									if(readRegisterPort >= nReadRegisterPorts)
									{
										cout << name() << ": not enough read register ports" << endl;
										ABORT();
									}
								}
#endif
								loadStoreReadingRegisters[i][2] = srcReg;	/* Remember that we are reading the source register */
								loadStoreReadingRenameBuffers[i][2] = -1;	/* No rename buffer is being read */
								outReadRegisterNumber[readRegisterPort++] = srcReg;	/* Ask to the register file the value of the source register */
							}
							else
							{
								/* The source register is renamed */
									
								if(registersBinding->RenameBufferValid(tag))
								{
									/* The rename buffer yet contains the right value */
#ifdef DEBUG
									if(Debug(DebugDispatchUnit))
									{
										cout << name() << ": Reading r" << (int) srcReg << " value from rr" << (int) tag << endl;
										if(readRenamePort >= nReadRenamePorts)
										{
											cout << name() << ": not enough read rename buffer ports" << endl;
											ABORT();
										}
									}
#endif
									loadStoreReadingRegisters[i][2] = -1;		/* No register is being read */
									loadStoreReadingRenameBuffers[i][2] = tag;	/* Remember that we are reading a rename buffer */
									outReadRenameNumber[readRenamePort++] = tag;	/* Ask to the register file the value of the rename buffer */
								}
								else
								{
#ifdef DEBUG
									if(Debug(DebugDispatchUnit))
										cout << name() << ": Waiting for r" << (int) srcReg << " value hold by rr" << (int) tag << endl;
#endif	
									loadStoreReadingRegisters[i][2] = -1;
									loadStoreReadingRenameBuffers[i][2] = -1;
									/* Just wait for the value to be produced by the functional units */
								}
							}
						}
						else
						{
							/* No source operand */
							loadStoreReadingRegisters[i][2] = -1;
							loadStoreReadingRenameBuffers[i][2] = -1;
							rs[i].loadStore->operand2.tag = -1;
						}
												
						
						/* Store the load/store operation into the reservation station */
						rs[i].loadStore->operation = decodedInstructions[i].operation.loadStore;
#if defined(DEBUG) || defined(TRACE)
						{
							const InstructionQueueEntry& instructionQueueEntry = *queue[i];
							rs[i].loadStore->dbgInstructionCounter = instructionQueueEntry.dbgInstructionCounter;
							rs[i].loadStore->dbgProgramCounter = instructionQueueEntry.pc;
							rs[i].loadStore->dbgInstruction = instructionQueueEntry.instruction;
						}
#endif
						if(decodedInstructions[i].hasImmed)
						{
							// store the immediate value into the reservation station
							rs[i].loadStore->operand2.data = decodedInstructions[i].immed;
#ifdef DEBUG
							if(Debug(DebugDispatchUnit))
							{
								cout << "immediate : ";
								WriteHex(cout, decodedInstructions[i].immed);
								cout << endl;
							}
#endif
						}
	
						// destination 0
						if(decodedInstructions[i].operation.loadStore.floatingPoint)
						{
							regnum_t dstReg = decodedInstructions[i].io.loadStore.dstReg[0];
					
							if(dstReg >= 0)
							{
								/* Instruction has a destination floating point register */
#ifdef DEBUG
								if(Debug(DebugDispatchUnit))
									cout << "destination register (0)" << ": fp" << (int) dstReg << endl;
#endif
								/* Allocate a floating point rename buffer, and store its tag into the reservation station */
								rs[i].loadStore->tags.resultTags[0] = registersBinding->AllocateFloatingPointRenameBuffer(dstReg);
							}
							else
							{
								/* Instruction has no destination floating point register */
								rs[i].loadStore->tags.resultTags[0] = -1;
							}
						}
						else
						{
							regnum_t dstReg = decodedInstructions[i].io.loadStore.dstReg[0];
				
							if(dstReg >= 0)
							{
								/* Instruction has a destination register */
#ifdef DEBUG
								if(Debug(DebugDispatchUnit))
									cout << "destination register (0)" << ": r" << (int) dstReg << endl;
#endif
								/* Allocate a rename buffer, and store its tag into the reservation station */
								rs[i].loadStore->tags.resultTags[0] = registersBinding->AllocateRenameBuffer(dstReg);
							}
							else
							{
								/* Instruction has no destination register */
								rs[i].loadStore->tags.resultTags[0] = -1;
							}
						}
						
						// destination 1
						regnum_t dstReg = decodedInstructions[i].io.loadStore.dstReg[1];
					
						if(dstReg >= 0)
						{
							/* Instruction has a destination register */
#ifdef DEBUG
							if(Debug(DebugDispatchUnit))
								cout << "destination register (1)" << ": r" << (int) dstReg << endl;
#endif
								/* Allocate a rename buffer, and store its tag into the reservation station */
								rs[i].loadStore->tags.resultTags[1] = registersBinding->AllocateRenameBuffer(dstReg);
						}
						else
						{
							/* Instruction has no destination register */
							rs[i].loadStore->tags.resultTags[1] = -1;
						}
						
						/* Store the instruction tag into the reservation station */
						rs[i].loadStore->tags.tag = rob->GetIndex(*robEntry);
						rs[i].loadStore->branchCounter = branchCounter;
						
						/* Store some other usefull informations into the reoder buffer entry */		
						robEntry->extra.loadStore.write = rs[i].loadStore->operation.write;
						robEntry->extra.loadStore.tags = rs[i].loadStore->tags;
						robEntry->branchCounter = rs[i].loadStore->branchCounter;
						robEntry->executionSerialized = false;
						robEntry->extra.loadStore.floatingPoint = rs[i].loadStore->operation.floatingPoint;
#if defined(DEBUG) || defined(TRACE)
						robEntry->dbgInstructionCounter = rs[i].loadStore->dbgInstructionCounter;
						robEntry->dbgProgramCounter = rs[i].loadStore->dbgProgramCounter;
						robEntry->dbgInstruction = rs[i].loadStore->dbgInstruction;
#endif					
					}
					break;
					
				case BranchUnitIdent:
					if(queue[i]->branchPredictedTaken)
						nBranchInstructionsToDispatch++;
					else
						nNonBranchInstructionsToDispatch++;
#ifdef DEBUG
					if(Debug(DebugDispatchUnit))
						cout << "unit : branch" << endl;
#endif
#ifdef TRACE
					if(Trace(TraceDispatchUnit))
					{
						trace_file->Value("number", instructionQueueEntry.dbgInstructionCounter);
						trace_file->Value("pc", instructionQueueEntry.pc);
						trace_file->Value("unit", "branch");
					}
#endif
					if(decodedInstructions[i].io.branch.inCR)
					{
						/* Instruction has a source CR field */
						int crf = decodedInstructions[i].io.branch.crf;
#ifdef DEBUG
						if(Debug(DebugDispatchUnit))
							cout << "source register : CR" << crf << endl;
#endif
						tag_t tag = registersBinding->GetCRRenameBuffer(crf);	/* Get the CR field rename buffer */
						rs[i].branch->cr.tag = tag;		/* and store its tag into the reservation station */
						rs[i].branch->cr.valid = false;	/* make the reservation station to wait for the rename buffer */
						
						if(tag >= 0)
						{
							/* The CR field is renamed */
							if(registersBinding->CRRenameBufferValid(tag))
							{
								/* The rename buffer yet contains the right value */
#ifdef DEBUG
								if(Debug(DebugDispatchUnit))
								{
									if(readRenameCRPort >= nReadRenameCRPorts)
									{
										cout << name() << ": not enough read CR rename buffer ports" << endl;
										ABORT();
									}
								}
#endif
								branchReadingCR[i] = -1;			/* No CR field register is being read */
								branchReadingRenameCR[i] = tag;		/* Remember that we are reading the rename buffer */
								outReadRenameCRNumber[readRenameCRPort++] = tag;	/* Ask the rename buffer value */
							}
							else
							{
								branchReadingCR[i] = -1;
								branchReadingRenameCR[i] = -1;
								/* Just wait for the value to be produced by the functional units */
							}
						}
						else
						{
							/* The CR field is not renamed */
							branchReadingCR[i] = 0;		/* Remember that we are reading CR */
							branchReadingRenameCR[i] = -1;	/* No rename buffer is being read */
						}
					}
					else
					{
						/* No source CR field */
						branchReadingCR[i] = -1;
						branchReadingRenameCR[i] = -1;
						rs[i].branch->cr.tag = -1;
					}
				
					if(decodedInstructions[i].io.branch.inLR)
					{
						/* Instruction has LR as source register */
#ifdef DEBUG
						if(Debug(DebugDispatchUnit))
							cout << "source register : LR" << endl;
#endif
						tag_t tag = registersBinding->GetLRRenameBuffer();	/* Get the rename buffer */
						rs[i].branch->lr.tag = tag;		/* and store its tag into the reservation station */
						rs[i].branch->lr.valid = false;	/* make the reservation station to wait for the rename buffer */
						
						if(tag >= 0)
						{
							/* LR is renamed */
							if(registersBinding->LRRenameBufferValid(tag))
							{
								/* The LR rename buffer yet contains the right value */
#ifdef DEBUG
								if(Debug(DebugDispatchUnit))
									if(readRenameLRPort >= nReadRenameLRPorts)
									{
										cout << name() << ": not enough read LR rename buffer ports" << endl;
										ABORT();
									}
#endif
								branchReadingLR[i] = -1;			/* Don't read LR */
								branchReadingRenameLR[i] = tag;		/* instead, read from the rename buffer */
								outReadRenameLRNumber[readRenameLRPort++] = tag;	/* Ask the rename buffer value */
							}
							else
							{
								branchReadingLR[i] = -1;
								branchReadingRenameLR[i] = -1;
								/* Just wait for the value to be produce by functional units */
							}
						}
						else
						{
							/* LR is not renamed */
							branchReadingLR[i] = 0;			/* read its value directly from the register */
							branchReadingRenameLR[i] = -1;	/* No LR rename buffer is being read */
						}
					}
					else
					{
						/* LR is not a source register */
						branchReadingLR[i] = -1;
						branchReadingRenameLR[i] = -1;
						rs[i].branch->lr.tag = -1;
					}
					
					if(decodedInstructions[i].io.branch.inCTR)
					{
						/* Instruction has CTR as source register */
#ifdef DEBUG
						if(Debug(DebugDispatchUnit))
							cout << "source register : CTR" << endl;
#endif
						tag_t tag = registersBinding->GetCTRRenameBuffer();	/* Get the CTR rename buffer */
						rs[i].branch->ctr.tag = tag;		/* and store its tag into the reservation station */
						rs[i].branch->ctr.valid = false;	/* make the reservation station to wait for the LR rename buffer */
						
						if(tag >= 0)
						{
							/* CTR is renamed */
							if(registersBinding->CTRRenameBufferValid(tag))
							{
								/* The CTR rename buffer yet contains the right value */
#ifdef DEBUG
								if(Debug(DebugDispatchUnit))
									if(readRenameCTRPort >= nReadRenameCTRPorts)
									{
										cout << name() << ": not enough read CTR rename buffer ports" << endl;
										ABORT();
									}
#endif
								branchReadingCTR[i] = -1;			/* Don't read CTR directly from the register */
								branchReadingRenameCTR[i] = tag;	/* instead, read its value from the rename buffer */
								outReadRenameCTRNumber[readRenameCTRPort++] = tag;	/* Ask for the rename buffer value */
							}
							else
							{
								branchReadingCTR[i] = -1;
								branchReadingRenameCTR[i] = -1;
								/* Just wait for the value to be produced by functional units */
							}
						}
						else
						{
							branchReadingCTR[i] = 0;		/* read the CTR value directly from the register */
							branchReadingRenameCTR[i] = -1;	/* No rename buffer is being read */
						}
					}
					else
					{
						/* CTR is not a source register for that instruction */
						branchReadingCTR[i] = -1;
						branchReadingRenameCTR[i] = -1;
						rs[i].branch->ctr.tag = -1;
					}			
					
					/* Store the branch operation into the reservation station */
					rs[i].branch->operation = decodedInstructions[i].operation.branch;
#if defined(DEBUG) || defined(TRACE)
					{
						const InstructionQueueEntry& instructionQueueEntry = *queue[i];
						rs[i].branch->dbgInstructionCounter = instructionQueueEntry.dbgInstructionCounter;
						rs[i].branch->dbgInstruction = instructionQueueEntry.instruction;
					}
#endif

					/* Store the immediate value into the reservation station */
					if(decodedInstructions[i].hasImmed)
						rs[i].branch->immed = decodedInstructions[i].immed;			
									
					/* Get the tags of the destination registers, and store them into the reservation stattion */
					rs[i].branch->tags.LRTag = decodedInstructions[i].io.branch.outLR ? registersBinding->AllocateLRRenameBuffer() : -1;
					rs[i].branch->tags.CTRTag = decodedInstructions[i].io.branch.outCTR ? registersBinding->AllocateCTRRenameBuffer() : -1;
					
					{
						const InstructionQueueEntry& instructionQueueEntry = *queue[i];
						rs[i].branch->branchPredictedTaken = instructionQueueEntry.branchPredictedTaken;
						rs[i].branch->pc = instructionQueueEntry.pc;
					}

					/* Store the instruction tag into the reservation station */
					rs[i].branch->tags.tag = robEntry ? rob->GetIndex(*robEntry) : -1;
					
					/* Store some other usefull informations into the reorder buffer entry */
					rs[i].branch->branchCounter = branchCounter;
					branchCounter++;								

					if(robEntry)
					{
						robEntry->extra.branch.tags = rs[i].branch->tags;
						robEntry->branchCounter = rs[i].branch->branchCounter;
						robEntry->executionSerialized = false;
#if defined(DEBUG) || defined(TRACE)
						robEntry->dbgInstructionCounter = rs[i].branch->dbgInstructionCounter;
						robEntry->dbgProgramCounter = rs[i].branch->pc;
						robEntry->dbgInstruction = rs[i].branch->dbgInstruction;				
#endif
					}
					break;				
					
				case SystemRegisterUnitIdent:
					nNonBranchInstructionsToDispatch++;
#ifdef DEBUG
					if(Debug(DebugDispatchUnit))
						cout << "unit : System register" << endl;
#endif
#ifdef TRACE
					if(Trace(TraceDispatchUnit))
					{
						trace_file->Value("number", instructionQueueEntry.dbgInstructionCounter);
						trace_file->Value("pc", instructionQueueEntry.pc);
						trace_file->Value("unit", "system register");
					}
#endif
					/* Store the system register unit operation into the reservation station */
					rs[i].systemRegister->operation = decodedInstructions[i].operation.systemRegister;
					rs[i].systemRegister->branchCounter = branchCounter;
#if defined(DEBUG) || defined(TRACE)
					{
						const InstructionQueueEntry& instructionQueueEntry = *queue[i];
						rs[i].systemRegister->dbgInstructionCounter = instructionQueueEntry.dbgInstructionCounter;
						rs[i].systemRegister->dbgProgramCounter = instructionQueueEntry.pc;
						rs[i].systemRegister->dbgInstruction = instructionQueueEntry.instruction;
					}
#endif
					
					switch(decodedInstructions[i].iclass)
					{
						case ICMoveToSpr:
							{	
								if(decodedInstructions[i].io.mtspr.dstSpr == 8)
								{
									/* LR is a destination register for that instruction */
#ifdef DEBUG
									if(Debug(DebugDispatchUnit))
										cout << "destination register : LR" << endl;
#endif
									/* Allocate a LR rename buffer, and store its tag into the reservation station */
									rs[i].systemRegister->tags.LRTag = registersBinding->AllocateLRRenameBuffer();
								}
								else
								{
									/* LR is not a destination register */
									rs[i].systemRegister->tags.LRTag = -1;
								}
							
								if(decodedInstructions[i].io.mtspr.dstSpr == 9)
								{
									/* CTR is a destination register for that instruction */
#ifdef DEBUG
									if(Debug(DebugDispatchUnit))
										cout << "destination register : CTR" << endl;
#endif	
									/* Allocate a CTR rename buffer, and its tag into the reservation station */
									rs[i].systemRegister->tags.CTRTag = registersBinding->AllocateCTRRenameBuffer();
								}
								else
								{
									/* CTR is not a destination register */
									rs[i].systemRegister->tags.CTRTag = -1;
								}
							
								rs[i].systemRegister->tags.resultTag = -1;	/* Instruction does not modify any general purpose registers */
								rs[i].systemRegister->tags.CRTag = -1;		/* Instruction does not modify any CR field */
								rs[i].systemRegister->interlock = false;	/* Instruction does not set interlock bit */
							}
							break;
							
						case ICMoveFromSpr:
							{					
								regnum_t dstReg = decodedInstructions[i].io.mfspr.dstReg;
								
								if(dstReg >= 0)
								{
									/* Instruction has a destination register */
#ifdef DEBUG
									if(Debug(DebugDispatchUnit))
										cout << "destination register : r" << (int) dstReg << endl;
#endif
									/* Allocate a rename buffer, and store its tag into the reservation station */
									rs[i].systemRegister->tags.resultTag = registersBinding->AllocateRenameBuffer(dstReg);
								}
								else
								{
									rs[i].systemRegister->tags.resultTag = -1;
								}
								
								rs[i].systemRegister->tags.CRTag = -1;	/* Instruction does not modify any CR field */
								rs[i].systemRegister->tags.LRTag = -1;	/* Instruction does not modify LR */
								rs[i].systemRegister->tags.CTRTag = -1;	/* Instruction does not modify CTR */
								rs[i].systemRegister->interlock = false;	/* Instruction does not set interlock bit */
							}
							break;
						
						case ICMoveFromTbr:
							{					
								regnum_t dstReg = decodedInstructions[i].io.mftb.dstReg;
								
								if(dstReg >= 0)
								{
									/* Instruction has a destination register */
#ifdef DEBUG
									if(Debug(DebugDispatchUnit))
										cout << "destination register : r" << (int) dstReg << endl;
#endif
									/* Allocate a rename buffer, and store its tag into the reservation station */
									rs[i].systemRegister->tags.resultTag = registersBinding->AllocateRenameBuffer(dstReg);
								}
								else
								{
									rs[i].systemRegister->tags.resultTag = -1;
								}
								
								rs[i].systemRegister->tags.CRTag = -1;	/* Instruction does not modify any CR field */
								rs[i].systemRegister->tags.LRTag = -1;	/* Instruction does not modify LR */
								rs[i].systemRegister->tags.CTRTag = -1;	/* Instruction does not modify CTR */
								rs[i].systemRegister->interlock = false;	/* Instruction does not set interlock bit */
							}
							break;
						
						case ICCRLogical:
							{
								/* Instruction has a destination CR field */
								regnum_t crf = decodedInstructions[i].io.crlogical.dstCrf;
#ifdef DEBUG
								if(Debug(DebugDispatchUnit))
									cout << "destination register : CR" << (int) crf << endl;
#endif
								/* Allocate a CR field rename buffer, and store its tag into the reservation station */
								rs[i].systemRegister->tags.CRTag = registersBinding->AllocateCRRenameBuffer(crf);
								rs[i].systemRegister->tags.resultTag = -1;	/* Instruction does not modify any general purpose register */
								rs[i].systemRegister->tags.LRTag = -1;		/* Instruction does not modify LR */
								rs[i].systemRegister->tags.CTRTag = -1;		/* Instruction does not modify CTR */
								rs[i].systemRegister->interlock = false;	/* Instruction does not set interlock bit */
								}
							break;
							
						case ICMoveFromCR:
							{
								regnum_t dstReg = decodedInstructions[i].io.mfcr.dstReg;
								
								if(dstReg >= 0)
								{
									/* Instruction has a destination register */
#ifdef DEBUG
									if(Debug(DebugDispatchUnit))
										cout << "destination register : r" << (int) dstReg << endl;
#endif
									/* Allocate a rename buffer, and store its tag into the reservation station */
									rs[i].systemRegister->tags.resultTag = registersBinding->AllocateRenameBuffer(dstReg);
								}
								else
								{
									rs[i].systemRegister->tags.resultTag = -1;
								}
								
								/* Instruction does not modifies any other register */
								rs[i].systemRegister->tags.CRTag = -1;
								rs[i].systemRegister->tags.LRTag = -1;
								rs[i].systemRegister->tags.CTRTag = -1;
								rs[i].systemRegister->interlock = false;
							}
							break;
							
						case ICSystemCall:
							/* System Call sets the interlock bit (so no instructions is decoded until it is finished) */
							rs[i].systemRegister->tags.resultTag = -1;
							rs[i].systemRegister->tags.CRTag = -1;
							rs[i].systemRegister->tags.LRTag = -1;
							rs[i].systemRegister->tags.CTRTag = -1;
							interlock = rs[i].systemRegister->interlock = true;
							break;
					
						case ICMoveToCrf:
							{
								/* Instruction has one or more destination CR fields */
								/* When several fiels are modified crf contains -1 */
								regnum_t crf = decodedInstructions[i].io.mtcrf.dstCrf;
#ifdef DEBUG
								if(Debug(DebugDispatchUnit))
								{
									if(crf >= 0)
										cout << "destination register : CR" << (int) crf << endl;
									else
										cout << "destination register : CR" << endl;
								}
#endif
								/* Allocate a CR field rename buffer, and store its tag into the reservation station */
								rs[i].systemRegister->tags.CRTag = crf >= 0 ? registersBinding->AllocateCRRenameBuffer(crf) : -1;
								/* No other registers is altered by the instruction */
								rs[i].systemRegister->tags.resultTag = -1;
								rs[i].systemRegister->tags.LRTag = -1;
								rs[i].systemRegister->tags.CTRTag = -1;
								/* The instruction set the interlock bit if severals CR fields are modified */
								interlock = rs[i].systemRegister->interlock = crf < 0;																
							}
							break;
					
						case ICMoveCrf:
							{
								/* Instruction has one destination CR field */
								regnum_t crf = decodedInstructions[i].io.mcrf.dstCrf;
#ifdef DEBUG
								if(Debug(DebugDispatchUnit))
								{
									if(crf >= 0)
										cout << "destination register : CR" << (int) crf << endl;
									else
										cout << "destination register : CR" << endl;
								}
#endif
								/* Allocate a CR field rename buffer, and store its tag into the reservation station */
								rs[i].systemRegister->tags.CRTag = crf >= 0 ? registersBinding->AllocateCRRenameBuffer(crf) : -1;
								
								/* No other registers are modified by the instruction */
								rs[i].systemRegister->tags.resultTag = -1;
								rs[i].systemRegister->tags.LRTag = -1;
								rs[i].systemRegister->tags.CTRTag = -1;
							}
							break;
					}
					
					/* Store the instruction tag into the reservation station */
					rs[i].systemRegister->tags.tag = rob->GetIndex(*robEntry);
					
					/* Fill the reorder buffer entry */
					robEntry->extra.systemRegister.tags = rs[i].systemRegister->tags;			
					robEntry->extra.systemRegister.interlock = rs[i].systemRegister->interlock;
					
					if(rs[i].systemRegister->operation.ident == ID_MTCRF && rs[i].systemRegister->tags.CRTag < 0)
					{
						/* If the instruction modifies several CR fields then we will need to write with a mask into CR */
						robEntry->extra.systemRegister.writeCR = true;
						robEntry->extra.systemRegister.crm = rs[i].systemRegister->operation.extra.mtcrf.crm;
					}
					else
					{
						robEntry->extra.systemRegister.writeCR = false;
					}
					
					/* All instruction executed into the system register unit are execution serialized */
					robEntry->executionSerialized = true;
					rs[i].systemRegister->executionSerialized = true;
					robEntry->branchCounter = rs[i].systemRegister->branchCounter;
#if defined(DEBUG) || defined(TRACE)
					robEntry->dbgInstructionCounter = rs[i].systemRegister->dbgInstructionCounter;
					robEntry->dbgProgramCounter = rs[i].systemRegister->dbgProgramCounter;
					robEntry->dbgInstruction = rs[i].systemRegister->dbgInstruction;
#endif
					break;
					
			}
#ifdef TRACE
			if(Trace(TraceDispatchUnit))
			{
				trace_file->End("instruction");
			}
#endif
		}
		
#ifdef TRACE
		if(Trace(TraceDispatchUnit))
		{
			trace_file->End("dispatch");
		}
#endif
	}
	else
	{
		nInstructionsToDispatch = 0;
		nNonBranchInstructionsToDispatch = 0;
		nBranchInstructionsToDispatch = 0;
	}
	
	outDispatchedInstructions = nNonBranchInstructionsToDispatch;
	outDispatchedBranchs = nBranchInstructionsToDispatch;
}

void DispatchUnit::Dispatch()
{
	int i, j;

	if(inBranchFinished)
	{
		/* A branch instruction has just resolved */
#ifdef DEBUG
		if(Debug(DebugDispatchUnit))
			cout << name() << ": Branch resolved" << endl;
#endif
		if(inMispredictedBranch)
		{
			/* The branch has been mispredicted, */
			/* so we have to flush some instructions hold by the reservation stations (those that have  branch counter > 0)*/
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << name() << ": Mispredicted branch" << endl;
#endif
			Flush();
			branchCounter = 0;
			branchFollow = false;
		}
		else
		{
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << name() << ": Decrement Branch Counter" << endl;
#endif
			/* Decrement the branch counter in each reservation station */
			DecrementBranchCounter();
		}
	}
	
		
	/*
	if(inBranchFinished)
	{
		//A branch instruction has just finished
#ifdef DEBUG
		if(Debug(DebugDispatchUnit))
			cout << name() << ": Branch finished" << endl;
#endif
		if(inMispredictedBranch)
		{
			// The branch has been mispredicted,
			// so we have to flush some instructions hold by the reservation stations (those that have  branch counter > 0)
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << name() << ": Mispredicted branch" << endl;
#endif
			Flush();
			branchCounter = 0;
		}
		else
		{
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << name() << ": Decrement Branch Counter" << endl;
#endif
			// Decrement the branch counter in each reservation station
			DecrementBranchCounter();
		}
	}
	*/
	
	if(interlock && inClearInterlock)
	{
		/* Clear the interlock bit */
		interlock = false;
	}

	/* Update the reservation stations with values produced */
	/* by the functional units and registers and rename buffers readings */	
	UpdateRS();
	
	/* Try to begin instructions that are ready to be executed */
	for(i = 0; i < nIntegerUnit; i++)
	{
		if(!inIntegerBusy[i] && !integerRSQueue[i].Empty())
		{
			bool execute = false;
			IntegerRS *rs = integerRSQueue[i].GetOldestValid();
			/* Get a reservation station that contains a ready instruction */
			
			if(rs)
			{
				if(rs->executionSerialized)
				{
					/* If instruction is serialized then check if completion unit allow the instruction to execute */
					if(inIntegerExecute[i] && inIntegerExecuteTag[i] == rs->tags.tag) execute = true;
				}
				else
				{
					execute = true;
				}
			}
			
			if(execute)
			{
				/* Instruction is ready to execute */
				unsigned int j;
			
				IntegerOperands integerOperands;
			
#ifdef DEBUG
				if(Debug(DebugDispatchUnit))
					cout << *rs << endl;
#endif
				for(j = 0; j < sizeof(integerOperands.data) / sizeof(integerOperands.data[0]); j++)
				{
					integerOperands.data[j] = rs->operands[j].data;
				}
				outIntegerOperands[i] = integerOperands;
				outIntegerOperation[i] = rs->operation;
				outIntegerTags[i] = rs->tags;
				outIntegerDispatched[i] = true;
				outIntegerBranchCounter[i] = rs->branchCounter;
#if defined(DEBUG) || defined(TRACE)
				outIntegerInstructionCounter[i] = rs->dbgInstructionCounter;
				outIntegerProgramCounter[i] = rs->dbgProgramCounter;
				outIntegerInstruction[i] = rs->dbgInstruction;
#endif
				
#ifdef DEBUG
				if(Debug(DebugDispatchUnit))
				{
					cout << name() << ": integer instruction   (" << rs->dbgInstructionCounter << ")  ";
					ppc_disassemble_to_stream(rs->dbgInstruction, rs->dbgProgramCounter, stdout);
					cout << "    (tag = " << (int) rs->tags.tag << ") ready to execute in integer unit " << i << endl;
				}
#endif
				/* Free the reservation station */
	 			integerRSQueue[i].Remove(rs);
			}
			else
			{
				outIntegerDispatched[i] = false;
			}
		}
		else
		{
			outIntegerDispatched[i] = false;
		}
	}
	
	for(i = 0; i < nFloatingPointUnits; i++)
	{
#ifdef DEBUG
		if(Debug(DebugDispatchUnit))
		{
			cout << name() << ": FloatingPointBusy[" << i << "] = " << (bool) inFloatingPointBusy[i] << endl;
			cout << name() << ": FloatingPointRSQueue[" << i << "].Empty() = " << floatingPointRSQueue[i].Empty() << endl;
		}
#endif
		if(!inFloatingPointBusy[i] && !floatingPointRSQueue[i].Empty())
		{
			bool execute = false;
			FloatingPointRS *rs = floatingPointRSQueue[i].GetOldestValid();	
			/* Get a reservation that contains a ready instruction */	
			
			if(rs)
			{
				if(rs->executionSerialized)
				{
					/* If instruction is serialized then check if the completion unit allow the instruction to execute */
					if(inFloatingPointExecute[i] && rs->tags.tag == inFloatingPointExecuteTag[i]) execute = true;
				}
				else
				{
					execute = true;
				}
				
				if(!FloatingPointUnit::Pipelined(rs->operation.ident) && !inFloatingPointPipelineEmpty[i])
				{
					execute = false;
				}
			}
			
			if(execute)
			{
				/* Instruction is read to execute */
				unsigned int j;
		
				FloatingPointOperands floatingPointOperands;
			
#ifdef DEBUG
				if(Debug(DebugDispatchUnit))
					cout << *rs << endl;
#endif
				for(j = 0; j < sizeof(floatingPointOperands.data) / sizeof(floatingPointOperands.data[0]); j++)
				{
					floatingPointOperands.data[j] = rs->operands[j].data;
				}
				outFloatingPointOperands[i] = floatingPointOperands;
				outFloatingPointOperation[i] = rs->operation;
				outFloatingPointTags[i] = rs->tags;
				outFloatingPointDispatched[i] = true;
				outFloatingPointBranchCounter[i] = rs->branchCounter;
#if defined(DEBUG) || defined(TRACE)
				outFloatingPointInstructionCounter[i] = rs->dbgInstructionCounter;
				outFloatingPointProgramCounter[i] = rs->dbgProgramCounter;
				outFloatingPointInstruction[i] = rs->dbgInstruction;
#endif
				
#ifdef DEBUG
				if(Debug(DebugDispatchUnit))
				{
					cout << name() << ": floating point instruction   (" << rs->dbgInstructionCounter << ")  ";
					ppc_disassemble_to_stream(rs->dbgInstruction, rs->dbgProgramCounter, stdout);
					cout << "    (tag = " << (int) rs->tags.tag << ") ready to execute" << endl;
				}
#endif
				/* Free the reservation station */
	 			floatingPointRSQueue[i].Remove(rs);
			}
			else
			{
				outFloatingPointDispatched[i] = false;
			}
		}
		else
		{
			outFloatingPointDispatched[i] = false;
		}
	}
	
	if(!loadStoreRSQueue.Empty())
	{	
		LoadStoreRS& rs = loadStoreRSQueue.GetTail();
		/* Get the reservation station that has the oldest loas/store instruction */
	
#ifdef DEBUG
		if(Debug(DebugDispatchUnit))
		{
			cout << name() << ": Store queue stalled = " << (bool) inStoreQueueStalled << endl;
			cout << name() << ": Load queue stalled = " << (bool) inLoadQueueStalled << endl;
		}
#endif
		if(rs.Valid() && ((rs.operation.write && !inStoreQueueStalled) || (!rs.operation.write && !inLoadQueueStalled)))
		{
			/* If instruction is ready to execute, and load/store queues can accept a new instruction, then the instruction is started */
			unsigned int j;		
			LoadStoreOperands loadStoreOperands;
			
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << rs << endl;
#endif
			loadStoreOperands.data0 = rs.operand0.data;
			loadStoreOperands.data1 = rs.operand1.data;
			loadStoreOperands.data2 = rs.operand2.data;
		
			outLoadStoreOperands = loadStoreOperands;
			outLoadStoreOperation = rs.operation;
			outLoadStoreTags = rs.tags;
			outLoadStoreBranchCounter = rs.branchCounter;
			outLoadStoreDispatched = true;
#if defined(DEBUG) || defined(TRACE)
			outLoadStoreInstructionCounter = rs.dbgInstructionCounter;
			outLoadStoreProgramCounter = rs.dbgProgramCounter;
			outLoadStoreInstruction = rs.dbgInstruction;
#endif
	
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << name() << ": load/store instruction (tag = " << (int) rs.tags.tag << ") ready to execute" << endl;
#endif
			/* Free the reservation station */
			loadStoreRSQueue.ReleaseTail();
		}
		else
		{
			outLoadStoreDispatched = false;
		}
	}
	else
	{
		outLoadStoreDispatched = false;
	}
	
	if(!branchRSQueue.Empty())
	{
		BranchRS& rs = branchRSQueue.GetTail();
		/* Get the oldest branch instruction */
		
		if(!inBranchBusy && rs.Valid())
		{
			/* Branch is ready to be executed */
			BranchOperands branchOperands;
			
			branchOperands.immed = rs.immed;
			branchOperands.pc = rs.pc;
			branchOperands.crfield = rs.cr.crfield;
			branchOperands.lr = rs.lr.data;
			branchOperands.ctr = rs.ctr.data;
			branchOperands.branchPredictedTaken = rs.branchPredictedTaken;
			
			outBranchOperands = branchOperands;
			outBranchOperation = rs.operation;
			outBranchTags = rs.tags;
			outBranchDispatched = true;
#if defined(DEBUG) || defined(TRACE)
			outBranchInstructionCounter = rs.dbgInstructionCounter;
			outBranchInstruction = rs.dbgInstruction;
#endif
			
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << name() << ": branch instruction (tag = " << (int) rs.tags.tag << ") ready to execute" << endl;
#endif
			/* Free the reservation station */
			branchRSQueue.ReleaseTail();
		}
		else
		{
			outBranchDispatched = false;
		}
	}
	else
	{
		outBranchDispatched = false;
	}
	
	if(!systemRegisterRSQueue.Empty())
	{
		bool execute = false;
		SystemRegisterRS& rs = systemRegisterRSQueue.GetTail();
		/* Get the oldest instruction */
		
		if(!inSystemRegisterBusy && rs.Valid())
		{
			if(rs.executionSerialized)
			{
				/* If the instruction is execution serialized then check if completion unit allows us to start the execution */
				if(inSystemRegisterExecute && inSystemRegisterExecuteTag == rs.tags.tag) execute = true;
				if(rs.operation.ident == ID_SC && !inLoadStoreEmpty && inDCacheBusy) execute = false;
			}
			else
			{
				execute = true;
			}
		}
		
		if(execute)
		{	
			/* Instruction is ready to execute */
			outSystemRegisterOperation = rs.operation;
			outSystemRegisterTags = rs.tags;
			outSystemRegisterDispatched = true;
			outSystemRegisterBranchCounter = rs.branchCounter;
#if defined(DEBUG) || defined(TRACE)
			outSystemRegisterInstructionCounter = rs.dbgInstructionCounter;
			outSystemRegisterProgramCounter = rs.dbgProgramCounter;
			outSystemRegisterInstruction = rs.dbgInstruction;
#endif
			
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << name() << ": system register instruction (tag = " << (int) rs.tags.tag << ") ready to execute" << endl;
#endif
			/* Free the reservation station */
			systemRegisterRSQueue.ReleaseTail();			
		}
		else
		{
			outSystemRegisterDispatched = false;
		}
	}
	else
	{
		outSystemRegisterDispatched = false;
	}
}

void DispatchUnit::UpdateRS()
{
	int i, j;
	int readRegisterPort = 0;
	int readFloatingPointRegisterPort = 0;
	int readRenamePort = 0;
	int readFloatingPointRenamePort = 0;
	int readRenameCRPort = 0;
	int readRenameLRPort = 0;
	int readRenameCTRPort = 0;
	
#ifdef DEBUG
	if(Debug(DebugDispatchUnit))
		cout << name() << ": Updating RS" << endl;
#endif
	/* Forward the results produced by the functional units to the reservation station waiting for them */

	for(i = 0; i < nIntegerUnit; i++)
	{
		if(inIntegerResultValid[i])
		{
			/* An integer unit produced a rename buffer */
			int j;
			
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << name() << ": integer unit produce rr" << (int) inIntegerResultTag[i] << endl;
#endif
			for(j = 0; j < nIntegerUnit; j++)
			{
				/* Make the result available for other integer instructions */
				integerRSQueue[j].UpdateIntegerRegister(inIntegerResultTag[i], inIntegerResultData[i]);
			}
				
			/* Make the result available to load/store instructions */
			loadStoreRSQueue.UpdateIntegerRegister(inIntegerResultTag[i], inIntegerResultData[i]);
		}
		if(inIntegerResultCRValid[i])
		{
			/* An integer unit produced a CR field rename buffer */
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << name() << ": Integer unit produce CR rename buffer #" << (int) inIntegerResultCRTag[i] << endl;
#endif
			/* Make the CR field value available to branch instructions */
			branchRSQueue.UpdateConditionRegister(inIntegerResultCRTag[i], inIntegerResultCRData[i]);
		}
	}
	
	for(i = 0; i < nFloatingPointUnits; i++)
	{
			if(inFloatingPointResultValid[i])
			{
				/* A floating point unit produced a floating point rename buffer */
				int j;	
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << name() << ": floating point unit produce rfp" << (int) inFloatingPointResultTag[i] << endl;
#endif
			for(j = 0; j < nFloatingPointUnits; j++)
			{
				/* Make the result available to other floating point instructions */
				floatingPointRSQueue[j].UpdateFloatingPointRegister(inFloatingPointResultTag[i], inFloatingPointResultData[i]);
			}
		
			/* Make the result available to load/store instructions */
			loadStoreRSQueue.UpdateFloatingPointRegister(inFloatingPointResultTag[i], inFloatingPointResultData[i]);
		}
		
		if(inFloatingPointResultCRValid[i])
		{
			/* A floating point unit produced a CR field rename buffer */
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << name() << ": floating point unit produce CR rename buffer #" << (int) inFloatingPointResultCRTag[i] << endl;
#endif
			/* Make the result available to branch instructions */
			branchRSQueue.UpdateConditionRegister(inFloatingPointResultCRTag[i], inFloatingPointResultCRData[i]);
		}
	}

	for(i = 0; i < 2; i++)
	{	
		if(inLoadStoreResultValid[i])
		{
			/* A load/store unit produced a rename buffer */
			int j;
			
#ifdef DEBUG
			if(Debug(DebugDispatchUnit))
				cout << name() << ": load/store unit produce rr" << (int) inLoadStoreResultTag[i] << endl;
#endif
			for(j = 0; j < nIntegerUnit; j++)
			{
				/* Make the result available to integer instructions */
				integerRSQueue[j].UpdateIntegerRegister(inLoadStoreResultTag[i], inLoadStoreResultData[i]);
			}
			
			/* Make the result available to other load/Store instructions */
			loadStoreRSQueue.UpdateIntegerRegister(inLoadStoreResultTag[i], inLoadStoreResultData[i]);
		}
	}
	
	if(inLoadStoreFloatingPointResultValid)
	{
		/* A load/store unit produced a floating point rename buffer */
		int j;
			
#ifdef DEBUG
		if(Debug(DebugDispatchUnit))
			cout << name() << ": load/store unit produce rfp" << (int) inLoadStoreResultTag[0] << endl;
#endif
		for(j = 0; j < nFloatingPointUnits; j++)
		{
			/* Make the result available to floating point instructions */
			floatingPointRSQueue[j].UpdateFloatingPointRegister(inLoadStoreResultTag[0], inLoadStoreFloatingPointResultData);
		}
		
		/* Make the result available to other load/store instructions */
		loadStoreRSQueue.UpdateFloatingPointRegister(inLoadStoreResultTag[0], inLoadStoreFloatingPointResultData);
	}
	
	if(inBranchResultLRValid)
	{
		/* Branch unit produced a LR rename buffer */
#ifdef DEBUG
		if(Debug(DebugDispatchUnit))
			cout << name() << ": Branch unit produce LR rename buffer #" << (int) inBranchResultLRTag << endl;
#endif
		/* Make the result available to other branch instructions */
		branchRSQueue.UpdateLinkRegister(inBranchResultLRTag, inBranchResultLRData);
	}
	
	if(inBranchResultCTRValid)
	{
		/* Branch unit produced a CTR rename buffer */
#ifdef DEBUG
		if(Debug(DebugDispatchUnit))
			cout << name() << ": Branch unit produce CTR rename buffer #" << (int) inBranchResultCTRTag << endl;
#endif
		/* Make the result available to other branch instructions */
		branchRSQueue.UpdateCountRegister(inBranchResultCTRTag, inBranchResultCTRData);
	}
	
	if(inSystemRegisterResultValid)
	{
		/* System register unit produced a rename buffer */
		int j;
		
#ifdef DEBUG
		if(Debug(DebugDispatchUnit))
			cout << name() << ": System Register unit produce rr" << (int) inSystemRegisterResultTag << endl;
#endif
		for(j = 0; j < nIntegerUnit; j++)
		{
			/* Make the result available to the integer instructions */
			integerRSQueue[j].UpdateIntegerRegister(inSystemRegisterResultTag, inSystemRegisterResultData);
		}
		/* Make the result available to the load/store instructions */
		loadStoreRSQueue.UpdateIntegerRegister(inSystemRegisterResultTag, inSystemRegisterResultData);
	}
	
	if(inSystemRegisterResultLRValid)
	{
		/* System Register unit produced a LR rename buffer */
#ifdef DEBUG
		if(Debug(DebugDispatchUnit))
			cout << name() << ": System Register unit produce LR rename buffer #" << (int) inSystemRegisterResultLRTag << endl;
#endif
		/* Make the result available to the branch instructions */
		branchRSQueue.UpdateLinkRegister(inSystemRegisterResultLRTag, inSystemRegisterResultLRData);
	}
	
	if(inSystemRegisterResultCTRValid)
	{
		/* System register unit produced a CTR rename buffer */
#ifdef DEBUG
		if(Debug(DebugDispatchUnit))
			cout << name() << ": System Register unit produce CTR rename buffer #" << (int) inSystemRegisterResultCTRTag << endl;
#endif
		/* Make the result available to the branch instructions */
		branchRSQueue.UpdateCountRegister(inSystemRegisterResultCTRTag, inSystemRegisterResultCTRData);
	}
	
	if(inSystemRegisterResultCRValid)
	{
		/* System register unit produced a CR field rename buffer */
#ifdef DEBUG
		if(Debug(DebugDispatchUnit))
			cout << name() << ": System Register unit produce CR rename buffer #" << (int) inSystemRegisterResultCRTag << endl;
#endif
		/* Make the result available to the branch instructions */
		branchRSQueue.UpdateConditionRegister(inSystemRegisterResultCRTag, inSystemRegisterResultCRData);
	}

	/* Now, store into the reservation station, the values asked to the register file by the decode process */
	for(i = 0; i < nInstructionsToDispatch; i++)
	{
		switch(units[i])
		{
			case IntegerUnitIdent:
				for(j = 0; j < 2; j++)
				{
					if(integerReadingRegisters[i][j] >= 0)
					{
						rs[i].integer->operands[j].data = inReadRegisterData[readRegisterPort];
#ifdef DEBUG
						if(Debug(DebugDispatchUnit))
						{
							cout << name() << ": Integer operands[" << j << "] <- ";
							WriteHex(cout,rs[i].integer->operands[j].data);
							cout << " (from register file)" << endl;
						}
#endif
						readRegisterPort++;
					}
					else
					{
						if(integerReadingRenameBuffers[i][j] >= 0)
						{
							rs[i].integer->operands[j].data = inReadRenameData[readRenamePort];
							rs[i].integer->operands[j].valid = true;
#ifdef DEBUG
							if(Debug(DebugDispatchUnit))
							{
								cout << name() << ": Integer operands[" << j << "] <- ";
								WriteHex(cout,rs[i].integer->operands[j].data);
								cout << " (from rename buffers)" << endl;
							}
#endif
							readRenamePort++;
						}
					}
				}
				break;
				
			case FloatingPointUnitIdent:
				for(j = 0; j < 3; j++)
				{
					if(floatingPointReadingFloatingPointRegisters[i][j] >= 0)
					{
						rs[i].floatingPoint->operands[j].data = inReadFloatingPointRegisterData[readFloatingPointRegisterPort];
#ifdef DEBUG
						if(Debug(DebugDispatchUnit))
						{
							cout << name() << ": Floating point operands[" << j << "] <- ";
							WriteHex(cout,rs[i].floatingPoint->operands[j].data);
							cout << " (from floating point register file)" << endl;
							cout << *rs[i].floatingPoint << endl;
						}
#endif
						readFloatingPointRegisterPort++;
					}
					else
					{
						if(floatingPointReadingFloatingPointRenameBuffers[i][j] >= 0)
						{
							rs[i].floatingPoint->operands[j].data = inReadFloatingPointRenameData[readFloatingPointRenamePort];
							rs[i].floatingPoint->operands[j].valid = true;
#ifdef DEBUG
							if(Debug(DebugDispatchUnit))
							{
								cout << name() << ": Floating point operands[" << j << "] <- ";
								WriteHex(cout,rs[i].floatingPoint->operands[j].data);
								cout << " (from floating point rename buffers)" << endl;
							}
#endif
							readFloatingPointRenamePort++;
						}
					}
				}
				break;
			
			case LoadStoreUnitIdent:
				// Operand 0
				if(loadStoreReadingFloatingPointRegister[i] >= 0)
				{
					rs[i].loadStore->operand0.data = inReadFloatingPointRegisterData[readFloatingPointRegisterPort];
#ifdef DEBUG
					if(Debug(DebugDispatchUnit))
					{
						cout << name() << ": Load/Store operand 0 <- ";
						WriteHex(cout,rs[i].loadStore->operand0.data);
						cout << " (from floating point register file)" << endl;
						cout << name() << ": " << *rs[i].loadStore << endl;
					}
#endif
					readFloatingPointRegisterPort++;
				}
				else
				{
					if(loadStoreReadingFloatingPointRenameBuffer[i] >= 0)
					{
						rs[i].loadStore->operand0.data = inReadFloatingPointRenameData[readFloatingPointRenamePort];
						rs[i].loadStore->operand0.valid = true;
#ifdef DEBUG
						if(Debug(DebugDispatchUnit))
						{
							cout << name() << ": Load/Store operand 0 <- ";
							WriteHex(cout,rs[i].loadStore->operand0.data);
							cout << " (from floating point rename buffers)" << endl;
						}
#endif
						readFloatingPointRenamePort++;						
					}
					else
					{
						if(loadStoreReadingRegisters[i][0] >= 0)
						{
							rs[i].loadStore->operand0.data = inReadRegisterData[readRegisterPort];
#ifdef DEBUG
							if(Debug(DebugDispatchUnit))
							{
								cout << name() << ": Load/Store operand 0 <- ";
								WriteHex(cout,rs[i].loadStore->operand0.data);
								cout << " (from register file)" << endl;
								cout << name() << ": " << *rs[i].loadStore << endl;
							}
#endif
							readRegisterPort++;
						}
						else
						{
							if(loadStoreReadingRenameBuffers[i][0] >= 0)
							{
								rs[i].loadStore->operand0.data = inReadRenameData[readRenamePort];
								rs[i].loadStore->operand0.valid = true;
#ifdef DEBUG
								if(Debug(DebugDispatchUnit))
								{
									cout << name() << ": Load/Store operand 0 <- ";
									WriteHex(cout,rs[i].loadStore->operand0.data);
									cout << " (from rename buffers)" << endl;
								}
#endif
								readRenamePort++;
							}
						}
					}
				}
			
				// Operand 1
				if(loadStoreReadingRegisters[i][1] >= 0)
				{
					rs[i].loadStore->operand1.data = inReadRegisterData[readRegisterPort];
#ifdef DEBUG
					if(Debug(DebugDispatchUnit))
					{
						cout << name() << ": Load/Store operand 1 <- ";
						WriteHex(cout,rs[i].loadStore->operand1.data);
						cout << " (from register file)" << endl;
						cout << name() << ": " << *rs[i].loadStore << endl;
					}
#endif
					readRegisterPort++;
				}
				else
				{
					if(loadStoreReadingRenameBuffers[i][1] >= 0)
					{
						rs[i].loadStore->operand1.data = inReadRenameData[readRenamePort];
						rs[i].loadStore->operand1.valid = true;
#ifdef DEBUG
						if(Debug(DebugDispatchUnit))
						{
							cout << name() << ": Load/Store operand 1 <- ";
							WriteHex(cout,rs[i].loadStore->operand1.data);
							cout << " (from rename buffers)" << endl;
						}
#endif
						readRenamePort++;
					}
				}

				// Operand 2				
				if(loadStoreReadingRegisters[i][2] >= 0)
				{
					rs[i].loadStore->operand2.data = inReadRegisterData[readRegisterPort];
#ifdef DEBUG
					if(Debug(DebugDispatchUnit))
					{
						cout << name() << ": Load/Store operand 2 <- ";
						WriteHex(cout,rs[i].loadStore->operand2.data);
						cout << " (from register file)" << endl;
						cout << name() << ": " << *rs[i].loadStore << endl;
					}
#endif
					readRegisterPort++;
				}
				else
				{
					if(loadStoreReadingRenameBuffers[i][2] >= 0)
					{
						rs[i].loadStore->operand2.data = inReadRenameData[readRenamePort];
						rs[i].loadStore->operand2.valid = true;
#ifdef DEBUG
						if(Debug(DebugDispatchUnit))
						{
							cout << name() << ": Load/Store operand 2 <- ";
							WriteHex(cout,rs[i].loadStore->operand2.data);
							cout << " (from rename buffers)" << endl;
						}
#endif
						readRenamePort++;
					}
				}
				break;
				
			case BranchUnitIdent:
				if(branchReadingCR[i] >= 0)
				{
					rs[i].branch->cr.crfield = (inCR >> (4 * (7 - decodedInstructions[i].io.branch.crf))) & 15;
#ifdef DEBUG
					if(Debug(DebugDispatchUnit))
					{
						cout << name() << ": Branch.CR" << (int) decodedInstructions[i].io.branch.crf << " <- "; WriteHex(cout, rs[i].branch->cr.crfield); cout << " from CR" << endl;
					}
#endif
				}
				else
				{
					if(branchReadingRenameCR[i] >= 0)
					{
						rs[i].branch->cr.crfield = inReadRenameCRData[readRenameCRPort];
						rs[i].branch->cr.valid = true;
#ifdef DEBUG
						if(Debug(DebugDispatchUnit))
						{
							cout << name() << ": Branch.CR" << (int) decodedInstructions[i].io.branch.crf << " <- "; WriteHex(cout, rs[i].branch->cr.crfield); cout << " from rename buffer" << endl;
						}
#endif
						readRenameCRPort++;
					}
				}

				if(branchReadingLR[i] >= 0)
				{
					rs[i].branch->lr.data = inLR;
				}
				else
				{
					if(branchReadingRenameLR[i] >= 0)
					{
						rs[i].branch->lr.data = inReadRenameLRData[readRenameLRPort];
						rs[i].branch->lr.valid = true;
					}
					readRenameLRPort++;
				}

				if(branchReadingCTR[i] >= 0)
				{
					rs[i].branch->ctr.data = inCTR;
#ifdef DEBUG
					if(Debug(DebugDispatchUnit))
					{
						cout << name() << ": Branch.CTR" << " <- "; WriteHex(cout, rs[i].branch->ctr.data); cout << " from CTR" << endl;
					}
#endif
				}
				else
				{				
					if(branchReadingRenameCTR[i] >= 0)
					{
						rs[i].branch->ctr.data = inReadRenameCTRData[readRenameCTRPort];
						rs[i].branch->ctr.valid = true;
#ifdef DEBUG
						if(Debug(DebugDispatchUnit))
						{
							cout << name() << ": Branch.CTR" << " <- "; WriteHex(cout, rs[i].branch->ctr.data); cout << " from rename buffer" << endl;
						}
#endif
						readRenameCTRPort++;
					}
				}
				break;
		}
	}
	
	/* Wake up finish process into completion unit */
	outFinish = !rob->Empty();
}

void DispatchUnit::FlushAll()
{
	int i;

	for(i = 0; i < nIntegerUnit; i++)
	{
		integerRSQueue[i].Flush();
	}
	
	for(i = 0; i < nFloatingPointUnits; i++)
	{
		floatingPointRSQueue[i].Flush();
	}
	
	loadStoreRSQueue.Flush();
	branchRSQueue.Flush();
	systemRegisterRSQueue.Flush();
}

void DispatchUnit::Flush()
{
	int i;

	for(i = 0; i < nIntegerUnit; i++)
	{
		integerRSQueue[i].Flush();
	}
	
	for(i = 0; i < nFloatingPointUnits; i++)
	{
		floatingPointRSQueue[i].Flush();
	}
	
	while(!loadStoreRSQueue.Empty())
	{
		LoadStoreRS& rs = loadStoreRSQueue.GetHead();
		if(rs.branchCounter == 0) break;
		loadStoreRSQueue.ReleaseHead();
	}
	
	while(!branchRSQueue.Empty())
	{
		BranchRS& rs = branchRSQueue.GetHead();
		if(rs.branchCounter == 0) break;
		branchRSQueue.ReleaseHead();
	}	

	while(!systemRegisterRSQueue.Empty())
	{
		SystemRegisterRS& rs = systemRegisterRSQueue.GetHead();
		if(rs.branchCounter == 0) break;
		if(rs.interlock)
		{
			interlock = false;
		}
		systemRegisterRSQueue.ReleaseHead();
	}	
}

void DispatchUnit::DecrementBranchCounter()
{
	int i;
	
	if(branchCounter > 0) branchCounter--;
	
	for(i = 0; i < nIntegerUnit; i++)
	{
		integerRSQueue[i].DecrementBranchCounter();
	}
	
	for(i = 0; i < nFloatingPointUnits; i++)
	{
		floatingPointRSQueue[i].DecrementBranchCounter();
	}
	
	loadStoreRSQueue.DecrementBranchCounter();
	branchRSQueue.DecrementBranchCounter();
	systemRegisterRSQueue.DecrementBranchCounter();
}

void DispatchUnit::Reset()
{
	nDecodedInstructions = 0;
	nInstructionsToDispatch = 0;
	branchCounter = 0;
	interlock = false;
	branchFollow = false;
	FlushAll();
}
