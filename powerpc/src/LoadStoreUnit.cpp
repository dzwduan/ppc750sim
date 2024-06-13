/***************************************************************************
                          LoadStoreUnit.cpp  -  description
                             -------------------
    begin                : Tue Apr 24 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <LoadStoreUnit.h>

bool LoadStoreUnit::Misalignment(UInt32 addr, int size)
{
	return (addr % size) != 0;
}

bool LoadStoreUnit::CrossLineBoundary(UInt32 addr, int size)
{
	return linesize - (addr % linesize) < size;
}

void LoadStoreUnit::OnFrontEdge()
{
	bool finished = false;
	bool resultValid = false;
	
#ifdef DEBUG
	if(Debug(DebugLoadStoreUnit))
	{
		cout << stage2.storeQueue;
		cout << "load waiting for ack = " << loadWaitingForAck << endl;
		cout << "store waiting for ack = " << storeWaitingForAck << endl;
		cout << "stage1.valid = " << stage1.valid << endl;
		cout << "stage2.load.valid = " << stage2.load.valid << endl;
	}
#endif
		
	/* Data Cache Access */
	
	/* See if we are waiting for an instruction accessing to the data cache */
	if(!loadWaitingForAck && !storeWaitingForAck)
	{
		/* We are free to access to the data cache */
		/* See if the load can be done before some store instructions by checking the dependencies */
		if(stage2.load.valid && !stage2.storeQueue.HasLoadDependency(stage2.load.tags.tag))
		{
			/* Load */
#ifdef DEBUG
			if(Debug(DebugLoadStoreUnit))
			{
				cout << name() << ": Loading " << (int) stage2.load.operation.size << " bytes at "; WriteHex(cout, stage2.load.addr); cout << endl;
			}
#endif
			Load();
			/* Make the load/store unit to wait for the load to complete into the the data cache */
			loadWaitingForAck = true;
		}
		else
		{
			/* Store */
			
			/* Get a the oldest store into the store queue */
			StoreQueueEntry *entry = stage2.storeQueue.GetOldestValid();
			
#ifdef DEBUG
			if(Debug(DebugLoadStoreUnit))
				cout << name() << ": entry = " << entry << endl;
#endif
		
			if(entry)
			{
				Store(entry);
			
				/* Make the load/store to wait for the store to complete */
				storeWaitingForAck = true;
#ifdef DEBUG
				if(Debug(DebugLoadStoreUnit))
				{
					cout << name() << ": Storing " << entry->data << " at ";
					WriteHex(cout, entry->addr);
					cout << endl;
				}
#endif
			}
		}
	}
	
	/* Effective Address Calculation */
	
	bool storeQueueStalled = stage2.storeQueue.GetFreeSpace() <= 0;
	bool loadQueueStalled = stage2.load.valid || loadWaitingForAck;
	
	if(inDispatched)
	{
		/* A new load/store instruction has been started */
		const LoadStoreOperation& operation = inOperation;
		const LoadStoreOperands& operands = inOperands;
		const LoadStoreTags& tags = inTags;
#if defined(DEBUG) || defined(TRACE)
		UInt32 dbgInstructionCounter = inInstructionCounter;
		UInt32 dbgProgramCounter = inProgramCounter;
		UInt32 dbgInstruction = inInstruction;
#endif

#ifdef DEBUG
		if(Debug(DebugLoadStoreUnit))
		{
			cout << name() << ": " << (operation.write ? "store" : "load") << " dispatched (tag = " << (int) tags.tag << ")   (" << dbgInstructionCounter << ")  ";
			ppc_disassemble_to_stream(dbgInstruction, dbgProgramCounter, stdout);
			cout << endl;
			cout << name() << ": Computing Effective Address" << endl;
			cout << name() << ": (RS) = ";
			WriteHex(cout, operands.data0);
			cout << ", (RA) = ";
			WriteHex(cout, operands.data1);
			cout << ", (RB)/immed = ";
			WriteHex(cout, operands.data2);
			cout << endl;
			if(operation.nullRA)
				cout << name() << ": RA = 0" << endl;
		}
#endif
		/* Effective Address Calculation */
		/* operands.data[0] -> (RS) */
		/* operands.data[1] -> (RA) */
		/* operands.data[2] -> (RB)/immed */	
		UInt32 EA = operation.nullRA ? operands.data2 : operands.data1 + operands.data2;
		stage1.tags = tags;
		stage1.addr = EA;
		stage1.operation = operation;
		if(stage1.operation.ident == ID_DCBZ)
		{
			stage1.operation.size = linesize;
		}
		stage1.branchCounter = inBranchCounter;
		stage1.valid = true;
		stage1.data = operands.data0;
#if defined(DEBUG) || defined(TRACE)
		stage1.dbgInstructionCounter = dbgInstructionCounter;
		stage1.dbgProgramCounter = dbgProgramCounter;
		stage1.dbgInstruction = dbgInstruction;
#endif
		
#ifdef DEBUG
		if(Debug(DebugLoadStoreUnit))
		{
			cout << name() << ": EA = ";
			WriteHex(cout, EA);
			cout << endl;
		}
#endif
#ifdef TRACE
		if(Trace(TraceLoadStoreUnit))
		{
			trace_file->Begin("execute");
			trace_file->Value("unit", "load/store");
			trace_file->Value("number", dbgInstructionCounter);
			trace_file->Value("pc", dbgInstructionCounter);
			trace_file->Value("write", operation.write);
			trace_file->Value("addr", EA);
			trace_file->End("execute");
		}
#endif
		
		
		if(stage1.operation.write)
		{
#ifdef DEBUG
			if(Debug(DebugLoadStoreUnit))
			{
				cout << name() << ": finishing a store (tag = " << (int) stage1.tags.tag << ")   (" << stage1.dbgInstructionCounter << ")  ";
				ppc_disassemble_to_stream(stage1.dbgInstruction, stage1.dbgProgramCounter, stdout);
				cout << endl;
			}
#endif
			finished = true;
			outTag[0] = stage1.tags.tag;
			storeQueueStalled = stage2.storeQueue.GetFreeSpace() <= 1;
		}
		else
		{
			loadQueueStalled = true;
		}
		/* Update (RA) if necessary */
		if(tags.resultTags[1] >= 0)
		{
			resultValid = true;
			outResultData[1] = EA;
			outResultTag[1] = tags.resultTags[1];
		}
	}
	
	outFinished[0] = finished;
	outFinished[1] = false;
	outResultValid[0] = false;
	outFloatingPointResultValid = false;
	outResultValid[1] = resultValid;

#ifdef DEBUG
	if(Debug(DebugLoadStoreUnit))
	{
		if(storeQueueStalled)
			cout << name() << ": Store Queue stalled" << endl;
		
		if(stage2.load.valid)
			cout << name() << ": Load Queue stalled" << endl;
	}
#endif	
#ifdef TRACE
	if(Trace(TraceLoadStoreUnit))
	{
		stage2.storeQueue.Trace();
	}
#endif
	
	outStoreQueueStalled = storeQueueStalled;
	outLoadQueueStalled = loadQueueStalled;
}

void LoadStoreUnit::Forward()
{
	if(loadWaitingForAck && (Dummy<bool>) inDCacheAck)
	{
		if(stage2.load.valid)
		{
			/* Load */
			/* Forward of the load result */
			UInt64 result;

#ifdef DEBUG
			if(Debug(DebugLoadStoreUnit))
				cout << name() << ": Getting data from Data Cache" << endl;
#endif
			
			switch(stage2.load.state)
			{
				case NORMAL_ACCESS:
					stage2.load.data = inDCacheData;
					break;
				
				case MSB_ACCESS:
					{
						DataArray<lsu2dcache> data = inDCacheData;
						
						
						UInt32 size = linesize - (stage2.load.addr % linesize);
					
						int i;
						
#ifdef DEBUG
						if(Debug(DebugLoadStoreUnit))
							cout << name() << ": MSB =";
#endif
						
						for(i = 0; i < size; i++)
						{
							stage2.load.data[i] = data[i];
#ifdef DEBUG
							if(Debug(DebugLoadStoreUnit))
							{
								cout << " "; WriteHex(cout, stage2.load.data[i]);
							}
#endif
						}
#ifdef DEBUG
						if(Debug(DebugLoadStoreUnit))
							cout << endl;
#endif
					}
					break;
					
				case LSB_ACCESS:
					{
						DataArray<lsu2dcache> data = inDCacheData;			
					
						UInt32 offset = linesize - (stage2.load.addr % linesize);
						UInt32 size = stage2.load.operation.size - offset;

						int i;
									
#ifdef DEBUG
						if(Debug(DebugLoadStoreUnit))
							cout << name() << ": LSB =";
#endif		
						for(i = 0; i < size; i++)
						{
							stage2.load.data[offset + i] = data[i];
#ifdef DEBUG
							if(Debug(DebugLoadStoreUnit))
							{
								cout << " "; WriteHex(cout, data[i]);
							}
#endif
						}
#ifdef DEBUG
						if(Debug(DebugLoadStoreUnit))
							cout << endl;
#endif
					}
					break;
			}

			switch(stage2.load.state)
			{
				case NORMAL_ACCESS:
				case LSB_ACCESS:
					if(stage2.load.operation.algebraic)
					{
						/* Sign extension of the load result  */
						switch(stage2.load.operation.size)
						{
							case 1:
								result = (SInt32) (SInt8) stage2.load.data[0];
								break;
							case 2:
								result = (SInt32) (SInt16) (stage2.load.data[0] << 8) | (stage2.load.data[1]);
								break;
							case 4:
								result = (stage2.load.data[0] << 24) | (stage2.load.data[1] << 16) | (stage2.load.data[2] << 8) | (stage2.load.data[3]);
								break;
							default:
								cout << name() << ": size is not valid" << endl;
								ABORT();
						}
					}
					else
					{
						/* zero extension of the load result */
						switch(stage2.load.operation.size)
						{
							case 1:
								result = stage2.load.data[0];
								break;
							case 2:
								result = (stage2.load.data[0] << 8) | (stage2.load.data[1]);
								break;
							case 4:
								if(stage2.load.operation.floatingPoint)
								{
									UInt32 result32 = (stage2.load.data[0] << 24) | (stage2.load.data[1] << 16) | (stage2.load.data[2] << 8) | (stage2.load.data[3]);
									float fresult = *(float *) &result32;
									double dresult = fresult;
									result = *(UInt64 *) &dresult;
								}
								else
								{						
									result = (stage2.load.data[0] << 24) | (stage2.load.data[1] << 16) | (stage2.load.data[2] << 8) | (stage2.load.data[3]);
								}
								break;						
							case 8:
								result = ((UInt64) stage2.load.data[0] << 56) |
								          ((UInt64) stage2.load.data[1] << 48) |
								          ((UInt64) stage2.load.data[2] << 40) |
								          ((UInt64) stage2.load.data[3] << 32) |
								          ((UInt64) stage2.load.data[4] << 24) |
								          ((UInt64) stage2.load.data[5] << 16) |
								          ((UInt64) stage2.load.data[6] << 8) |
								          ((UInt64) stage2.load.data[7]);
								break;
						}
					}
					
#ifdef DEBUG
					if(Debug(DebugLoadStoreUnit))
					{
						cout << name() << ": producing ";
						WriteHex(cout, result);
						cout << endl;
						cout << name() << ": finishing a load (tag = " << (int) stage2.load.tags.tag << ")  (" << stage2.load.dbgInstructionCounter << ")  ";
						ppc_disassemble_to_stream(stage2.load.dbgInstruction, stage2.load.dbgProgramCounter, stdout);
						cout << endl;
						cout << name() << ": Load Queue is not stalled" << endl;
					}
#endif			
					/* Make the result available to other units */
					if(stage2.load.operation.floatingPoint)
					{
						outFloatingPointResultValid = true;
						outResultValid[0] = false;
						outFloatingPointResultData = result;
					}
					else
					{
						outFloatingPointResultValid = false;
						outResultValid[0] = true;
						outResultData[0] = result;
					}
					outResultTag[0] = stage2.load.tags.resultTags[0];
					outFinished[1] = true;
					outTag[1] = stage2.load.tags.tag;
			
					outLoadQueueStalled = false;
					
					break;
			}
		}
	}
	
	if(storeWaitingForAck && (Dummy<bool>) inDCacheAck)
	{
		StoreQueueEntry *entry = stage2.storeQueue.GetOldest();
		outStoreQueueStalled = entry->state == MSB_ACCESS;
	}
}

	
void LoadStoreUnit::OnFallingEdge()
{
	if(storeWaitingForAck && (Dummy<bool>) inDCacheAck)
	{
		StoreQueueEntry *entry = stage2.storeQueue.GetOldest();
		storeWaitingForAck = false;
		switch(entry->state)
		{
			case MSB_ACCESS:
				outDCacheReq = false;
#ifdef DEBUG
				if(Debug(DebugLoadStoreUnit))
					cout << name() << ": End of MSB access" << endl;
#endif
				entry->state = LSB_ACCESS;
				break;
			case NORMAL_ACCESS:
			case LSB_ACCESS:
				outDCacheReq = false;
				stage2.storeQueue.ReleaseTail();
#ifdef DEBUG
				if(Debug(DebugLoadStoreUnit))
				{
					cout << name() << ": Removing a store from store queue" << endl;
					cout << name() << ": Store Queue is not stalled" << endl;
				}
#endif
				break;
		}
#ifdef TRACE
		if(Trace(TraceLoadStoreUnit))
		{
			trace_file->Begin("dcache_response");
			trace_file->Value("number", entry->dbgInstructionCounter);
			trace_file->Value("pc", entry->dbgProgramCounter);
			trace_file->End("dcache_response");
		}
#endif
	}
	
	if(loadWaitingForAck && (Dummy<bool>) inDCacheAck)
	{		
		loadWaitingForAck = false;
		switch(stage2.load.state)
		{
			case MSB_ACCESS:
				outDCacheReq = false;
#ifdef DEBUG
				if(Debug(DebugLoadStoreUnit))
					cout << name() << ": End of MSB access" << endl;
#endif
				stage2.load.state = LSB_ACCESS;
				break;
		
			case NORMAL_ACCESS:
			case LSB_ACCESS:
				outDCacheReq = false;
				stage2.load.valid = false;
#ifdef DEBUG
				if(Debug(DebugLoadStoreUnit))
				{
					cout << name() << ": Removing a load from load queue" << endl;
					cout << name() << ": Load Queue is not stalled" << endl;
				}
#endif
				break;
		}
#ifdef TRACE
		if(Trace(TraceLoadStoreUnit))
		{
			trace_file->Begin("dcache_response");
			trace_file->Value("number", stage2.load.dbgInstructionCounter);
			trace_file->Value("pc", stage2.load.dbgProgramCounter);
			trace_file->End("dcache_response");
		}
#endif
	}		

	
#ifdef DEBUG
	if(Debug(DebugLoadStoreUnit))
	{
		cout << stage2.storeQueue;
		cout << "stage1.valid = " << stage1.valid << endl;
		cout << "load waiting for ack = " << loadWaitingForAck << endl;
		cout << "store waiting for ack = " << storeWaitingForAck << endl;	
	}
#endif
	if(inBranchFinished)
	{
		if(inMispredictedBranch)
			Flush();
		
		DecrementBranchCounter();
	}

#ifdef DEBUG
	if(Debug(DebugLoadStoreUnit))
	{
		cout << "stage1.valid = " << stage1.valid << endl;
		cout << "stage1.operation.ident = " << stage1.operation.ident << endl;
		cout << "stage1.operation.write = " << stage1.operation.write << endl;
	}
#endif

	stage2.storeQueue.Shift();
	
#ifdef DEBUG
	if(Debug(DebugLoadStoreUnit))
		cout << "store queue after shifting :" << endl << stage2.storeQueue;
#endif
	
	/* Shift the pipeline if we can do it */
	if(stage1.valid)
	{
		if(!stage1.operation.write)
		{
			/* Load */

#ifdef DEBUG		
			if(stage2.load.valid)
			{
				cout << ": Can't add a load to load queue (tag = " << (int) stage1.tags.tag << ")  (" << stage1.dbgInstructionCounter << ")  ";
				ppc_disassemble_to_stream(stage1.dbgInstruction, stage1.dbgProgramCounter, stdout);
				cout << endl;
				ABORT();
			}
#endif
			stage2.load.addr = stage1.addr;
			stage2.load.operation = stage1.operation;
			stage2.load.tags = stage1.tags;
			stage2.load.branchCounter = stage1.branchCounter;
			stage2.load.valid = true;
			stage2.storeQueue.SetLoadDependency(stage2.load.tags.tag, stage2.load.addr, stage2.load.operation.size);
			stage2.load.state = NORMAL_ACCESS;
			stage1.valid = false;
#if defined(DEBUG) || defined(TRACE)
			stage2.load.dbgInstructionCounter = stage1.dbgInstructionCounter;
			stage2.load.dbgProgramCounter = stage1.dbgProgramCounter;
			stage2.load.dbgInstruction = stage1.dbgInstruction;
#endif
		}
		else
		{
			/* Store */
#ifdef DEBUG
			if(stage2.storeQueue.GetFreeSpace() <= 0)
			{
				cout << name() << ": Can't add a store to store queue (tag = " << (int) stage1.tags.tag << ")  (" << stage1.dbgInstructionCounter << ")  ";
				ppc_disassemble_to_stream(stage1.dbgInstruction, stage1.dbgProgramCounter, stdout);
				cout << endl;
				ABORT();
			}
#endif
			
			/* Filling of the store queue */
			StoreQueueEntry *entry = stage2.storeQueue.Allocate();
			entry->addr = stage1.addr;
			entry->operation = stage1.operation;
			entry->tags = stage1.tags;
			entry->data = stage1.data;
			entry->branchCounter = stage1.branchCounter;
			entry->valid = false;
			entry->dependentLoadTag = -1;
			entry->state = NORMAL_ACCESS;
#if defined(DEBUG) || defined(TRACE)
			entry->dbgInstructionCounter = stage1.dbgInstructionCounter;
			entry->dbgProgramCounter = stage1.dbgProgramCounter;
			entry->dbgInstruction = stage1.dbgInstruction;
#endif
			stage1.valid = false;

#ifdef DEBUG
			if(Debug(DebugLoadStoreUnit))
			{
				cout << name() << ": Adding store to store queue (tag = " << (int) stage1.tags.tag << ")  (" << stage1.dbgInstructionCounter << ")  ";
				ppc_disassemble_to_stream(stage1.dbgInstruction, stage1.dbgProgramCounter, stdout);
				cout << endl;
				cout << stage2.storeQueue;
			}
#endif
		}
	}
	
	outEmpty = !stage1.valid && !stage2.load.valid && stage2.storeQueue.Empty();
}

void LoadStoreUnit::Load()
{
	/* Do the data cache request */
	outDCacheReq = true;
			
	/* There is one or two data cache access depending on the data alignment */
	/* If the data access crosses a cache line boundary there is two data cache access */
	/* In most cases there is only one data cache access */
	switch(stage2.load.state)
	{
		case NORMAL_ACCESS:
			if(Misalignment(stage2.load.addr, stage2.load.operation.size) && CrossLineBoundary(stage2.load.addr, stage2.load.operation.size))
			{
				/* Data cache access crosses a cache line boundary */
				/* Start the data cache access with the most significative bytes */
				stage2.load.state = MSB_ACCESS;
				
				UInt32 size = linesize - (stage2.load.addr % linesize);
				outDCacheSize = size;
#ifdef TRACE
				if(Trace(TraceLoadStoreUnit))
				{
					trace_file->Begin("dcache_request");
					trace_file->Value("addr", stage2.load.addr);
					trace_file->Value("size", size);
					trace_file->Value("write", false);
					trace_file->End("dcache_request");
				}
#endif				
			}
			else
			{
				/* Data cache access is well aligned */
				outDCacheSize = stage2.load.operation.size;
#ifdef TRACE
				if(Trace(TraceLoadStoreUnit))
				{
					trace_file->Begin("dcache_request");
					trace_file->Value("addr", stage2.load.addr);
					trace_file->Value("size", stage2.load.operation.size);
					trace_file->Value("write", false);
					trace_file->End("dcache_request");
				}
#endif				
			}
			outDCacheAddr = stage2.load.addr;
			break;
				
		case LSB_ACCESS:
			{
				/* We previously access to the most significative bytes */
				/* Now we do the data cache access for the less significative bytes */
				UInt32 addr = (stage2.load.addr - stage2.load.addr % linesize) + linesize;;
				outDCacheAddr = addr;
				UInt32 size = stage2.load.operation.size - (linesize - (stage2.load.addr % linesize));
				outDCacheSize = size;
#ifdef TRACE
				if(Trace(TraceLoadStoreUnit))
				{
					trace_file->Begin("dcache_request");
					trace_file->Value("addr", stage2.load.addr);
					trace_file->Value("size", size);
					trace_file->Value("write", false);
					trace_file->End("dcache_request");
				}
#endif				
			}
			break;
	}
	outDCacheWrite = false;
}

void LoadStoreUnit::Store(StoreQueueEntry *entry)
{
	DataArray<lsu2dcache> data;
	outDCacheReq = true;
	outDCacheWrite = true;
			
	switch(entry->operation.ident)
	{
		case ID_DCBZ:
			outDCacheAddr = entry->addr;
			outDCacheSize = entry->operation.size;
			outDCacheZeroBlock = true;
			break;
		default:
			{
				UInt64 dataToWrite;
				/* There is one or two data cache access depending on the data alignment */
				/* If the data access crosses a cache line boundary there is two data cache access */
				/* In most cases there is only one data cache access */
							
				switch(entry->state)
				{
					case NORMAL_ACCESS:
						if(Misalignment(entry->addr, entry->operation.size) && CrossLineBoundary(entry->addr, entry->operation.size))
						{
							/* Data cache access crosses a cache line boundary */
							/* Start the data cache access with the most significative bytes */
							entry->state = MSB_ACCESS;
							UInt32 size = linesize - (entry->addr % linesize);
							outDCacheSize = size;
							
#ifdef TRACE
							if(Trace(TraceLoadStoreUnit))
							{
								trace_file->Begin("dcache_access");
								trace_file->Value("addr", entry->addr);
								trace_file->Value("size", size);
								trace_file->Value("write", true);
								trace_file->End("dcache_access");
							}
#endif							
							dataToWrite = entry->data;
						}
						else
						{
							/* Data cache access is well aligned */
							outDCacheSize = entry->operation.size;
							dataToWrite = entry->data;
#ifdef TRACE
							if(Trace(TraceLoadStoreUnit))
							{
								trace_file->Begin("dcache_access");
								trace_file->Value("addr", entry->addr);
								trace_file->Value("size", entry->operation.size);
								trace_file->Value("write", true);
								trace_file->End("dcache_access");
							}
#endif							
						}
						outDCacheAddr = entry->addr;
						break;
						
					case LSB_ACCESS:
						{
							/* We previously access to the most significative bytes */
							/* Now we do the data cache access for the less significative bytes */
							UInt32 addr = (entry->addr - entry->addr % linesize) + linesize;
							outDCacheAddr = addr;
							UInt32 size = entry->operation.size - (linesize - (entry->addr % linesize));
							outDCacheSize = size;
							dataToWrite = entry->data << (8 * (linesize - entry->addr % linesize));
#ifdef TRACE
							if(Trace(TraceLoadStoreUnit))
							{
								trace_file->Begin("dcache_access");
								trace_file->Value("addr", addr);
								trace_file->Value("size", size);
								trace_file->Value("write", true);
								trace_file->End("dcache_access");
							}
#endif							
						}
						break;
				}
				
				switch(entry->operation.size)
				{
					case 1:
						data[0] = dataToWrite;
						break;
				
					case 2:
						data[0] = dataToWrite >> 8;
						data[1] = dataToWrite;
						break;
					
					case 4:
						if(entry->operation.floatingPoint)
						{
							/* 64 bits Floating point data must be converted to a 32 bits floating point data */
							double d = *(double *) &dataToWrite;
							float f = d;
							UInt32 data32 = *(UInt32 *) &f;
							data[0] = data32 >> 24;
							data[1] = data32 >> 16;
							data[2] = data32 >> 8;
							data[3] = data32;									
						}
						else
						{
							data[0] = dataToWrite >> 24;
							data[1] = dataToWrite >> 16;
							data[2] = dataToWrite >> 8;
							data[3] = dataToWrite;
						}
						break;
						
					case 8:
						data[0] = dataToWrite >> 56;
						data[1] = dataToWrite >> 48;
						data[2] = dataToWrite >> 40;
						data[3] = dataToWrite >> 32;
						data[4] = dataToWrite >> 24;
						data[5] = dataToWrite >> 16;
						data[6] = dataToWrite >> 8;
						data[7] = dataToWrite;
						break;
				}
				outDCacheData = data;
				outDCacheZeroBlock = false;
			}	
	}
}

void LoadStoreUnit::WriteBack()
{
	int i;
	
#ifdef DEBUG
	if(Debug(DebugLoadStoreUnit))
		cout << name() << ": Write Back" << endl;
#endif
	for(i = 0; i < nMemoryWriteBackPorts; i++)
	{
		if((Dummy<bool>) inWriteBack[i])
		{
			StoreQueueEntry *entry = stage2.storeQueue.GetOldest();
	
			if(!loadWaitingForAck && !storeWaitingForAck && entry && entry->tags.tag == inWriteBackTag[i])
			{
				entry->valid = true;
				Store(entry);			
#ifdef DEBUG
				if(Debug(DebugLoadStoreUnit))
				{
					cout << name() << ": Writing " << entry->operation.size << " bytes (" << entry->data << ") at ";
					WriteHex(cout, entry->addr);
					cout << endl;
				}
#endif				
				storeWaitingForAck = true;
			}
			else
			{
#ifdef DEBUG
				if(Debug(DebugLoadStoreUnit))
					cout << name() << ": Validating a store (tag = " << (int) inWriteBackTag[i] << ")" << endl;
#endif
				stage2.storeQueue.Validate(inWriteBackTag[i]);
			}
		}
	}	
}


void LoadStoreUnit::DecrementBranchCounter()
{
	if(stage1.valid)
		if(stage1.branchCounter > 0) stage1.branchCounter--;
		
	if(stage2.load.valid)
		if(stage2.load.branchCounter > 0) stage2.load.branchCounter--;
		
	stage2.storeQueue.DecrementBranchCounter();
}

void LoadStoreUnit::Flush()
{
	if(stage1.valid)
		if(stage1.branchCounter > 0) stage1.valid = false;
		
	if(stage2.load.valid)
		if(stage2.load.branchCounter > 0) stage2.load.valid = false;
		
	stage2.storeQueue.Flush();
}

void LoadStoreUnit::Reset()
{
	loadWaitingForAck = 0;
	storeWaitingForAck = 0;
	
	stage1.valid = false;
	stage1.state = 0;
	memset(&stage1.operation, 0, sizeof(stage1.operation));
	memset(&stage1.tags, -1, sizeof(stage1.tags));
	stage1.addr = 0;
	stage1.data = 0;
	stage1.branchCounter = 0;
		
	stage2.load.valid = false;
	stage2.load.state = 0;
	memset(&stage2.load.operation, 0, sizeof(stage2.load.operation));
	memset(&stage2.load.tags, -1, sizeof(stage2.load.tags));
	stage2.load.addr = 0;
	stage2.load.branchCounter = 0;
	stage2.storeQueue.Reset();
}
