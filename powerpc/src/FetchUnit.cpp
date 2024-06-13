/***************************************************************************
                          FetchUnit.cpp  -  description
                             -------------------
    begin                : Tue Apr 24 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <FetchUnit.h>

UInt32 FetchUnit::AlignToLineBoundary(UInt32 addr)
{
	return addr - (addr % linesize);
}

void FetchUnit::Fetch()
{
	int i, j;
	bool branch = false;

#ifdef DEBUG
	if(Debug(DebugFetchUnit))
	{
		cout << name() << ": state = " << state << endl;
		cout << name() << ": waitingForAck = " << waitingForAck << endl;
		cout << name() << ": mispredicted branch = " << mispredictedBranch << endl;
		cout << name() << ": updateBTIC = " << updateBTIC << endl;
		cout << name() << ": branchPC = "; WriteHex(cout, branchPC); cout << endl;
		cout << name() << ": branch counter = " << branchCounter << endl;
		cout << name() << ": " << nInstructionsToPredecode << " instructions to predecode" << endl;
	}
#endif
	/* Predecoding branchs */
	if(nInstructionsToPredecode > 0)
	{
		int instructionQueueSize = instructionQueue.Size();
		int i = instructionQueueSize - nInstructionsToPredecode;
	
		do
		{
			InstructionQueueEntry& entry = instructionQueue[i];
			
			nInstructionsToPredecode--;
			i++;
			
			if(ppc_is_a_branch(entry.instruction))
			{
				if(ppc_is_conditional_branch(entry.instruction))
				{
#ifdef DEBUG
					if(Debug(DebugFetchUnit))
						cout << name() << ": predecoding a conditional branch" << endl;
#endif			
					/* BCx */
					/* branch prediction */
					outReadBHTPC = entry.pc;
					outReadBTICPC = entry.pc;
					/* compute target address */
					entry.nia = ppc_get_conditional_branch_target_addr(entry.instruction, entry.pc);
					branchToPredict = &entry;
					state = 3;
					branch = true;
					branchCounter++;				
					break;
				}
				else
				{
					if(ppc_is_unconditional_branch(entry.instruction))
					{
#ifdef DEBUG
						if(Debug(DebugFetchUnit))
							cout << name() << ": predecoding an unconditional branch" << endl;
#endif			
						/* Bx */
						outReadBTICPC = entry.pc;
						/* compute target address */
						entry.nia = ppc_get_unconditional_branch_target_addr(entry.instruction, entry.pc);
						branchToPredict = &entry;
						state = 3;
						branch = true;
						branchCounter++;
						break;
					}
					else
					{
						/* BCLR or BCCTR */
						entry.branchPredicted = true;
						/* Those branch instructions are predicted not taken */
						branchCounter++;				
					}
				}
			} /* if(is_a_branch) */
			else
			{
				entry.branchCounter = branchCounter;
			}
			
		} while(nInstructionsToPredecode > 0);
	}
	
	if(!branch && !instructionQueue.Full() && branchCounter <= nMaxBranchsBeforeFetchStall)
	{
		/* Instruction Cache Access */
		UInt32 addr = programCounter;
		UInt32 size = linesize - (addr % linesize);
		if(size > (UInt32) fetch2icache) size = (UInt32) fetch2icache;
		nInstructionsToFetch = size / sizeof(UInt32);
	
		outICacheReq = true;
		outICacheAddr = addr;
		outICacheSize = size;
	
		waitingForAck = true;
	
#ifdef DEBUG
		if(Debug(DebugFetchUnit))
		{
			cout << name() << ": Fetching " << size << " bytes at ";
			WriteHex(cout, addr);
			cout << " from ICache" << endl;
		}
#endif
#ifdef TRACE
		trace_file->Begin("icache_request");
		trace_file->Value("addr", addr);
		trace_file->Value("size", size);
		trace_file->End("icache_request");		
#endif
		state = 1;
	}
}

/* Emulator code begins */
UInt32 FetchUnit::EmulatorFetch(UInt32 programCounter, UInt32 instruction)
{
	UInt32 inst;
#ifdef DEBUG
	if(Debug(DebugFetchUnit))
		cout << name() << ": Calling Emulator" << endl;
#endif
	inst = ppc_fetch(programCounter);
#ifdef EMULATE
	if(inst != instruction)
	{
		cout << name() << ": Emulator fetches ";
		ppc_disassemble_to_stream(inst, programCounter, stdout); cout << endl;
		cout << name() << ": Simulator fetches ";
		ppc_disassemble_to_stream(instruction, programCounter, stdout); cout << endl;
		StopSimulation(); return inst;
	}
#endif
#ifdef DEBUG
	if(Debug(DebugFetchUnit))
	{
		ppc_disassemble_to_stream(instruction, programCounter, stdout); cout << endl;
	}
#endif
	return inst;
}
/* Emulator code ends */

bool FetchUnit::ReadBTIC()
{
#ifdef TRACE
		trace_file->Begin("btic_response");
#endif	
	
	if(inReadBTICHit)
	{
#ifdef TRACE
		trace_file->Value("hit", true);
		trace_file->End("btic_response");
#endif
#ifdef DEBUG
		if(Debug(DebugFetchUnit))
			cout << name() << ": BTIC Hit" << endl;
#endif
		int i;
		int n = inReadBTICInstructionsCount;
		int freeSpaceInInstructionQueue = instructionQueue.GetFreeSpace();
		if(n > freeSpaceInInstructionQueue) n = freeSpaceInInstructionQueue;
				
		bticHits++;
		
		if(branchCounter > nMaxBranchsBeforeFetchStall) return true;
		
		for(i = 0; i < n; i++, programCounter += sizeof(UInt32))
		{
			nInstructionsToPredecode++;
			UInt32 instruction = inReadBTICInstructions[i];
			
			InstructionQueueEntry& entry = instructionQueue.Allocate();
			entry.branchPredictedTaken = false;
			entry.instruction = inReadBTICInstructions[i];
			entry.pc = programCounter;
			entry.nia = programCounter + 4;
			entry.branchFollow = false;
			entry.branchCounter = branchCounter;
#if defined(DEBUG) || defined(TRACE)
			entry.dbgInstructionCounter = dbgInstructionCounter++;
#endif
#ifdef DEBUG
			if(Debug(DebugFetchUnit))
			{
				cout << name() << ": Adding (" << entry.dbgInstructionCounter << ")   ";
				ppc_disassemble_to_stream(entry.instruction, programCounter, stdout);
				cout << "    into instruction queue" << endl;
			}
#endif


#ifdef TRACE
			trace_file->Begin("add_instruction");
			trace_file->Value("number", entry.dbgInstructionCounter);
			trace_file->Value("pc", entry.pc);
			trace_file->End("add_instruction");
#endif
			
#ifdef EMULATE
			/* Emulator code begins */
			entry.inst = EmulatorFetch(programCounter, entry.instruction);
			/* Emulator code ends */
#endif
		}
		return true;
	}
#ifdef DEBUG
	if(Debug(DebugFetchUnit))
		cout << name() << ": BTIC Miss" << endl;
#endif
#ifdef TRACE
	trace_file->Value("hit", false);
	trace_file->End("btic_response");
#endif
	bticMisses++;
	return false;
}

void FetchUnit::FillQueue()
{
#ifdef DEBUG
	if(Debug(DebugFetchUnit))
	{
		cout << name() << ": state = " << state << endl;
		cout << name() << ": waitingForAck = " << waitingForAck << endl;
		cout << name() << ": mispredicted branch = " << mispredictedBranch << endl;
		cout << name() << ": updateBTIC = " << updateBTIC << endl;
		cout << name() << ": branchPC = "; WriteHex(cout, branchPC); cout << endl;
		cout << name() << ": branch counter = " << branchCounter << endl;
		cout << name() << ": " << nInstructionsToPredecode << " instructions to predecode" << endl;
//		if(inDispatchedInstructions > 0)
			cout << name() << ": Removing " << inDispatchedInstructions << " instructions from instruction queue" << endl;
	}
#endif
#ifdef TRACE
	trace_file->Value("iq_pop", inDispatchedInstructions);
#endif
	instructionQueue.Pop(inDispatchedInstructions);
	branchQueue.Pop(inDispatchedBranchs);
	
	if(inBranchFinished)
	{
		if(inMispredictedBranch)
		{
#ifdef DEBUG
			if(Debug(DebugFetchUnit))
				cout << name() << ": Mispredicted branch. Flushing instruction queue. PC = " << inPC << endl;
#endif

			branchCounter = 0;
			instructionQueue.Flush();
			branchQueue.Flush();
			switch(state)
			{
				case 1:
				case 3:
					mispredictedBranch = true;
					break;
					
				case 0:
				case 2:
					state = 0;
					break;
			}
			//outICacheReq = false;
			programCounter = inPC;
			nInstructionsToFlush = 0;
			nInstructionsToPredecode = 0;
 			updateBTIC = false;
//			takenBranch = false;
		}
		else
		{
			branchCounter--;
			instructionQueue.DecrementBranchCounter();
			branchQueue.DecrementBranchCounter();		
		}
	}
	
	int i, j;

	
	
	switch(state)
	{
		case 1: /* Filling instruction queue with instruction coming from Instruction Cache */
			if(waitingForAck && inICacheAck)
			{
#ifdef DEBUG
				if(Debug(DebugFetchUnit))
					cout << name() << ": Getting a response from ICache" << endl;
#endif
#ifdef TRACE
				if(Trace(TraceFetchUnit))
				{
					trace_file->Begin("icache_response");
					trace_file->End("icache_response");
				}
#endif
				outICacheReq = false;
				waitingForAck = false;
				bool takenBranch = false;
				bool branch = false;
				
				if(!mispredictedBranch)
				{
					int freeSpaceInInstructionQueue = instructionQueue.GetFreeSpace();
					int updateBTICInstructionsCount = 0;
					int nInstructionsToUpdateIntoBTIC;
					int i = 0;
			
					if(updateBTIC)
					{
						nInstructionsToUpdateIntoBTIC = nInstructionsPerBTICEntry;
						updateBTICInstructionsCount = 0;
					}
					else
					{
						nInstructionsToUpdateIntoBTIC = 0;
					}
					
					bool dropInstructions = false;
					UInt32 offset = 0;
					const DataArray<fetch2icache>& data = inICacheData;
					
					for(i = 0; !takenBranch && nInstructionsToFetch > 0 && (freeSpaceInInstructionQueue > 0 || nInstructionsToUpdateIntoBTIC > 0); offset += sizeof(UInt32))
					{
						UInt32 instruction = (data.buffer[offset] << 24) | (data.buffer[offset+1] << 16) |
											(data.buffer[offset+2] << 8) | data.buffer[offset+3];

						if(freeSpaceInInstructionQueue > 0)
						{
							freeSpaceInInstructionQueue--;
							nInstructionsToPredecode++;
							InstructionQueueEntry& entry = instructionQueue.Allocate();
							entry.branchPredicted = false;
							entry.branchPredictedTaken = false;
							entry.instruction = instruction;
							entry.pc = programCounter;
							entry.nia = programCounter + 4;
							entry.branchFollow = false;
							entry.branchCounter = branchCounter;
#if defined(DEBUG) || defined(TRACE)
							entry.dbgInstructionCounter = dbgInstructionCounter++;
#endif
					
#ifdef EMULATE
							/* Emulator code begins*/
							entry.inst = EmulatorFetch(programCounter, entry.instruction);
							/* Emulator code ends */
#endif						
						
#ifdef DEBUG
							if(Debug(DebugFetchUnit))
							{
								cout << name() << ": Adding (" << entry.dbgInstructionCounter << ")   ";
								ppc_disassemble_to_stream(instruction, programCounter, stdout);
								cout << "    into instruction queue" << endl;
							}
#endif
#ifdef TRACE
							if(Trace(TraceFetchUnit))
							{
								trace_file->Begin("add_instruction");
								trace_file->Value("number", entry.dbgInstructionCounter);
								trace_file->Value("pc", entry.pc);
								trace_file->End("add_instruction");
							}
#endif
							i++;
				
							programCounter += sizeof(UInt32);
						} /* if(freeSpaceInInstructionQueue > 0) */
										
						if(nInstructionsToUpdateIntoBTIC > 0)
						{
							outUpdateBTICInstructions[updateBTICInstructionsCount] = instruction;
							updateBTICInstructionsCount++;
							nInstructionsToUpdateIntoBTIC--;
						}
						
						nInstructionsToFetch--;
					} /* for */
					if(updateBTIC)
					{
						outUpdateBTIC = true;
						outUpdateBTICInstructionsCount = updateBTICInstructionsCount;
						outUpdateBTICPC = updateBTICPC;
						updateBTIC = false;
#ifdef DEBUG
						if(Debug(DebugFetchUnit))
						{
							cout << name() << ": Updating BTIC" << endl;
						}
#endif
					}
					state = 2;
				}
				else
				{
					/* Mispredicted branch */
					mispredictedBranch = false;
					state = 0;
				}
			}
			break;
			
		case 3: /* Filling instruction queue with instruction coming from BTIC if BHT predicts a taken branch */
			if(!mispredictedBranch)
			{
				bool takenBranch = false;
				UInt32 branchPC;
				
				if(instructionQueue.Size() > nInstructionsToPredecode)
				{
					if(branchQueue.GetFreeSpace() > 0)
					{
						/* Branch instruction has not yet been dispatched */
						InstructionQueueEntry& entry = instructionQueue.GetRecentEntry(nInstructionsToPredecode);
						entry.branchPredicted = true;
						if(ppc_is_unconditional_branch(entry.instruction) || (ppc_is_conditional_branch(entry.instruction) && inReadBHTTakenBranch))
						{
							takenBranch = true;
							/* Branch is predicted taken */
							/* Do the Branch folding */
							/* Add the branch to the branch queue */
							InstructionQueueEntry& branchEntry = branchQueue.Allocate();
							branchEntry = entry;
							branchEntry.branchPredictedTaken = true;
							branchPC = branchEntry.pc;
							programCounter = branchEntry.nia;
					
							/* Remove instructions that have been fetched speculatively and the branch itself */
							instructionQueue.Flush(nInstructionsToPredecode + 1);
							nInstructionsToPredecode = 0;
							if(instructionQueue.Size() > 0)
							{
								InstructionQueueEntry& entry = instructionQueue.GetRecentEntry(0);
								entry.branchFollow = true;
							}
							else
							{
								branchEntry.priority = true;
							}
						}
					}
					else
					{
						nInstructionsToPredecode++;
						branchCounter--;
					}
				}
				
				if(takenBranch)
				{					
					if(ReadBTIC())
					{
						// BTIC Hit
						updateBTIC = false;
					}
					else
					{
						// BTIC Miss
						updateBTIC = true;
						updateBTICPC = branchPC;
					}
				}
			}
			else
			{
				/* Mispredicted Branch */
				mispredictedBranch = false;
				state = 0;
			}
			state = 0;
			break;
	}
	

	int nInstructionsInQueue = instructionQueue.Size();
	for(i = 0; i < nInstructionsInQueue && i < nIQ; i++)
	{	
		outIQ[i] = instructionQueue[i];
	}
	outInstructionsInQueue = nInstructionsInQueue;
	
	int nBranchsInQueue = branchQueue.Size();
	for(i = 0; i < nBranchsInQueue; i++)
	{	
		outBranchQueue[i] = branchQueue[i];
	}
	outBranchsInQueue = nBranchsInQueue;
#ifdef DEBUG
	if(Debug(DebugFetchUnit))
	{
		cout << "waitingForAck = " << waitingForAck << endl;
//		if(takenBranch) cout << name() << ": taken branch" << endl;
		cout << "Instruction queue content :" << endl;
		cout << instructionQueue;
		cout << "Branch queue content :" << endl;
		cout << branchQueue;
	}
#endif

#ifdef TRACE
	if(Trace(TraceFetchUnit))
	{
		instructionQueue.Trace();
	}
#endif
}

void FetchUnit::Reset()
{
//	programCounter = 0;
	nInstructionsToFetch = 0;
	nFetchedInstructions = 0;
	nInstructionsToFlush = 0;
	waitingForAck = false;
	mispredictedBranch = false;
	bticHits = 0;
	bticMisses = 0;
	updateBTIC = false;
	branchPC = 0;
	updateBTICPC = 0;
	branchCounter = 0;
	state = 0;
	nInstructionsToPredecode = 0;
		
#if defined(DEBUG) || defined(TRACE)
	dbgInstructionCounter = 0;
#endif

	instructionQueue.Flush();
	branchQueue.Flush();
}
