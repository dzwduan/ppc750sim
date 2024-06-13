/***************************************************************************
                          CompletionUnit.cpp  -  unite de completion
                             -------------------
    begin                : Fri Apr 6 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <CompletionUnit.h>

CompletionUnit::CompletionUnit(const sc_module_name& name, ReorderBuffer *rob, RegistersBinding *registersBinding)
{
	SC_HAS_PROCESS(CompletionUnit);

	SC_METHOD(Retire);
	sensitive_pos << inClock;
		
	SC_METHOD(OnFallingEdge);
	sensitive_neg << inClock;
	
	SC_METHOD(Finish);
	sensitive << inFinish;

	this->rob = rob;
	this->registersBinding = registersBinding;
	
	Reset();		
}


/* This method computes the number of instructions that can be commited by checking */
int CompletionUnit::GetInstructionsToWriteBack()
{
	int i, n;
	int writeBackPort = 0;
	int writeBackFloatingPointPort = 0;
	int writeBackCRPort = 0;
	int writeBackLRPort = 0;
	int writeBackCTRPort = 0;
	int memoryWriteBackPort = 0;

	n = rob->Size();
	if(n > nCQ) n = nCQ;
	
	for(i = 0; i < n; i++)
	{
		ReorderBufferEntry& robEntry = (*rob)[i];
		
		if(!robEntry.finished || robEntry.branchCounter > 0) return i;
		
		switch(robEntry.unitIdent)
		{
			case IntegerUnitIdent:
				if(robEntry.extra.integer.tags.resultTag >= 0)
				{
					if(writeBackPort >= nWriteBackPorts) return i;
					writeBackPort++;
				}
				
				if(robEntry.extra.integer.tags.CRTag >= 0)
				{
					if(writeBackCRPort >= nWriteBackCRPorts) return i;
					writeBackCRPort++;
				}
				break;
				
			case FloatingPointUnitIdent:
				if(robEntry.extra.floatingPoint.tags.resultTag >= 0)
				{
					if(writeBackFloatingPointPort >= nWriteBackFloatingPointPorts) return i;
					writeBackFloatingPointPort++;
				}
				
				if(robEntry.extra.floatingPoint.tags.CRTag >= 0)
				{
					if(writeBackCRPort >= nWriteBackCRPorts) return i;
					writeBackCRPort++;
				}
				break;
				
			case LoadStoreUnitIdent:
				if(robEntry.extra.loadStore.write)
				{
					if(memoryWriteBackPort >= nMemoryWriteBackPorts) return i;
					memoryWriteBackPort++;
				}
			
				if(robEntry.extra.loadStore.tags.resultTags[0] >= 0)
				{
					if(robEntry.extra.loadStore.floatingPoint)
					{
						if(writeBackFloatingPointPort >= nWriteBackFloatingPointPorts) return i;
						writeBackFloatingPointPort++;
					}
					else
					{
						if(writeBackPort >= nWriteBackPorts) return i;
						writeBackPort++;
					}
				}
				
				if(robEntry.extra.loadStore.tags.resultTags[1] >= 0)
				{
					if(writeBackPort >= nWriteBackPorts) return i;
					writeBackPort++;
				}
				break;
				
			case BranchUnitIdent:
				if(robEntry.extra.branch.tags.LRTag >= 0)
				{
					if(writeBackLRPort >= nWriteBackLRPorts) return i;
					writeBackLRPort++;
				}
				
				if(robEntry.extra.branch.tags.CTRTag >= 0)
				{
					if(writeBackCTRPort >= nWriteBackCTRPorts) return i;
					writeBackCTRPort++;
				}
				break;
				
			case SystemRegisterUnitIdent:
				if(robEntry.extra.systemRegister.tags.resultTag >= 0)
				{
					if(writeBackPort >= nWriteBackPorts) return i;
					writeBackPort++;
				}
				
				if(robEntry.extra.systemRegister.tags.LRTag >= 0)
				{
					if(writeBackLRPort >= nWriteBackLRPorts) return i;
					writeBackLRPort++;
				}
				
				if(robEntry.extra.systemRegister.tags.CTRTag >= 0)
				{
					if(writeBackCTRPort >= nWriteBackCTRPorts) return i;
					writeBackCTRPort++;
				}
				
				if(robEntry.extra.systemRegister.tags.CRTag >= 0)
				{
					if(writeBackCRPort >= nWriteBackCRPorts) return i;
					writeBackCRPort++;
				}
				break;
		}
	}
	return i;	
}

void CompletionUnit::Retire()
{
	int i, j;

	/* Write Back */	
	int writeBackPort = 0;
	int writeBackFloatingPointPort = 0;
	int writeBackCRPort = 0;
	int writeBackLRPort = 0;
	int writeBackCTRPort = 0;
	int memoryWriteBackPort = 0;
	bool writeCarry = false;
	bool writeOverflow = false;
	bool writeCR = false;
	/* Get the number of instructions that can be written back */
	int nInstructionsToWriteBack = GetInstructionsToWriteBack();
	
#ifdef DEBUG
	if(Debug(DebugCompletionUnit))
	{
		if(nInstructionsToWriteBack > 0)
			cout << name() << ": Write Back " << nInstructionsToWriteBack << " instructions" << endl;
		
		cout << name() << ": Registers binding :" << endl;
		cout << *registersBinding << endl;
	}
#endif

	/* Write Back */
	
	for(i = 0; i < nInstructionsToWriteBack; i++)
	{
		/* Get the instructions from the oldest to the newest */
		ReorderBufferEntry& robEntry = (*rob)[i];
		
		switch(robEntry.unitIdent)
		{
			case IntegerUnitIdent:
#ifdef DEBUG
				if(Debug(DebugCompletionUnit))
					cout << name() << ": Write Back an integer instruction (tag = " << (int) robEntry.extra.integer.tags.tag << ")" << endl;
#endif
				if(robEntry.extra.integer.tags.resultTag >= 0)
				{
					/* Instruction has a rename buffer to transfer to a register */
					/* Get the register number in which the rename buffer value has to be written */
					int dstReg = registersBinding->GetRegister(robEntry.extra.integer.tags.resultTag);
#ifdef DEBUG
					if(Debug(DebugCompletionUnit))
						cout << name() << ": Write Back rr" << (int) robEntry.extra.integer.tags.resultTag << " to r" << dstReg << endl;
#endif
					/* Ask to the register file to do the data transfer between the rename buffer and the register */
					outWriteBack[writeBackPort] = true;
					outWriteBackRegisterNumber[writeBackPort] = dstReg;
					outWriteBackRenameNumber[writeBackPort] = robEntry.extra.integer.tags.resultTag;
					writeBackPort++;
				}
				
				if(robEntry.extra.integer.tags.CRTag >= 0)
				{
					/* Instruction has a CR field rename buffer to transfer into CR */
#ifdef DEBUG
					if(Debug(DebugCompletionUnit))
						cout << name() << ": Write Back CR rename buffer #" << (int) robEntry.extra.integer.tags.CRTag << endl;
#endif
					outWriteBackCR[writeBackCRPort] = true;
					outWriteBackCRRenameNumber[writeBackCRPort] = robEntry.extra.integer.tags.CRTag;
					outWriteBackCRFieldNumber[writeBackCRPort] = registersBinding->GetField(robEntry.extra.integer.tags.CRTag);
					writeBackCRPort++;
				}
				if(robEntry.extra.integer.writeCarry)
				{
					/* Instruction modifies the carry bit XER[CA] */
#ifdef DEBUG
					if(Debug(DebugCompletionUnit))
						cout << name() << ": Write Back carry (" << robEntry.extra.integer.carry << ")" << endl;
#endif
					writeCarry = true;
					outCarry = robEntry.extra.integer.carry;
				}
				
				if(robEntry.extra.integer.writeOverflow)
				{
					/* Instruction modifies the overflow bit XER[SO] */
#ifdef DEBUG
					if(Debug(DebugCompletionUnit))
						cout << name() << ": Write Back overflow" << endl;
#endif
					writeOverflow = true;
					outOverflow = robEntry.extra.integer.overflow;
				}
				break;

			case FloatingPointUnitIdent:
#ifdef DEBUG
				if(Debug(DebugCompletionUnit))
					cout << name() << ": Write Back a floating point instruction (tag = " << (int) robEntry.extra.floatingPoint.tags.tag << ")" << endl;
#endif
				if(robEntry.extra.floatingPoint.tags.resultTag >= 0)
				{
					/* Instruction has to transfer the floating point rename buffer to a floating point register */
					/* Get the floating point register number where the floating point rename buffer value must be written */
					int dstReg = registersBinding->GetFloatingPointRegister(robEntry.extra.floatingPoint.tags.resultTag);
#ifdef DEBUG
					if(Debug(DebugCompletionUnit))
						cout << name() << ": Write Back rfp" << (int) robEntry.extra.floatingPoint.tags.resultTag << " to fp" << dstReg << endl;
#endif
					/* Ask to the register file to do the transfer between the floating point rename buffer ans the floating point register */
					outWriteBackFloatingPoint[writeBackFloatingPointPort] = true;
					outWriteBackFloatingPointRegisterNumber[writeBackFloatingPointPort] = dstReg;
					outWriteBackFloatingPointRenameNumber[writeBackFloatingPointPort] = robEntry.extra.floatingPoint.tags.resultTag;
					writeBackFloatingPointPort++;
				}
				
				if(robEntry.extra.floatingPoint.tags.CRTag >= 0)
				{
					/* Instruction has a CR field to write back */
#ifdef DEBUG
					if(Debug(DebugCompletionUnit))
						cout << name() << ": Write Back CR rename buffer #" << (int) robEntry.extra.floatingPoint.tags.CRTag << endl;
#endif
					outWriteBackCR[writeBackCRPort] = true;
					outWriteBackCRRenameNumber[writeBackCRPort] = robEntry.extra.floatingPoint.tags.CRTag;
					outWriteBackCRFieldNumber[writeBackCRPort] = registersBinding->GetField(robEntry.extra.floatingPoint.tags.CRTag);
					writeBackCRPort++;
				}
				break;
				
			case LoadStoreUnitIdent:
#ifdef DEBUG
				if(Debug(DebugCompletionUnit))
					cout << name() << ": Write Back a load/store instruction (tag = " << (int) robEntry.extra.loadStore.tags.tag << ")" << endl;
#endif
				if(robEntry.extra.loadStore.write)
				{
					/* Store can go to the data cache */
					outMemoryWriteBack[memoryWriteBackPort] = true;
					outMemoryWriteBackTag[memoryWriteBackPort] = robEntry.extra.loadStore.tags.tag;
					memoryWriteBackPort++;
				}
				
				if(robEntry.extra.loadStore.tags.resultTags[0] >= 0)
				{
					if(robEntry.extra.loadStore.floatingPoint)
					{
						/* Instruction has a floating point result to write back */
					
						regnum_t dstReg = registersBinding->GetFloatingPointRegister(robEntry.extra.loadStore.tags.resultTags[0]);
#ifdef DEBUG
						if(Debug(DebugCompletionUnit))
							cout << name() << ": Write Back rfp" << (int) robEntry.extra.loadStore.tags.resultTags[0] << " to fp" << (int) dstReg << endl;
#endif
						outWriteBackFloatingPoint[writeBackFloatingPointPort] = true;
						outWriteBackFloatingPointRegisterNumber[writeBackFloatingPointPort] = dstReg;
						outWriteBackFloatingPointRenameNumber[writeBackFloatingPointPort] = robEntry.extra.loadStore.tags.resultTags[0];
						writeBackFloatingPointPort++;
					}
					else
					{
						/* Instruction has a result to write back */
						
						regnum_t dstReg = registersBinding->GetRegister(robEntry.extra.loadStore.tags.resultTags[0]);
#ifdef DEBUG
						if(Debug(DebugCompletionUnit))
							cout << name() << ": Write Back rr" << (int) robEntry.extra.loadStore.tags.resultTags[0] << " to r" << (int) dstReg << endl;
#endif
						outWriteBack[writeBackPort] = true;
						outWriteBackRegisterNumber[writeBackPort] = dstReg;
						outWriteBackRenameNumber[writeBackPort] = robEntry.extra.loadStore.tags.resultTags[0];
						writeBackPort++;
					}
				}
				
				if(robEntry.extra.loadStore.tags.resultTags[1] >= 0)
				{
					/* Instruction has a result to write back */
					regnum_t dstReg = registersBinding->GetRegister(robEntry.extra.loadStore.tags.resultTags[1]);
#ifdef DEBUG
					if(Debug(DebugCompletionUnit))
						cout << name() << ": Write Back rr" << (int) robEntry.extra.loadStore.tags.resultTags[1] << " to r" << (int) dstReg << endl;
#endif
					outWriteBack[writeBackPort] = true;
					outWriteBackRegisterNumber[writeBackPort] = dstReg;
					outWriteBackRenameNumber[writeBackPort] = robEntry.extra.loadStore.tags.resultTags[1];
					writeBackPort++;
				}
				break;
				
			case BranchUnitIdent:
#ifdef DEBUG
				if(Debug(DebugCompletionUnit))
					cout << name() << ": Write Back a branch instruction (tag = " << (int) robEntry.extra.branch.tags.tag << ")" << endl;
#endif
				if(robEntry.extra.branch.tags.LRTag >= 0)
				{
					/* Instruction has to write LR */
#ifdef DEBUG
					if(Debug(DebugCompletionUnit))
						cout << name() << ": Write Back LR rename buffer #" << (int) robEntry.extra.branch.tags.LRTag << endl;
#endif
					outWriteBackLR[writeBackLRPort] = true;
					outWriteBackLRRenameNumber[writeBackLRPort] = robEntry.extra.branch.tags.LRTag;
					writeBackLRPort++;
				}
		
				if(robEntry.extra.branch.tags.CTRTag >= 0)
				{
					/* Instruction has to write CTR */
#ifdef DEBUG
					if(Debug(DebugCompletionUnit))
						cout << name() << ": Write Back CTR rename buffer #" << (int) robEntry.extra.branch.tags.CTRTag << endl;
#endif
					outWriteBackCTR[writeBackCTRPort] = true;
					outWriteBackCTRRenameNumber[writeBackCTRPort] = robEntry.extra.branch.tags.CTRTag;
					writeBackCTRPort++;
				}
				break;
			
			case SystemRegisterUnitIdent:
#ifdef DEBUG
				if(Debug(DebugCompletionUnit))
					cout << name() << ": Write Back a system register instruction (tag = " << (int) robEntry.extra.systemRegister.tags.tag << ")" << endl;
#endif
				if(robEntry.extra.systemRegister.tags.resultTag >= 0)
				{
					/* Instruction has to write a result */
					int dstReg = registersBinding->GetRegister(robEntry.extra.systemRegister.tags.resultTag);;
#ifdef DEBUG
					if(Debug(DebugCompletionUnit))
						cout << name() << ": Write Back rr" << (int) robEntry.extra.systemRegister.tags.resultTag << " to r" << dstReg << endl;
#endif
					outWriteBack[writeBackPort] = true;
					outWriteBackRegisterNumber[writeBackPort] = dstReg;
					outWriteBackRenameNumber[writeBackPort] = robEntry.extra.systemRegister.tags.resultTag;
					writeBackPort++;
				}
				
				if(robEntry.extra.systemRegister.tags.LRTag >= 0)
				{
					/* Instruction has to write into LR */
#ifdef DEBUG
					if(Debug(DebugCompletionUnit))
						cout << name() << ": Write Back LR rename buffer #" << (int) robEntry.extra.systemRegister.tags.LRTag << endl;
#endif
					outWriteBackLR[writeBackLRPort] = true;
					outWriteBackLRRenameNumber[writeBackLRPort] = robEntry.extra.systemRegister.tags.LRTag;
					writeBackLRPort++;
				}
				
				if(robEntry.extra.systemRegister.tags.CTRTag >= 0)
				{
					/* Instruction has to write into CTR */
#ifdef DEBUG
					if(Debug(DebugCompletionUnit))
						cout << name() << ": Write Back CTR rename buffer #" << (int) robEntry.extra.systemRegister.tags.CTRTag << endl;
#endif
					outWriteBackCTR[writeBackCTRPort] = true;
					outWriteBackCTRRenameNumber[writeBackCTRPort] = robEntry.extra.systemRegister.tags.CTRTag;
					writeBackCTRPort++;
				}
				
				if(robEntry.extra.systemRegister.tags.CRTag >= 0)
				{
					/* Instruction has to write a CR field */
#ifdef DEBUG
					if(Debug(DebugCompletionUnit))
						cout << name() << ": Write Back CR rename buffer #" << (int) robEntry.extra.systemRegister.tags.CRTag << endl;
#endif
					outWriteBackCR[writeBackCRPort] = true;
					outWriteBackCRRenameNumber[writeBackCRPort] = robEntry.extra.systemRegister.tags.CRTag;
					outWriteBackCRFieldNumber[writeBackCRPort] = registersBinding->GetField(robEntry.extra.systemRegister.tags.CRTag);
					writeBackCRPort++;
				}
				
				if(robEntry.extra.systemRegister.writeCR)
				{
					/* Instruction has to write into CR (with a binary mask) */
					writeCR = true;
					outCRM = robEntry.extra.systemRegister.crm;
					outCR = robEntry.extra.systemRegister.cr;
				}
				break;
		}
	}
	
	outWriteCR = writeCR;
	
#ifdef DEBUG
	if(Debug(DebugCompletionUnit))
		cout << name() << ": " << writeBackPort << " registers written back" << endl;
#endif
	
	/* Don't write on unused write back ports */
	for(; writeBackPort < nWriteBackPorts; writeBackPort++)
		outWriteBack[writeBackPort] = false;
	
	for(; writeBackFloatingPointPort < nWriteBackFloatingPointPorts; writeBackFloatingPointPort++)
		outWriteBackFloatingPoint[writeBackFloatingPointPort] = false;
	
	for(; writeBackCRPort < nWriteBackCRPorts; writeBackCRPort++)
		outWriteBackCR[writeBackCRPort] = false;
		
	for(; writeBackLRPort < nWriteBackLRPorts; writeBackLRPort++)
		outWriteBackLR[writeBackLRPort] = false;
		
	for(; writeBackCTRPort < nWriteBackCTRPorts; writeBackCTRPort++)
		outWriteBackCTR[writeBackCTRPort] = false;
		
	for(; memoryWriteBackPort < nMemoryWriteBackPorts; memoryWriteBackPort++)
		outMemoryWriteBack[memoryWriteBackPort] = false;
		
	outWriteCarry = writeCarry;
	outWriteOverflow = writeOverflow;

	nInstructionsToRetire = nInstructionsToWriteBack;		
	
	/* Retire */

#ifdef DEBUG
	if(Debug(DebugCompletionUnit))
		if(nInstructionsToRetire > 0)
			cout << name() << ": Retiring " << nInstructionsToRetire << " instructions" << endl;
		
#endif
	nRetiredInstructions += nInstructionsToRetire;
	
	bool clearInterlock = false;
		
	for(i = 0; i < nInstructionsToRetire; i++)
	{
		/* Get a reorder buffer entry (from the oldest to the newest) */
		ReorderBufferEntry& robEntry = rob->GetTail();
		
		programCounter = robEntry.nia;
		
		switch(robEntry.unitIdent)
		{
			case IntegerUnitIdent:
#ifdef DEBUG
				if(Debug(DebugCompletionUnit))
				{
					cout << name() << ": Retiring instruction (tag = " << (int) robEntry.extra.integer.tags.tag << ")  (" << robEntry.dbgInstructionCounter << ")  ";
					ppc_disassemble_to_stream(robEntry.dbgInstruction, robEntry.dbgProgramCounter, stdout);
					cout << endl;
				}
#endif
#ifdef TRACE
				if(Trace(TraceCompletionUnit))
				{
					trace_file->Begin("retire");
					trace_file->Value("number", robEntry.dbgInstructionCounter);
					trace_file->Value("pc", robEntry.dbgProgramCounter);
					trace_file->End("retire");
				}
#endif
#ifdef EMULATE
				/* Emulator code begins */
				ppc_execute_check_path(robEntry.inst, robEntry.instPC);
				/* Emulator code ends */
#endif

				/* Free the rename buffers */
				if(robEntry.extra.integer.tags.resultTag >= 0)
					registersBinding->FreeRenameBuffer(robEntry.extra.integer.tags.resultTag);
					
				if(robEntry.extra.integer.tags.CRTag >= 0)
					registersBinding->FreeCRRenameBuffer(robEntry.extra.integer.tags.CRTag);
					
				break;
					
			case FloatingPointUnitIdent:
#ifdef DEBUG
				if(Debug(DebugCompletionUnit))
				{
					cout << name() << ": Retiring instruction (tag = " << (int) robEntry.extra.floatingPoint.tags.tag << ")  (" << robEntry.dbgInstructionCounter << ")  ";
					ppc_disassemble_to_stream(robEntry.dbgInstruction, robEntry.dbgProgramCounter, stdout);
					cout << endl;
				}
#endif
#ifdef EMULATE
				/* Emulator code begins */
				ppc_execute_check_path(robEntry.inst, robEntry.instPC);
				/* Emulator code ends */
#endif

				/* Free the rename buffers */
				if(robEntry.extra.floatingPoint.tags.resultTag >= 0)
					registersBinding->FreeFloatingPointRenameBuffer(robEntry.extra.floatingPoint.tags.resultTag);
					
				if(robEntry.extra.floatingPoint.tags.CRTag >= 0)
					registersBinding->FreeCRRenameBuffer(robEntry.extra.floatingPoint.tags.CRTag);
					
				break;
				
			case LoadStoreUnitIdent:
#ifdef DEBUG
					if(Debug(DebugCompletionUnit))
					{
						cout << name() << ": Retiring instruction (tag = " << (int) robEntry.extra.loadStore.tags.tag << ")  (" << robEntry.dbgInstructionCounter << ")  ";
						ppc_disassemble_to_stream(robEntry.dbgInstruction, robEntry.dbgProgramCounter, stdout);
						cout << endl;
					}
#endif
#ifdef EMULATE
					/* Emulator code begins */
					ppc_execute_check_path(robEntry.inst, robEntry.instPC);
					/* Emulator code ends */
#endif
					/* Free the rename buffers */
					if(robEntry.extra.loadStore.tags.resultTags[0] >= 0)
					{
						if(robEntry.extra.loadStore.floatingPoint)
						{
							registersBinding->FreeFloatingPointRenameBuffer(robEntry.extra.loadStore.tags.resultTags[0]);
						}
						else
						{
							registersBinding->FreeRenameBuffer(robEntry.extra.loadStore.tags.resultTags[0]);
						}
					}
					
					if(robEntry.extra.loadStore.tags.resultTags[1] >= 0)
						registersBinding->FreeRenameBuffer(robEntry.extra.loadStore.tags.resultTags[1]);
							
					break;
					
				case BranchUnitIdent:
#ifdef DEBUG
					if(Debug(DebugCompletionUnit))
					{
						cout << name() << ": Retiring instruction (tag = " << (int) robEntry.extra.branch.tags.tag << ")  (" << robEntry.dbgInstructionCounter << ")  ";
						ppc_disassemble_to_stream(robEntry.dbgInstruction, robEntry.dbgProgramCounter, stdout);
						cout << endl;
					}
#endif
#ifdef EMULATE
					/* Emulator code begins */
					ppc_execute_check_path(robEntry.inst, robEntry.instPC);
					/* Emulator code ends */
#endif
					/* Free the rename buffers */
					if(robEntry.extra.branch.tags.LRTag >= 0)
						registersBinding->FreeLRRenameBuffer(robEntry.extra.branch.tags.LRTag);
						
					if(robEntry.extra.branch.tags.CTRTag >= 0)
						registersBinding->FreeCTRRenameBuffer(robEntry.extra.branch.tags.CTRTag);

																		
					break;
					
				case SystemRegisterUnitIdent:
#ifdef DEBUG
					if(Debug(DebugCompletionUnit))
					{
						cout << name() << ": Retiring instruction (tag = " << (int) robEntry.extra.systemRegister.tags.tag << ")  (" << robEntry.dbgInstructionCounter << ")  ";
						ppc_disassemble_to_stream(robEntry.dbgInstruction, robEntry.dbgProgramCounter, stdout);
						cout << endl;
					}
#endif
#ifdef EMULATE
					/* Emulator code begins */
					ppc_execute_check_path(robEntry.inst, robEntry.instPC);
					/* Emulator code ends */
#endif
					
					/* Free the rename buffers */
					if(robEntry.extra.systemRegister.tags.resultTag >= 0)
						registersBinding->FreeRenameBuffer(robEntry.extra.systemRegister.tags.resultTag);
						
					if(robEntry.extra.systemRegister.tags.LRTag >= 0)
						registersBinding->FreeLRRenameBuffer(robEntry.extra.systemRegister.tags.LRTag);
						
					if(robEntry.extra.systemRegister.tags.CTRTag >= 0)
						registersBinding->FreeCTRRenameBuffer(robEntry.extra.systemRegister.tags.CTRTag);
					
					if(robEntry.extra.systemRegister.tags.CRTag >= 0)
						registersBinding->FreeCRRenameBuffer(robEntry.extra.systemRegister.tags.CRTag);
						
					if(robEntry.extra.systemRegister.interlock)
					{
						/* Clear the interlock bit */
						clearInterlock = true;
					}
					break;
		}
		rob->ReleaseTail();
	}
	
	outClearInterlock = clearInterlock;
	
	bool integerExecute[nIntegerUnit];
	bool floatingPointExecute[nFloatingPointUnits];
	bool systemRegisterExecute = false;

	/* Allow an execution serialized instruction to execute if it is the oldest instruction */
	for(i = 0; i < nIntegerUnit; i++)
		integerExecute[i] = false;
	
	for(i = 0; i < nFloatingPointUnits; i++)
		floatingPointExecute[i] = false;
	
	if(!rob->Empty())
	{
	
		ReorderBufferEntry& robEntry = rob->GetTail();
		if(!robEntry.finished && robEntry.executionSerialized)
		{
			/* Instruction that is execution serialized is the oldest instruction into the reorder buffer */
			switch(robEntry.unitIdent)
			{
				case IntegerUnitIdent:
#ifdef DEBUG
					if(Debug(DebugCompletionUnit))
						cout << name() << ": integer execution serialized instruction is ready to be executed (tag =" << (int) rob->GetIndex(robEntry) << ")" << endl;
#endif
					integerExecute[robEntry.extra.integer.unitNumber] = true;
					outIntegerExecuteTag[robEntry.extra.integer.unitNumber] = robEntry.extra.integer.tags.tag;
					break;
					
				case FloatingPointUnitIdent:
#ifdef DEBUG
					if(Debug(DebugCompletionUnit))
						cout << name() << ": floating point execution serialized instruction is ready to be executed (tag =" << (int) rob->GetIndex(robEntry) << ")" << endl;
#endif
					floatingPointExecute[robEntry.extra.floatingPoint.unitNumber] = true;
					outFloatingPointExecuteTag[robEntry.extra.floatingPoint.unitNumber] = robEntry.extra.floatingPoint.tags.tag;
					break;				
					
				case SystemRegisterUnitIdent:
#ifdef DEBUG
					if(Debug(DebugCompletionUnit))
						cout << name() << ": system register execution serialized instruction is ready to be executed (tag =" << (int) rob->GetIndex(robEntry) << ")" << endl;
#endif
					systemRegisterExecute = true;
					outSystemRegisterExecuteTag = robEntry.extra.systemRegister.tags.tag;					
					break;
			}
		}
	}
	
	for(i = 0; i < nIntegerUnit; i++)
		outIntegerExecute[i] = integerExecute[i];
		
	for(i = 0; i < nFloatingPointUnits; i++)
		outFloatingPointExecute[i] = floatingPointExecute[i];
		
	outSystemRegisterExecute = systemRegisterExecute;
	
	/* Wake up decode process of the dispatch unit */
	outDecode = rob->GetFreeSpace() > 0;	
}

void CompletionUnit::Finish()
{
	int i;
	
	/* Validate the rename buffers */

	for(i = 0; i < nIntegerUnit; i++)
	{
		if(inIntegerResultValid[i])
		{
			/* An integer unit produced a rename buffer */
#ifdef DEBUG
			if(Debug(DebugCompletionUnit))
				cout << name() << ": integer unit " << i << " produce rename buffer #" << (int) inIntegerResultTag[i] << endl;
#endif
			/* Validate the rename buffer  */
			registersBinding->ValidateRenameBuffer(inIntegerResultTag[i]);
		}
		
		if(inIntegerResultCRValid[i])
		{
			/* An integer unit produced a CR field rename buffer */
#ifdef DEBUG
			if(Debug(DebugCompletionUnit))
				cout << name() << ": integer unit " << i << " produce cr rename buffer #" << (int) inIntegerResultCRTag[i] << endl;
#endif
			/* Validate the CR field rename buffer */
			registersBinding->ValidateCRRenameBuffer(inIntegerResultCRTag[i]);
		}
	}
	
	for(i = 0; i < nFloatingPointUnits; i++)
	{
		if(inFloatingPointResultValid[i])
		{
			/* A floating point unit produced a floating point rename buffer */
#ifdef DEBUG
			if(Debug(DebugCompletionUnit))
				cout << name() << ": floating point unit " << i << " produce floating point rename buffer #" << (int) inFloatingPointResultTag[i] << endl;
#endif
			/* Validate the floating point rename buffer */
			registersBinding->ValidateFloatingPointRenameBuffer(inFloatingPointResultTag[i]);
		}
		
		if(inFloatingPointResultCRValid[i])
		{
			/* A floating point unit produced a CR field rename buffer */
#ifdef DEBUG
			if(Debug(DebugCompletionUnit))
				cout << name() << ": floating point unit " << i << " produce cr rename buffer #" << (int) inFloatingPointResultCRTag[i] << endl;
#endif
			/* Validate the CR field rename buffer */
			registersBinding->ValidateCRRenameBuffer(inFloatingPointResultCRTag[i]);
		}
	}
	
	if(inLoadStoreFloatingPointResultValid)
	{
		/* Load/store unit produced a floating point rename buffer */
#ifdef DEBUG
		if(Debug(DebugCompletionUnit))
			cout << name() << ": load/Store unit produce floating point rename buffer #" << (int) inLoadStoreResultTag[0] << endl;
#endif
		/* Validate the floating point rename buffer */
		registersBinding->ValidateFloatingPointRenameBuffer(inLoadStoreResultTag[0]);
	}
	else if(inLoadStoreResultValid[0])
	{
		/* Load/store unit produced a rename buffer */
#ifdef DEBUG
		if(Debug(DebugCompletionUnit))
			cout << name() << ": load/Store unit produce rename buffer #" << (int) inLoadStoreResultTag[0] << endl;
#endif
		/* validate the rename buffer */
		registersBinding->ValidateRenameBuffer(inLoadStoreResultTag[0]);
	}
		
	if(inLoadStoreResultValid[1])
	{
		/* Load/store unit produced a rename buffer */
#ifdef DEBUG
		if(Debug(DebugCompletionUnit))
			cout << name() << ": load/Store unit produce rename buffer #" << (int) inLoadStoreResultTag[1] << endl;
#endif
		/* validate the rename buffer */
		registersBinding->ValidateRenameBuffer(inLoadStoreResultTag[1]);
	}
	
	if(inBranchResultLRValid)
	{
		/* Branch unit produced a LR rename buffer */
#ifdef DEBUG
		if(Debug(DebugCompletionUnit))
			cout << name() << ": branch unit produce rename LR buffer #" << (int) inBranchResultLRTag << endl;
#endif
		/* Validate the LR rename buffer */
		registersBinding->ValidateLRRenameBuffer(inBranchResultLRTag);
	}
		
	if(inBranchResultCTRValid)
	{
		/* branch unit produced a CTR rename buffer */
#ifdef DEBUG
		if(Debug(DebugCompletionUnit))
			cout << name() << ": branch unit produce rename buffer #" << (int) inBranchResultCTRTag << endl;
#endif
		/* valid the CTR rename buffer */
		registersBinding->ValidateCTRRenameBuffer(inBranchResultCTRTag);
	}
	
	if(inSystemRegisterResultValid)
	{
		/* System Register Unit produced a rename buffer */
#ifdef DEBUG
		if(Debug(DebugCompletionUnit))
			cout << name() << ": system register unit produce rename buffer #" << (int) inSystemRegisterResultTag << endl;
#endif
		/* Validate the rename buffer */
		registersBinding->ValidateRenameBuffer(inSystemRegisterResultTag);
	}
		
	if(inSystemRegisterResultLRValid)
	{
		/* System Register Unit produced a LR rename buffer */
#ifdef DEBUG
		if(Debug(DebugCompletionUnit))
			cout << name() << ": system register unit produce LR rename buffer #" << (int) inSystemRegisterResultLRTag << endl;
#endif
		/* Validate the LR rename buffer */
		registersBinding->ValidateLRRenameBuffer(inSystemRegisterResultLRTag);
	}
	
	if(inSystemRegisterResultCTRValid)
	{
		/* System register Unit produced a CTR rename buffer */
#ifdef DEBUG
		if(Debug(DebugCompletionUnit))
			cout << name() << ": system register unit produce CTR rename buffer #" << (int) inSystemRegisterResultCTRTag << endl;
#endif
		/* Validate the CTR rename buffer */
		registersBinding->ValidateCTRRenameBuffer(inSystemRegisterResultCTRTag);
	}
	
	if(inSystemRegisterResultCRValid)
	{
		/* System Register Unit produced a CR field rename buffer */
#ifdef DEBUG
		if(Debug(DebugCompletionUnit))
			cout << name() << ": system register unit produce CR rename buffer #" << (int) inSystemRegisterResultCRTag << endl;
#endif
		/* Validate the CR field rename buffer */
		registersBinding->ValidateCRRenameBuffer(inSystemRegisterResultCRTag);
	}
	
	/* Finish */
	
	for(i = 0; i < nIntegerUnit; i++)
	{
		if(inIntegerFinished[i])
		{
			/* An integer unit finished an instruction */
			tag_t tag = inIntegerTag[i];
#ifdef DEBUG
			if(Debug(DebugCompletionUnit))
			{
				cout << name() << ": integer unit " << i << " has finished an instruction (tag = " << (int) tag << ")" << endl;
				if(!rob->Contains(tag))
				{
					cout << name() << ": Error : Reorder buffer does not contain tag " << (int) tag << endl;
					ABORT();
				}
			}
#endif
			/* Get the reorder buffer entry containing that instruction */
			ReorderBufferEntry& robEntry = rob->GetEntry(tag);
			/* Finish the instruction */
			robEntry.finished = true;
			/* Collect result that must be temporaly stored into the reorder buffer (XER[OV] and XER[CA])*/
			if(robEntry.extra.integer.writeOverflow) robEntry.extra.integer.overflow = inIntegerOverflow[i];
			if(robEntry.extra.integer.writeCarry) robEntry.extra.integer.carry = inIntegerCarry[i];
			if(robEntry.executionSerialized) outIntegerExecute[i] = false;
#ifdef TRACE
			if(Trace(TraceCompletionUnit))
			{
				trace_file->Begin("finish");
				trace_file->Value("number", robEntry.dbgInstructionCounter);
				trace_file->Value("pc", robEntry.dbgProgramCounter);
				trace_file->End("finish");
			}
#endif
		}
	}
	
	for(i = 0; i < nFloatingPointUnits; i++)
	{
		if(inFloatingPointFinished[i])
		{
			/* A Floating point unit finished an instruction */
			tag_t tag = inFloatingPointTag[i];
#ifdef DEBUG
			if(Debug(DebugCompletionUnit))
			{
				cout << name() << ": floating point unit " << i << " has finished an instruction (tag = " << (int) tag << ")" << endl;
				if(!rob->Contains(tag))
				{
					cout << name() << ": Error : Reorder buffer does not contain tag " << (int) tag << endl;
					ABORT();
				}
			}
#endif
			/* Get the reorder buffer entry containing that instruction */
			ReorderBufferEntry& robEntry = rob->GetEntry(tag);
			/* Finish the instruction */
			robEntry.finished = true;
			if(robEntry.executionSerialized) outFloatingPointExecute[i] = false;
		}
	}
	
	for(i = 0; i < 2; i++)
	{
		if(inLoadStoreFinished[i])
		{
			/* Load/store unit finished an instruction */
			tag_t tag = inLoadStoreTag[i];
#ifdef DEBUG
			if(Debug(DebugCompletionUnit))
			{
				cout << name() << ": load/store unit has finished an instruction (tag = " << (int) tag  << ")" << endl;
				if(!rob->Contains(tag))
				{
					cout << name() << ": Reorder buffer does not contain this instruction (tag = " << (int) tag << ")" << endl;
					ABORT();
				}
			}
#endif
			/* Get the reorder buffer entry containing that instruction */
			ReorderBufferEntry& robEntry = rob->GetEntry(tag);
			/* Finish the instruction */
			robEntry.finished = true;
		}
	}
	
	if(inSystemRegisterFinished)
	{
		/* System Register Unit finished an instruction */
		tag_t tag = inSystemRegisterTag;
#ifdef DEBUG
		if(Debug(DebugCompletionUnit))
		{
			cout << name() << ": system register unit has finished an instruction (tag = " << (int) tag << ")" << endl;
			if(!rob->Contains(tag))
			{
				cout << name() << ": Reorder buffer does not contain this instruction (tag = " << (int) tag << ")" << endl;
				ABORT();
			}
		}
#endif
		/* Get the reorder buffer entry containing that instruction */
		ReorderBufferEntry& robEntry = rob->GetEntry(tag);
		/* Finish the instruction */
		robEntry.finished = true;
		if(robEntry.extra.systemRegister.writeCR) robEntry.extra.systemRegister.cr = inSystemRegisterCRData;
	}
	
	if(inBranchFinished)
	{
		/* Branch unit finished an instruction */
		if(inBranchTag != -1)
		{
#ifdef DEBUG
			if(Debug(DebugCompletionUnit))
			{
				cout << name() << ": branch unit has finished an instruction (tag = " << (int) inBranchTag << ")" << endl;
				if(!rob->Contains(inBranchTag))
				{
					cout << name() << ": Reorder buffer does not contain this instruction (tag = " << (int) inBranchTag << ")" << endl;
					ABORT();
				}
			}
#endif
			/* Get the reorder buffer entry containing that instruction */
			ReorderBufferEntry& robEntry = rob->GetEntry(inBranchTag);
			/* Finish the instruction */
			robEntry.finished = true;		
		}
		else
		{
			nRetiredInstructions++;
		}
		
		if(inMispredictedBranch)
		{
			/* If branch has been mispredicted then flush dependent instructions */
#ifdef DEBUG
			if(Debug(DebugCompletionUnit))
			{
				cout << name() << ": Mispredicted Branch" << endl;
				cout << name() << ": Reorder buffer content before flushing :" << endl << *rob;
				cout << name() << ": Registers Binding before flushing :" << endl << *registersBinding;
			}
#endif
			Flush();
#ifdef DEBUG
			if(Debug(DebugCompletionUnit))
			{
				cout << name() << ": Reorder buffer content after flushing :" << endl << *rob;
				cout << name() << ": Registers Binding after flushing :" << endl << *registersBinding;
			}
#endif
		}
		else
		{
			/* The branch has been resolved and branch preduction was good */
#ifdef DEBUG
			if(Debug(DebugCompletionUnit))
				cout << name() << ": Decrement Branch Counter" << endl;
#endif
			rob->DecrementBranchCounter();
		}
	}
	
#ifdef DEBUG
	if(Debug(DebugCompletionUnit))
	{
		cout << "Reorder buffer content :" << endl;
		cout << *rob;
	}
#endif
}

void CompletionUnit::OnFallingEdge()
{
	if(nMaxRetiredInstructions > 0 && nRetiredInstructions >= nMaxRetiredInstructions)
	{
		StopSimulation();
	}
}

void CompletionUnit::Flush()
{
	int j;
	
	/* Flush instructions that have their branch counter > 0 */
	/* Instructions that are flushed release some rename buffers (in reverse order they are allocated) */
	while(!rob->Empty())
	{
		ReorderBufferEntry& robEntry = rob->GetHead();
		
		if(robEntry.branchCounter == 0) return;
		
#ifdef DEBUG
		if(Debug(DebugCompletionUnit))
		{
			cout << name() << ": Flushing instruction (" << robEntry.dbgInstructionCounter << ")  ";
			ppc_disassemble_to_stream(robEntry.dbgInstruction, robEntry.dbgProgramCounter, stdout);
			cout << endl;
		}
#endif
		switch(robEntry.unitIdent)
		{
			case IntegerUnitIdent:
				if(robEntry.extra.integer.tags.resultTag >= 0)
					registersBinding->FreeRenameBufferReverseOrder(robEntry.extra.integer.tags.resultTag);
					
				if(robEntry.extra.integer.tags.CRTag >= 0)
					registersBinding->FreeCRRenameBufferReverseOrder(robEntry.extra.integer.tags.CRTag);
					
				break;
				
			case FloatingPointUnitIdent:
				if(robEntry.extra.floatingPoint.tags.resultTag >= 0)
					registersBinding->FreeFloatingPointRenameBufferReverseOrder(robEntry.extra.floatingPoint.tags.resultTag);
					
				if(robEntry.extra.floatingPoint.tags.CRTag >= 0)
					registersBinding->FreeCRRenameBufferReverseOrder(robEntry.extra.floatingPoint.tags.CRTag);
					
				break;
			
			case LoadStoreUnitIdent:
				if(robEntry.extra.loadStore.tags.resultTags[0] >= 0)
				{
					if(robEntry.extra.loadStore.floatingPoint)
					{
						registersBinding->FreeFloatingPointRenameBufferReverseOrder(robEntry.extra.loadStore.tags.resultTags[0]);
					}
					else
					{
						registersBinding->FreeRenameBufferReverseOrder(robEntry.extra.loadStore.tags.resultTags[0]);
					}
				}

				if(robEntry.extra.loadStore.tags.resultTags[1] >= 0)
						registersBinding->FreeRenameBufferReverseOrder(robEntry.extra.loadStore.tags.resultTags[1]);
				break;
				
			case BranchUnitIdent:
				if(robEntry.extra.branch.tags.LRTag >= 0)
					registersBinding->FreeLRRenameBufferReverseOrder(robEntry.extra.branch.tags.LRTag);
					
				if(robEntry.extra.branch.tags.CTRTag >= 0)
					registersBinding->FreeCTRRenameBufferReverseOrder(robEntry.extra.branch.tags.CTRTag);
					
				break;
				
			case SystemRegisterUnitIdent:
				if(robEntry.extra.systemRegister.tags.resultTag >= 0)
					registersBinding->FreeRenameBufferReverseOrder(robEntry.extra.systemRegister.tags.resultTag);
						
				if(robEntry.extra.systemRegister.tags.LRTag >= 0)
					registersBinding->FreeLRRenameBufferReverseOrder(robEntry.extra.systemRegister.tags.LRTag);
						
				if(robEntry.extra.systemRegister.tags.CTRTag >= 0)
					registersBinding->FreeCTRRenameBufferReverseOrder(robEntry.extra.systemRegister.tags.CTRTag);
					
				if(robEntry.extra.systemRegister.tags.CRTag >= 0)
					registersBinding->FreeCRRenameBufferReverseOrder(robEntry.extra.systemRegister.tags.CRTag);
					
				break;
		}
		
		rob->ReleaseHead();
	}
}

void CompletionUnit::Reset()
{
	nInstructionsToRetire = 0;
	nRetiredInstructions = 0;
	nMaxRetiredInstructions = 0;
	rob->Flush();
}

void CompletionUnit::Stop(int ninstructions)
{
	nRetiredInstructions = 0;
	nMaxRetiredInstructions = ninstructions;
}