/***************************************************************************
                          DispatchUnit.h  -  Unite de dispatch
                             -------------------
    begin                : Thu Apr 5 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __DISPATCHUNIT_H__
#define __DISPATCHUNIT_H__

#include <systemc.h>
#include <IntegerRS.h>
#include <LoadStoreRS.h>
#include <RegistersBinding.h>
#include <ReorderBuffer.h>
#include <RegisterFile.h>
#include <InstructionQueue.h>
#include <BranchRS.h>
#include <SystemRegisterRS.h>
#include <FloatingPointRS.h>

SC_MODULE(DispatchUnit)
{
	/* I/O Ports */

	sc_in<bool> inClock;
	
	/* From fetch unit */
	sc_in<InstructionQueueEntry> inIQ[nIQ];	/* Oldest instructions coming from the instruction queue */
	sc_in<int> inInstructionsInQueue;						/* Number of instructions contained into the instruction queue */
	sc_in<InstructionQueueEntry> inBranchQueue[nBranchQueueEntries];
	sc_in<int> inBranchsInQueue;
	
	/* To fetch unit */
	sc_out<int> outDispatchedInstructions;					/* Number of instructions which have been decoded and dispatched */
															/* (which have to be removed from the instruction queue) */
	sc_out<int> outDispatchedBranchs;							/* Number of branchs which have been decoded and dispatched */
															/* (which have to be removed from the branch queue) */
	
	/* To Register file */
	sc_out<regnum_t> outReadRegisterNumber[nReadRegisterPorts - 1];							/* Read ports (general purpose registers): register number */
	sc_out<tag_t> outReadRenameNumber[nReadRenamePorts];									/* Read ports (rename buffers): rename buffer number */
	sc_out<regnum_t> outReadFloatingPointRegisterNumber[nReadFloatingPointRegisterPorts];	/* Read Ports (floating point registers): register number */
	sc_out<tag_t> outReadFloatingPointRenameNumber[nReadFloatingPointRenamePorts];			/* Read Ports (floating point rename buffers): rename buffer number */
	
	/* From Register file */
	sc_in<UInt32> inReadRegisterData[nReadRegisterPorts - 1];								/* Read ports (general purpose registers): register value */
	sc_in<UInt32> inReadRenameData[nReadRenamePorts];										/* Read ports (rename buffers): rename buffer value */
	sc_in<UInt64> inReadFloatingPointRegisterData[nReadFloatingPointRegisterPorts];			/* Read ports (floating point registers): register value */
	sc_in<UInt64> inReadFloatingPointRenameData[nReadFloatingPointRenamePorts];				/* Read ports (floating point rename buffers): rename buffer value */
	
	/* To Condition Register */
	sc_out<tag_t> outReadRenameCRNumber[nReadRenameCRPorts];		/* Read ports (CR field rename buffers): field number */
	
	/* From Condition Register */
	sc_in<UInt8> inReadRenameCRData[nReadRenameCRPorts];			/* Read ports (CR field rename buffers): field value */
	
	/* To Link Register */
	sc_out<tag_t> outReadRenameLRNumber[nReadRenameLRPorts];		/* Read ports (LR rename buffers): rename buffer number */
	
	/* From Link Register */
	sc_in<UInt32> inReadRenameLRData[nReadRenameLRPorts];			/* Read ports (LR rename buffers): rename buffer value */
	
	/* To Count Register */
	sc_out<tag_t> outReadRenameCTRNumber[nReadRenameCTRPorts];		/* Read Ports (CTR rename buffers): rename buffer number */
		
	/* From Count Register */
	sc_in<UInt32> inReadRenameCTRData[nReadRenameCTRPorts];			/* Read Ports (CTR rename buffers): rename buffer value */
	
	sc_in<UInt32> inCR;				/* CR value */
	sc_in<UInt32> inLR;				/* LR value */
	sc_in<UInt32> inCTR;			/* CTR value */
	
	/* To/From Integer Units */
	sc_in<bool> inIntegerBusy[nIntegerUnit];						/* true if the integer unit is busy */
	
	sc_out<bool> outIntegerDispatched[nIntegerUnit];				/* true if the integer unit is starting to execute the instruction */
	sc_out<IntegerOperation> outIntegerOperation[nIntegerUnit];		/* integer operation */
	sc_out<IntegerOperands> outIntegerOperands[nIntegerUnit];		/* operands */
	sc_out<IntegerTags> outIntegerTags[nIntegerUnit];				/* tags */
	sc_out<int> outIntegerBranchCounter[nIntegerUnit];				/* number of branch instructions that the instruction depends on */
#if defined(DEBUG) || defined(TRACE)
	sc_out<UInt32> outIntegerInstructionCounter[nIntegerUnit];		/* serial number of the instruction (debugging purposes) */
	sc_out<UInt32> outIntegerProgramCounter[nIntegerUnit];			/* instruction address (debugging purposes) */
	sc_out<UInt32> outIntegerInstruction[nIntegerUnit];				/* instruction word (debugging purposes) */
#endif
	/* To/From Floating Point Units */
	sc_in<bool> inFloatingPointBusy[nFloatingPointUnits];			/* true if the floating point unit is busy */
	sc_in<bool> inFloatingPointPipelineEmpty[nFloatingPointUnits];	/* true if the floating point pipeline is empty */
	
	sc_out<bool> outFloatingPointDispatched[nFloatingPointUnits];					/* true if the floating point unit is starting to execute the instruction */
	sc_out<FloatingPointOperation> outFloatingPointOperation[nFloatingPointUnits];	/* floating point operation */
	sc_out<FloatingPointOperands> outFloatingPointOperands[nFloatingPointUnits];	/* operands */
	sc_out<FloatingPointTags> outFloatingPointTags[nFloatingPointUnits];			/* tags */
	sc_out<int> outFloatingPointBranchCounter[nFloatingPointUnits];					/* number of branch instructions that the instruction depends on */
#if defined(DEBUG) || defined(TRACE)
	sc_out<UInt32> outFloatingPointInstructionCounter[nFloatingPointUnits];	/* serial number of the instruction (debugging purposes) */
	sc_out<UInt32> outFloatingPointProgramCounter[nFloatingPointUnits];		/* instruction address (debugging purposes) */
	sc_out<UInt32> outFloatingPointInstruction[nFloatingPointUnits];		/* instruction word (debugging purposes) */
#endif	

	/* To/From load/store unit */
	sc_in<bool> inLoadStoreBusy;			/* true if the load/store unit is busy */
	sc_in<bool> inStoreQueueStalled;		/* true if no more store instruction can be issued to the load/store unit */
	sc_in<bool> inLoadQueueStalled;			/* true if no more load instruction can be issued to the load/store unit */
	
	sc_out<bool> outLoadStoreDispatched;				/* true if the loat/store unit is starting to execute the instruction */
	sc_out<LoadStoreOperation> outLoadStoreOperation;	/* load/store operation */
	sc_out<LoadStoreOperands> outLoadStoreOperands;		/* operands */
	sc_out<LoadStoreTags> outLoadStoreTags;				/* tags */
	sc_out<int> outLoadStoreBranchCounter;				/* number of branch instructions that the instruction depends on */
#if defined(DEBUG) || defined(TRACE)
	sc_out<UInt32> outLoadStoreInstructionCounter;		/* serial number of the instruction (debugging purposes) */
	sc_out<UInt32> outLoadStoreProgramCounter;			/* instruction address (debugging purposes) */
	sc_out<UInt32> outLoadStoreInstruction;				/* instruction word (debugging purposes) */
#endif	

	/* To/From Branch Unit */
	sc_in<bool> inBranchBusy;							/* true if the branch unit is busy */
	
	sc_out<bool> outBranchDispatched;					/* true if the branch unit is starting to execute the instruction */
	sc_out<BranchOperation> outBranchOperation;			/* branch operation */
	sc_out<BranchOperands> outBranchOperands;			/* operands */
	sc_out<BranchTags> outBranchTags;					/* tags */
	
	sc_in<bool> inBranchFinished;						/* true if a branch instruction has just finished */
	sc_in<bool> inMispredictedBranch;					/* true if the branch behaviour has been mispredicted */
#if defined(DEBUG) || defined(TRACE)
	sc_out<UInt32> outBranchInstructionCounter;			/* serial number of the instruction (debugging purposes) */			
	sc_out<UInt32> outBranchInstruction;				/* instruction word (debugging purposes) */
#endif
	
	/* System Register Unit */
	sc_in<bool> inSystemRegisterBusy;							/* true if system register unit is busy */
	sc_out<bool> outSystemRegisterDispatched;					/* true if the system register unit is starting to execute the instruction */
	sc_out<SystemRegisterOperation> outSystemRegisterOperation;	/* system register unit operation */
	sc_out<SystemRegisterTags> outSystemRegisterTags;			/* tags */
	sc_out<int> outSystemRegisterBranchCounter;					/* number of branch instructions that the instruction depends on */
#if defined(DEBUG) || defined(TRACE)
	sc_out<UInt32> outSystemRegisterInstructionCounter;			/* serial number of the instruction (debugging purposes) */
	sc_out<UInt32> outSystemRegisterProgramCounter;				/* instruction address (debugging purposes) */
	sc_out<UInt32> outSystemRegisterInstruction;				/* instruction word (debugging purposes) */
#endif
		
	sc_in<bool> inSystemRegisterFinished;						/* true if a system register unit instruction has just finished (so the interlock bit can be reseted) */
	
	/* Integer Units results (forwarded) */
	sc_in<bool> inIntegerResultValid[nIntegerUnit];				/* true if the integer unit produce a result */
	sc_in<tag_t> inIntegerResultTag[nIntegerUnit];				/* tag of the result */
	sc_in<UInt32> inIntegerResultData[nIntegerUnit];			/* value of the result */
	
	sc_in<bool> inIntegerResultCRValid[nIntegerUnit];			/* true if the integer unit produce a CR field */
	sc_in<tag_t> inIntegerResultCRTag[nIntegerUnit];			/* tag of the CR field */
	sc_in<UInt8> inIntegerResultCRData[nIntegerUnit];			/* value of the CR field */
	
	/* Floating Point Units results (forwarded) */
	sc_in<bool> inFloatingPointResultValid[nFloatingPointUnits];	/* true if the floating point unit produce a result */
	sc_in<tag_t> inFloatingPointResultTag[nFloatingPointUnits];		/* tag of the result */
	sc_in<UInt64> inFloatingPointResultData[nFloatingPointUnits];	/* value of the result */
	
	sc_in<bool> inFloatingPointResultCRValid[nFloatingPointUnits];	/* true if the floating point unit produce a CR field */
	sc_in<tag_t> inFloatingPointResultCRTag[nFloatingPointUnits];	/* tag of the CR field */
	sc_in<UInt8> inFloatingPointResultCRData[nFloatingPointUnits];	/* value of the CR field */
	
	/* Load/Store Unit results (forwarded) */
	sc_in<bool> inLoadStoreResultValid[2];							/* true if the load/store unit produce an integer result */
	sc_in<bool> inLoadStoreFloatingPointResultValid;				/* true if the load/store unit produce a floating point result */
	sc_in<tag_t> inLoadStoreResultTag[2];							/* tag of the result (first is for the value read from memory, second is for the EA) */
	sc_in<UInt32> inLoadStoreResultData[2];							/* value of the result (first is the value read from memory, second is the EA) */
	sc_in<UInt64> inLoadStoreFloatingPointResultData;				/* value of the floating point result (read from memory) */
	
	/* Branch Unit results (forwarded) */
	sc_in<bool> inBranchResultLRValid;								/* true if branch unit produce a LR value */
	sc_in<tag_t> inBranchResultLRTag;								/* tag of the LR value */
	sc_in<UInt32> inBranchResultLRData;								/* the LR value */
	
	sc_in<bool> inBranchResultCTRValid;								/* true if branch unit produce a CTR value */
	sc_in<tag_t> inBranchResultCTRTag;								/* tag of the CTR value */
	sc_in<UInt32> inBranchResultCTRData;							/* the CTR value */
	
	/* System Register Unit results */
	sc_in<bool> inSystemRegisterResultValid;						/* true if system register unit produce a result */
	sc_in<tag_t> inSystemRegisterResultTag;							/* tag of the result */
	sc_in<UInt32> inSystemRegisterResultData;						/* value of the result */
	
	sc_in<bool> inSystemRegisterResultLRValid;						/* true if system register unit produce a LR value */
	sc_in<tag_t> inSystemRegisterResultLRTag;						/* tag of the LR value */
	sc_in<UInt32> inSystemRegisterResultLRData;						/* the LR value */
	
	sc_in<bool> inSystemRegisterResultCTRValid;						/* true if system register unit produce a CTR value */
	sc_in<tag_t> inSystemRegisterResultCTRTag;						/* tag of the CTR value */
	sc_in<UInt32> inSystemRegisterResultCTRData;					/* the CTR value */
	
	sc_in<bool> inSystemRegisterResultCRValid;						/* true if system register unit produce a CR field */
	sc_in<tag_t> inSystemRegisterResultCRTag;						/* tag of the CR field */
	sc_in<UInt8> inSystemRegisterResultCRData;						/* value of the CR field */
	
	/* From Completion Unit */
	sc_in<bool> inIntegerExecute[nIntegerUnit];						/* true if an integer execution serialized instruction is the oldest into the processor */
	sc_in<tag_t> inIntegerExecuteTag[nIntegerUnit];					/* tag of the instruction */
	sc_in<bool> inFloatingPointExecute[nFloatingPointUnits];		/* true if a floating point execution serialized instruction is the oldest into the processor */
	sc_in<tag_t> inFloatingPointExecuteTag[nFloatingPointUnits];	/* tag of the instruction */
	sc_in<bool> inSystemRegisterExecute;							/* true if a system register unit execution serialized instruction is the oldest into the processor */
	sc_in<tag_t> inSystemRegisterExecuteTag;						/* tag of the instruction */
	
	sc_in<bool> inClearInterlock;									/* true if the interlock bit hqs to be cleared */
	
	/* From Data Cache */
	sc_in<bool> inDCacheBusy;										/* true if data cache is not busy */
	
	/* From Load/Store Unit */
	sc_in<bool> inLoadStoreEmpty;									/* true if the load/store unit is empty (no being executing a load/store instruction) */
	
	/* To Completion Unit */
	sc_out<Dummy<bool> > outFinish;									/* To wake up the finish process */
	
	/* From Completion Unit */
	sc_in<Dummy<bool> > inDecode;									/* To wake up the decode process */

	
	
	
	
	
	
	/* Internal variables */

	int nDecodedInstructions;				/* Number of instructions decoded by the decode process */
	int nInstructionsToDispatch;			/* Number of instructions to dispatch */
	bool interlock;							/* Interlock bit. If it set no more instructions are decoded until it is cleared */
	
	/* Reservation stations */
	IntegerRSQueue integerRSQueue[nIntegerUnit];
	LoadStoreRSQueue loadStoreRSQueue;
	BranchRSQueue branchRSQueue;
	SystemRegisterRSQueue systemRegisterRSQueue;
	FloatingPointRSQueue floatingPointRSQueue[nFloatingPointUnits];
	
	/* if a value is being read from register file then it is >= 0 (this is the register or rename buffer number) else it is -1 */
	int integerReadingRegisters[nIQ + nIQBranchs][2];
	int floatingPointReadingFloatingPointRegisters[nIQ + nIQBranchs][3];
	int loadStoreReadingRegisters[nIQ + nIQBranchs][3];
	int loadStoreReadingFloatingPointRegister[nIQ + nIQBranchs];
	int integerReadingRenameBuffers[nIQ + nIQBranchs][2];
	int floatingPointReadingFloatingPointRenameBuffers[nIQ + nIQBranchs][3];
	int branchReadingRenameCR[nIQ + nIQBranchs];
	int branchReadingCR[nIQ + nIQBranchs];
	int loadStoreReadingRenameBuffers[nIQ + nIQBranchs][3];
	int loadStoreReadingFloatingPointRenameBuffer[nIQ + nIQBranchs];
	int branchReadingLR[nIQ + nIQBranchs];
	int branchReadingRenameLR[nIQ + nIQBranchs];
	int branchReadingCTR[nIQ + nIQBranchs];
	int branchReadingRenameCTR[nIQ + nIQBranchs];
	
	/* Reservation stations corresponding to the last decoded instructions */
	union
	{
		IntegerRS *integer;
		LoadStoreRS *loadStore;
		BranchRS *branch;
		SystemRegisterRS *systemRegister;
		FloatingPointRS *floatingPoint;
	} rs[nIQ + nIQBranchs];
	
	DecodedInstruction decodedInstructions[nIQ + nIQBranchs];	/* decoded instructions */
	UnitIdent units[nIQ + nIQBranchs];	/* unit ident of the last decoded instructions */
	int branchCounter;					/* number of branch instructions that the current instruction bein decoded depends on */
	RegistersBinding *registersBinding;	/* Registers binding which defines how registers have been renamed */
	ReorderBuffer *rob;					/* Reorder buffer */
	bool branchFollow;
	
	void Decode();
	void Dispatch();
	
	
	void DecodeInstruction(UInt32 instructionWord, DecodedInstruction& decodedInstruction);
	int GetLessLoadedIntegerRSQueue(int ic);
	int GetLessLoadedFloatingPointRSQueue();
	IntegerRS *AllocateIntegerRS(int ic);
	FloatingPointRS *AllocateFloatingPointRS();
	LoadStoreRS *AllocateLoadStoreRS();
	BranchRS *AllocateBranchRS();
	SystemRegisterRS *AllocateSystemRegisterRS();
	int AllocateRS(int i);
	void UpdateRS();
	void DispatchInstructions();
	int CheckResources(int nInstructions);
	void Flush();
	void DecrementBranchCounter();
	
	DispatchUnit(const sc_module_name& name, ReorderBuffer *rob, RegistersBinding *registersBinding);
	
	void Reset();
	void FlushAll();
};

#endif
