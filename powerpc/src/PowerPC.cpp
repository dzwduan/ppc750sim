/***************************************************************************
                          PowerPC.cpp  -  description
                             -------------------
    begin                : Thu Apr 19 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <stdlib.h>
#include <systemc.h>
#include <PowerPC.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ppcemul.h>
#include <signal.h>

PowerPC *PowerPC::powerpc = 0;

PowerPC::PowerPC(const sc_module_name& name)
{
	PowerPC::powerpc = this;
	Build();
	Reset();
}

void PowerPC::Build()
{
	int i;
	
	registersBinding = new RegistersBinding();
	reorderBuffer = new ReorderBuffer();
	

	icache = new ICache("ICache");
	fetchUnit = new FetchUnit("FetchUnit");
	dispatchUnit = new DispatchUnit("DispatchUnit", reorderBuffer, registersBinding);
	
	for(i = 0; i < nIntegerUnit; i++)
	{
		char name[256];
		
		sprintf(name, "IntegerUnit%d", i);
		integerUnit[i] = new IntegerUnit(name);
		integerUnit[i]->inClock(inClock);
	}
	
	for(i = 0; i < nFloatingPointUnits; i++)
	{
		char name[256];
		sprintf(name, "FloatingPointUnit%d", i);
		floatingPointUnit[i] = new FloatingPointUnit(name);
		floatingPointUnit[i]->inClock(inClock);
	}
	
	loadStoreUnit = new LoadStoreUnit("LoadStoreUnit");
	completionUnit = new CompletionUnit("CompletionUnit", reorderBuffer, registersBinding);
	registerFile = new RegisterFile("RegisterFile");
	dcache = new DCache("DCache");
	busInterfaceUnit = new BusInterfaceUnit("BusInterfaceUnit");
	bpu = new BranchPredictionUnit("BranchPredictionUnit");
	branchUnit = new BranchUnit("BranchUnit");
	conditionRegister = new ConditionRegister("ConditionRegister");
	linkRegister = new LinkRegister("LinkRegister");
	countRegister = new CountRegister("CountRegister");
	xerRegister = new XERRegister("XERRegister");
	systemRegisterUnit = new SystemRegisterUnit("SystemRegisterUnit");
	
	icache->inClock(inClock);
	fetchUnit->inClock(inClock);
	dispatchUnit->inClock(inClock);
	loadStoreUnit->inClock(inClock);
	completionUnit->inClock(inClock);
	registerFile->inClock(inClock);
	dcache->inClock(inClock);
	busInterfaceUnit->inClock(inClock);
	bpu->inClock(inClock);
	branchUnit->inClock(inClock);
	conditionRegister->inClock(inClock);
	linkRegister->inClock(inClock);
	countRegister->inClock(inClock);
	xerRegister->inClock(inClock);
	systemRegisterUnit->inClock(inClock);

	fetchUnit->outICacheCancel(icacheCancel);
	fetchUnit->outICacheReq(fetch2icacheReq);
	fetchUnit->outICacheAddr(fetch2icacheAddr);
	fetchUnit->outICacheSize(fetch2icacheSize);
	icache->inCancel(icacheCancel);
	icache->inReq(fetch2icacheReq);
	icache->inAddr(fetch2icacheAddr);
	icache->inSize(fetch2icacheSize);
	icache->outAck(icache2fetchAck);
	icache->outData(icache2fetchReadData);
	icache->outHit(icache2fetchHit);
	fetchUnit->inICacheAck(icache2fetchAck);
	fetchUnit->inICacheData(icache2fetchReadData);
	fetchUnit->inICacheHit(icache2fetchHit);
	
	icache->outMemReq(icache2biuReq);
	icache->outMemAddr(icache2biuAddr);
	busInterfaceUnit->inICacheReq(icache2biuReq);
	busInterfaceUnit->inICacheAddr(icache2biuAddr);
	busInterfaceUnit->outICacheData(biu2icacheReadData);
	busInterfaceUnit->outICacheAck(biu2icacheAck);
	icache->inMemData(biu2icacheReadData);
	icache->inMemAck(biu2icacheAck);
	
	fetchUnit->outReadBHTPC(readBHTPC);
	fetchUnit->outReadBTICPC(readBTICPC);
	bpu->inReadBHTPC(readBHTPC);
	bpu->inReadBTICPC(readBTICPC);
	
	bpu->outReadBHTTakenBranch(readBHTTakenBranch);
	fetchUnit->inReadBHTTakenBranch(readBHTTakenBranch);
	
	bpu->outReadBTICHit(readBTICHit);
	bpu->outReadBTICInstructionsCount(readBTICInstructionsCount);
	
	for(i = 0; i < nInstructionsPerBTICEntry; i++)
	{
		bpu->outReadBTICInstructions[i](readBTICInstructions[i]);
		fetchUnit->inReadBTICInstructions[i](readBTICInstructions[i]);
	}
	
	fetchUnit->inReadBTICHit(readBTICHit);
	fetchUnit->inReadBTICInstructionsCount(readBTICInstructionsCount);
	
	fetchUnit->outUpdateBTIC(updateBTIC);
	fetchUnit->outUpdateBTICPC(updateBTICPC);
	fetchUnit->outUpdateBTICInstructionsCount(updateBTICInstructionsCount);
	
	bpu->inUpdateBTIC(updateBTIC);
	bpu->inUpdateBTICPC(updateBTICPC);
	bpu->inUpdateBTICInstructionsCount(updateBTICInstructionsCount);
	for(i = 0; i < nInstructionsPerBTICEntry; i++)
	{
		fetchUnit->outUpdateBTICInstructions[i](updateBTICInstructions[i]);
		bpu->inUpdateBTICInstructions[i](updateBTICInstructions[i]);
	}

	for(i = 0; i < nIQ; i++)
	{
		fetchUnit->outIQ[i](instructions[i]);
		dispatchUnit->inIQ[i](instructions[i]);
	}
	for(i = 0; i < nBranchQueueEntries; i++)
	{
		fetchUnit->outBranchQueue[i](branchs[i]);
		dispatchUnit->inBranchQueue[i](branchs[i]);
	}
	
	fetchUnit->outInstructionsInQueue(instructionsInQueue);
	dispatchUnit->inInstructionsInQueue(instructionsInQueue);
	fetchUnit->outBranchsInQueue(branchsInQueue);
	dispatchUnit->inBranchsInQueue(branchsInQueue);
	dispatchUnit->outDispatchedInstructions(dispatchedInstructions);
	dispatchUnit->outDispatchedBranchs(dispatchedBranchs);
	fetchUnit->inDispatchedInstructions(dispatchedInstructions);
	fetchUnit->inDispatchedBranchs(dispatchedBranchs);
	
	/* One register file read port is reserved for the system register unit */
	for(i = 0; i < nReadRegisterPorts - 1; i++)
	{
		dispatchUnit->outReadRegisterNumber[i](readRegisterNumber[i]);
		registerFile->inReadRegisterNumber[i](readRegisterNumber[i]);
		registerFile->outReadRegisterData[i](readRegisterData[i]);
		dispatchUnit->inReadRegisterData[i](readRegisterData[i]);
	}
	
	for(i = 0; i < nReadFloatingPointRegisterPorts; i++)
	{
		dispatchUnit->outReadFloatingPointRegisterNumber[i](readFloatingPointRegisterNumber[i]);
		registerFile->inReadFloatingPointRegisterNumber[i](readFloatingPointRegisterNumber[i]);
		registerFile->outReadFloatingPointRegisterData[i](readFloatingPointRegisterData[i]);
		dispatchUnit->inReadFloatingPointRegisterData[i](readFloatingPointRegisterData[i]);
	}
	
	systemRegisterUnit->outReadRegisterNumber(readRegisterNumber[nReadRegisterPorts - 1]);
	registerFile->inReadRegisterNumber[nReadRegisterPorts - 1](readRegisterNumber[nReadRegisterPorts - 1]);
	registerFile->outReadRegisterData[nReadRegisterPorts - 1](readRegisterData[nReadRegisterPorts - 1]);
	systemRegisterUnit->inReadRegisterData(readRegisterData[nReadRegisterPorts - 1]);
	
	for(i = 0; i < nReadRenamePorts; i++)
	{
		dispatchUnit->outReadRenameNumber[i](readRenameNumber[i]);
		registerFile->inReadRenameNumber[i](readRenameNumber[i]);
		registerFile->outReadRenameData[i](readRenameData[i]);
		dispatchUnit->inReadRenameData[i](readRenameData[i]);
	}
	
	for(i = 0; i < nReadFloatingPointRenamePorts; i++)
	{
		dispatchUnit->outReadFloatingPointRenameNumber[i](readFloatingPointRenameNumber[i]);
		registerFile->inReadFloatingPointRenameNumber[i](readFloatingPointRenameNumber[i]);
		registerFile->outReadFloatingPointRenameData[i](readFloatingPointRenameData[i]);
		dispatchUnit->inReadFloatingPointRenameData[i](readFloatingPointRenameData[i]);
	}
	
	for(i = 0; i < nReadRenameCRPorts; i++)
	{
		dispatchUnit->outReadRenameCRNumber[i](readRenameCRNumber[i]);
		conditionRegister->inReadRenameCRNumber[i](readRenameCRNumber[i]);
		conditionRegister->outReadRenameCRData[i](readRenameCRData[i]);
		dispatchUnit->inReadRenameCRData[i](readRenameCRData[i]);
	}
	
	for(i = 0; i < nReadRenameLRPorts; i++)
	{
		dispatchUnit->outReadRenameLRNumber[i](readRenameLRNumber[i]);
		linkRegister->inReadRenameLRNumber[i](readRenameLRNumber[i]);
		linkRegister->outReadRenameLRData[i](readRenameLRData[i]);
		dispatchUnit->inReadRenameLRData[i](readRenameLRData[i]);
	}
	
	for(i = 0; i < nReadRenameCTRPorts; i++)
	{
		dispatchUnit->outReadRenameCTRNumber[i](readRenameCTRNumber[i]);
		countRegister->inReadRenameCTRNumber[i](readRenameCTRNumber[i]);
		countRegister->outReadRenameCTRData[i](readRenameCTRData[i]);
		dispatchUnit->inReadRenameCTRData[i](readRenameCTRData[i]);
	}	
	
	/* Integer unit  */
	for(i = 0; i < nIntegerUnit; i++)
	{
		integerUnit[i]->outBusy(integerBusy[i]);
		dispatchUnit->inIntegerBusy[i](integerBusy[i]);
		dispatchUnit->outIntegerDispatched[i](integerDispatched[i]);
		dispatchUnit->outIntegerOperation[i](integerOperation[i]);
		dispatchUnit->outIntegerOperands[i](integerOperands[i]);
		dispatchUnit->outIntegerTags[i](integerTags[i]);
		dispatchUnit->outIntegerBranchCounter[i](integerBranchCounter[i]);
		completionUnit->outIntegerExecute[i](integerExecute[i]);
		completionUnit->outIntegerExecuteTag[i](integerExecuteTag[i]);
		dispatchUnit->inIntegerExecute[i](integerExecute[i]);
		dispatchUnit->inIntegerExecuteTag[i](integerExecuteTag[i]);
		integerUnit[i]->inDispatched(integerDispatched[i]);
		integerUnit[i]->inOperation(integerOperation[i]);
		integerUnit[i]->inOperands(integerOperands[i]);
		integerUnit[i]->inTags(integerTags[i]);
		integerUnit[i]->inCarry(carry);
		integerUnit[i]->inBranchFinished(branchFinished);
		integerUnit[i]->inMispredictedBranch(mispredictedBranch);
		integerUnit[i]->inBranchCounter(integerBranchCounter[i]);
#if defined(DEBUG) || defined(TRACE)
		dispatchUnit->outIntegerInstructionCounter[i](integerInstructionCounter[i]);
		dispatchUnit->outIntegerProgramCounter[i](integerProgramCounter[i]);
		dispatchUnit->outIntegerInstruction[i](integerInstruction[i]);
		integerUnit[i]->inInstructionCounter(integerInstructionCounter[i]);
		integerUnit[i]->inProgramCounter(integerProgramCounter[i]);
		integerUnit[i]->inInstruction(integerInstruction[i]);
#endif
	}
		
	/* Floating point unit */
	for(i = 0; i < nFloatingPointUnits; i++)
	{
		floatingPointUnit[i]->outBusy(floatingPointBusy[i]);
		floatingPointUnit[i]->outFloatingPointPipelineEmpty(floatingPointPipelineEmpty[i]);
		dispatchUnit->inFloatingPointBusy[i](floatingPointBusy[i]);
		dispatchUnit->inFloatingPointPipelineEmpty[i](floatingPointPipelineEmpty[i]);
		dispatchUnit->outFloatingPointDispatched[i](floatingPointDispatched[i]);
		dispatchUnit->outFloatingPointOperation[i](floatingPointOperation[i]);
		dispatchUnit->outFloatingPointOperands[i](floatingPointOperands[i]);
		dispatchUnit->outFloatingPointTags[i](floatingPointTags[i]);
		dispatchUnit->outFloatingPointBranchCounter[i](floatingPointBranchCounter[i]);
		completionUnit->outFloatingPointExecute[i](floatingPointExecute[i]);
		completionUnit->outFloatingPointExecuteTag[i](floatingPointExecuteTag[i]);
		dispatchUnit->inFloatingPointExecute[i](floatingPointExecute[i]);
		dispatchUnit->inFloatingPointExecuteTag[i](floatingPointExecuteTag[i]);
		floatingPointUnit[i]->inDispatched(floatingPointDispatched[i]);
		floatingPointUnit[i]->inOperation(floatingPointOperation[i]);
		floatingPointUnit[i]->inOperands(floatingPointOperands[i]);
		floatingPointUnit[i]->inTags(floatingPointTags[i]);
		floatingPointUnit[i]->inBranchFinished(branchFinished);
		floatingPointUnit[i]->inMispredictedBranch(mispredictedBranch);
		floatingPointUnit[i]->inBranchCounter(floatingPointBranchCounter[i]);
#if defined(DEBUG) || defined(TRACE)
		dispatchUnit->outFloatingPointInstructionCounter[i](floatingPointInstructionCounter[i]);
		dispatchUnit->outFloatingPointProgramCounter[i](floatingPointProgramCounter[i]);
		dispatchUnit->outFloatingPointInstruction[i](floatingPointInstruction[i]);
		floatingPointUnit[i]->inInstructionCounter(floatingPointInstructionCounter[i]);
		floatingPointUnit[i]->inProgramCounter(floatingPointProgramCounter[i]);
		floatingPointUnit[i]->inInstruction(floatingPointInstruction[i]);
#endif
	}
	
	/* load/store unit */
	loadStoreUnit->outBusy(loadStoreBusy);
	dispatchUnit->inLoadStoreBusy(loadStoreBusy);
	dispatchUnit->outLoadStoreDispatched(loadStoreDispatched);
	dispatchUnit->outLoadStoreOperation(loadStoreOperation);
	dispatchUnit->outLoadStoreOperands(loadStoreOperands);
	dispatchUnit->outLoadStoreTags(loadStoreTags);
	dispatchUnit->outLoadStoreBranchCounter(loadStoreBranchCounter);
	loadStoreUnit->inDispatched(loadStoreDispatched);
	loadStoreUnit->inOperation(loadStoreOperation);
	loadStoreUnit->inOperands(loadStoreOperands);
	loadStoreUnit->inTags(loadStoreTags);
	loadStoreUnit->inBranchCounter(loadStoreBranchCounter);
	loadStoreUnit->inBranchFinished(branchFinished);
	loadStoreUnit->inMispredictedBranch(mispredictedBranch);
	
	loadStoreUnit->outStoreQueueStalled(storeQueueStalled);
	dispatchUnit->inStoreQueueStalled(storeQueueStalled);
	loadStoreUnit->outLoadQueueStalled(loadQueueStalled);
	dispatchUnit->inLoadQueueStalled(loadQueueStalled);
	loadStoreUnit->outEmpty(loadStoreEmpty);
	dispatchUnit->inLoadStoreEmpty(loadStoreEmpty);
	
#if defined(DEBUG) || defined(TRACE)
	dispatchUnit->outLoadStoreInstructionCounter(loadStoreInstructionCounter);
	dispatchUnit->outLoadStoreProgramCounter(loadStoreProgramCounter);
	dispatchUnit->outLoadStoreInstruction(loadStoreInstruction);
	loadStoreUnit->inInstructionCounter(loadStoreInstructionCounter);
	loadStoreUnit->inProgramCounter(loadStoreProgramCounter);
	loadStoreUnit->inInstruction(loadStoreInstruction);
#endif
	/* branch unit */
	dispatchUnit->outBranchDispatched(branchDispatched);
	dispatchUnit->outBranchOperation(branchOperation);
	dispatchUnit->outBranchOperands(branchOperands);
	dispatchUnit->outBranchTags(branchTags);
	branchUnit->inDispatched(branchDispatched);
	branchUnit->inOperation(branchOperation);
	branchUnit->inOperands(branchOperands);
	branchUnit->inTags(branchTags);
	
	branchUnit->outBusy(branchBusy);
	dispatchUnit->inBranchBusy(branchBusy);
	
#if defined(DEBUG) || defined(TRACE)
	dispatchUnit->outBranchInstructionCounter(branchInstructionCounter);
	dispatchUnit->outBranchInstruction(branchInstruction);
	branchUnit->inInstructionCounter(branchInstructionCounter);
	branchUnit->inInstruction(branchInstruction);
#endif
	/* system register unit */
	dispatchUnit->outSystemRegisterDispatched(systemRegisterDispatched);
	dispatchUnit->outSystemRegisterOperation(systemRegisterOperation);
	dispatchUnit->outSystemRegisterTags(systemRegisterTags);	
	dispatchUnit->outSystemRegisterBranchCounter(systemRegisterBranchCounter);
	dispatchUnit->inSystemRegisterBusy(systemRegisterBusy);
	completionUnit->outSystemRegisterExecute(systemRegisterExecute);
	completionUnit->outSystemRegisterExecuteTag(systemRegisterExecuteTag);
	dispatchUnit->inSystemRegisterExecute(systemRegisterExecute);
	dispatchUnit->inSystemRegisterExecuteTag(systemRegisterExecuteTag);
	systemRegisterUnit->inDispatched(systemRegisterDispatched);
	systemRegisterUnit->inOperation(systemRegisterOperation);
	systemRegisterUnit->inTags(systemRegisterTags);	
	systemRegisterUnit->outBusy(systemRegisterBusy);
	systemRegisterUnit->inBranchFinished(branchFinished);
	systemRegisterUnit->inMispredictedBranch(mispredictedBranch);
	systemRegisterUnit->inBranchCounter(systemRegisterBranchCounter);
	
#if defined(DEBUG) || defined(TRACE)
	dispatchUnit->outSystemRegisterInstructionCounter(systemRegisterInstructionCounter);
	dispatchUnit->outSystemRegisterProgramCounter(systemRegisterProgramCounter);
	dispatchUnit->outSystemRegisterInstruction(systemRegisterInstruction);
	systemRegisterUnit->inInstructionCounter(systemRegisterInstructionCounter);
	systemRegisterUnit->inProgramCounter(systemRegisterProgramCounter);
	systemRegisterUnit->inInstruction(systemRegisterInstruction);
#endif
	/* L1 Data cache */
	loadStoreUnit->outDCacheReq(lsu2dcacheReq);
	loadStoreUnit->outDCacheData(lsu2dcacheWriteData);
	loadStoreUnit->outDCacheWrite(lsu2dcacheWrite);
	loadStoreUnit->outDCacheAddr(lsu2dcacheAddr);
	loadStoreUnit->outDCacheSize(lsu2dcacheSize);
	loadStoreUnit->outDCacheZeroBlock(lsu2dcacheZeroBlock);
	dcache->inReq(lsu2dcacheReq);
	dcache->inData(lsu2dcacheWriteData);
	dcache->inWrite(lsu2dcacheWrite);
	dcache->inAddr(lsu2dcacheAddr);
	dcache->inSize(lsu2dcacheSize);
	dcache->inZeroBlock(lsu2dcacheZeroBlock);
	dcache->outAck(dcache2lsuAck);
	dcache->outData(dcache2lsuReadData);
	dcache->outHit(dcache2lsuHit);
	loadStoreUnit->inDCacheAck(dcache2lsuAck);
	loadStoreUnit->inDCacheData(dcache2lsuReadData);
	loadStoreUnit->inDCacheHit(dcache2lsuHit);
	
	dcache->outBusy(dcacheBusy);
	dispatchUnit->inDCacheBusy(dcacheBusy);
	
	/* Bus interface */	
	dcache->outMemReq(dcache2biuReq);
	dcache->outMemWrite(dcache2biuWrite);
	dcache->outMemAddr(dcache2biuAddr);
	dcache->outMemData(dcache2biuWriteData);
	busInterfaceUnit->inDCacheReq(dcache2biuReq);
	busInterfaceUnit->inDCacheWrite(dcache2biuWrite);
	busInterfaceUnit->inDCacheAddr(dcache2biuAddr);
	busInterfaceUnit->inDCacheData(dcache2biuWriteData);
	busInterfaceUnit->outDCacheAck(biu2dcacheAck);
	busInterfaceUnit->outDCacheData(biu2dcacheReadData);
	dcache->inMemAck(biu2dcacheAck);
	dcache->inMemData(biu2dcacheReadData);
	
	/* Integer unit results */
	for(i = 0; i < nIntegerUnit; i++)
	{
		integerUnit[i]->outFinished(integerFinished[i]);
		integerUnit[i]->outTag(integerTag[i]);
		
		integerUnit[i]->outResultValid(integerResultValid[i]);
		integerUnit[i]->outResultTag(integerResultTag[i]);
		integerUnit[i]->outResultData(integerResultData[i]);
		
		integerUnit[i]->outResultCRValid(integerResultCRValid[i]);
		integerUnit[i]->outResultCRTag(integerResultCRTag[i]);
		integerUnit[i]->outResultCRData(integerResultCRData[i]);
		
		integerUnit[i]->outOverflow(integerOverflow[i]);
		integerUnit[i]->outCarry(integerCarry[i]);
			
		completionUnit->inIntegerFinished[i](integerFinished[i]);
		completionUnit->inIntegerTag[i](integerTag[i]);
		completionUnit->inIntegerOverflow[i](integerOverflow[i]);
		completionUnit->inIntegerCarry[i](integerCarry[i]);
		completionUnit->inIntegerResultValid[i](integerResultValid[i]);
		completionUnit->inIntegerResultTag[i](integerResultTag[i]);
		completionUnit->inIntegerResultCRValid[i](integerResultCRValid[i]);
		completionUnit->inIntegerResultCRTag[i](integerResultCRTag[i]);
		
		dispatchUnit->inIntegerResultValid[i](integerResultValid[i]);
		dispatchUnit->inIntegerResultTag[i](integerResultTag[i]);
		dispatchUnit->inIntegerResultData[i](integerResultData[i]);
		
		dispatchUnit->inIntegerResultCRValid[i](integerResultCRValid[i]);
		dispatchUnit->inIntegerResultCRTag[i](integerResultCRTag[i]);
		dispatchUnit->inIntegerResultCRData[i](integerResultCRData[i]);
		
		registerFile->inWriteRename[IntegerWriteRenamePort + i](integerResultValid[i]);
		registerFile->inWriteRenameNumber[IntegerWriteRenamePort + i](integerResultTag[i]);
		registerFile->inWriteRenameData[IntegerWriteRenamePort + i](integerResultData[i]);
		
		conditionRegister->inWriteRenameCR[IntegerWriteRenameCRPort + i](integerResultCRValid[i]);
		conditionRegister->inWriteRenameCRNumber[IntegerWriteRenameCRPort + i](integerResultCRTag[i]);
		conditionRegister->inWriteRenameCRData[IntegerWriteRenameCRPort + i](integerResultCRData[i]);
	}
	
	/* Floating point unit results */
	for(i = 0; i < nFloatingPointUnits; i++)
	{
		floatingPointUnit[i]->outFinished(floatingPointFinished[i]);
		floatingPointUnit[i]->outTag(floatingPointTag[i]);
		
		floatingPointUnit[i]->outResultValid(floatingPointResultValid[i]);
		floatingPointUnit[i]->outResultTag(floatingPointResultTag[i]);
		floatingPointUnit[i]->outResultData(floatingPointResultData[i]);
		
		floatingPointUnit[i]->outResultCRValid(floatingPointResultCRValid[i]);
		floatingPointUnit[i]->outResultCRTag(floatingPointResultCRTag[i]);
		floatingPointUnit[i]->outResultCRData(floatingPointResultCRData[i]);
		
		completionUnit->inFloatingPointFinished[i](floatingPointFinished[i]);
		completionUnit->inFloatingPointTag[i](floatingPointTag[i]);
		completionUnit->inFloatingPointResultValid[i](floatingPointResultValid[i]);
		completionUnit->inFloatingPointResultTag[i](floatingPointResultTag[i]);
		completionUnit->inFloatingPointResultCRValid[i](floatingPointResultCRValid[i]);
		completionUnit->inFloatingPointResultCRTag[i](floatingPointResultCRTag[i]);
		
		dispatchUnit->inFloatingPointResultValid[i](floatingPointResultValid[i]);
		dispatchUnit->inFloatingPointResultTag[i](floatingPointResultTag[i]);
		dispatchUnit->inFloatingPointResultData[i](floatingPointResultData[i]);
		
		dispatchUnit->inFloatingPointResultCRValid[i](floatingPointResultCRValid[i]);
		dispatchUnit->inFloatingPointResultCRTag[i](floatingPointResultCRTag[i]);
		dispatchUnit->inFloatingPointResultCRData[i](floatingPointResultCRData[i]);
		
		registerFile->inWriteFloatingPointRename[FloatingPointWriteFloatingPointRenamePort + i](floatingPointResultValid[i]);
		registerFile->inWriteFloatingPointRenameNumber[FloatingPointWriteFloatingPointRenamePort + i](floatingPointResultTag[i]);
		registerFile->inWriteFloatingPointRenameData[FloatingPointWriteFloatingPointRenamePort + i](floatingPointResultData[i]);
		
		conditionRegister->inWriteRenameCR[FloatingPointWriteRenameCRPort + i](floatingPointResultCRValid[i]);
		conditionRegister->inWriteRenameCRNumber[FloatingPointWriteRenameCRPort + i](floatingPointResultCRTag[i]);
		conditionRegister->inWriteRenameCRData[FloatingPointWriteRenameCRPort + i](floatingPointResultCRData[i]);
	}
	
	/* load/Store unit results */
	for(i = 0; i < 2; i++)
	{
		loadStoreUnit->outFinished[i](loadStoreFinished[i]);
		loadStoreUnit->outTag[i](loadStoreTag[i]);
		completionUnit->inLoadStoreFinished[i](loadStoreFinished[i]);
		completionUnit->inLoadStoreTag[i](loadStoreTag[i]);
	}

	for(i = 0; i < 2; i++)
	{
		loadStoreUnit->outResultValid[i](loadStoreResultValid[i]);
		loadStoreUnit->outResultTag[i](loadStoreResultTag[i]);
		loadStoreUnit->outResultData[i](loadStoreResultData[i]);
		
		completionUnit->inLoadStoreResultValid[i](loadStoreResultValid[i]);
		completionUnit->inLoadStoreResultTag[i](loadStoreResultTag[i]);
		
		dispatchUnit->inLoadStoreResultValid[i](loadStoreResultValid[i]);
		dispatchUnit->inLoadStoreResultTag[i](loadStoreResultTag[i]);
		dispatchUnit->inLoadStoreResultData[i](loadStoreResultData[i]);
		
		registerFile->inWriteRename[LoadStoreWriteRenamePort + i](loadStoreResultValid[i]);
		registerFile->inWriteRenameNumber[LoadStoreWriteRenamePort + i](loadStoreResultTag[i]);
		registerFile->inWriteRenameData[LoadStoreWriteRenamePort + i](loadStoreResultData[i]);
	}
	
	loadStoreUnit->outFloatingPointResultValid(loadStoreFloatingPointResultValid);
	loadStoreUnit->outFloatingPointResultData(loadStoreFloatingPointResultData);
	
	completionUnit->inLoadStoreFloatingPointResultValid(loadStoreFloatingPointResultValid);
	
	dispatchUnit->inLoadStoreFloatingPointResultValid(loadStoreFloatingPointResultValid);
	dispatchUnit->inLoadStoreFloatingPointResultData(loadStoreFloatingPointResultData);
	
	registerFile->inWriteFloatingPointRename[LoadStoreWriteFloatingPointRenamePort](loadStoreFloatingPointResultValid);
	registerFile->inWriteFloatingPointRenameNumber[LoadStoreWriteFloatingPointRenamePort](loadStoreResultTag[0]);	
	registerFile->inWriteFloatingPointRenameData[LoadStoreWriteFloatingPointRenamePort](loadStoreFloatingPointResultData);
	
	/* Branch unit results */
	branchUnit->outResultLRValid(branchResultLRValid);
	branchUnit->outResultLRTag(branchResultLRTag);
	branchUnit->outResultLRData(branchResultLRData);
	
	branchUnit->outResultCTRValid(branchResultCTRValid);
	branchUnit->outResultCTRTag(branchResultCTRTag);
	branchUnit->outResultCTRData(branchResultCTRData);
	
	branchUnit->outMispredictedBranch(mispredictedBranch);
	branchUnit->outFinished(branchFinished);
	branchUnit->outTag(branchTag);
	branchUnit->outPC(branchPC);
	branchUnit->outUpdateBHT(updateBHT);
	branchUnit->outBHTPC(updateBHTPC);
	branchUnit->outTakenBranch(updateBHTTakenBranch);
	
	completionUnit->inBranchResultLRValid(branchResultLRValid);
	completionUnit->inBranchResultLRTag(branchResultLRTag);
	
	dispatchUnit->inBranchResultLRValid(branchResultLRValid);
	dispatchUnit->inBranchResultLRTag(branchResultLRTag);
	dispatchUnit->inBranchResultLRData(branchResultLRData);
	
	completionUnit->inBranchResultCTRValid(branchResultCTRValid);
	completionUnit->inBranchResultCTRTag(branchResultCTRTag);
	
	dispatchUnit->inBranchResultCTRValid(branchResultCTRValid);
	dispatchUnit->inBranchResultCTRTag(branchResultCTRTag);
	dispatchUnit->inBranchResultCTRData(branchResultCTRData);
	
	linkRegister->inWriteRenameLR[BranchUnitWriteRenameLRPort](branchResultLRValid);
	linkRegister->inWriteRenameLRNumber[BranchUnitWriteRenameLRPort](branchResultLRTag);
	linkRegister->inWriteRenameLRData[BranchUnitWriteRenameLRPort](branchResultLRData);
	
	countRegister->inWriteRenameCTR[BranchUnitWriteRenameCTRPort](branchResultCTRValid);
	countRegister->inWriteRenameCTRNumber[BranchUnitWriteRenameCTRPort](branchResultCTRTag);
	countRegister->inWriteRenameCTRData[BranchUnitWriteRenameCTRPort](branchResultCTRData);
	
	fetchUnit->inBranchFinished(branchFinished);
	fetchUnit->inMispredictedBranch(mispredictedBranch);
	fetchUnit->inPC(branchPC);
	
	bpu->inUpdateBHT(updateBHT);
	bpu->inUpdateBHTPC(updateBHTPC);
	bpu->inUpdateBHTTakenBranch(updateBHTTakenBranch);
	
	completionUnit->inBranchFinished(branchFinished);
	completionUnit->inBranchTag(branchTag);
	completionUnit->inMispredictedBranch(mispredictedBranch);
	completionUnit->inBranchPC(branchPC);
	
	dispatchUnit->inBranchFinished(branchFinished);
	dispatchUnit->inMispredictedBranch(mispredictedBranch);
	
	/* System register unit results */
	systemRegisterUnit->outFinished(systemRegisterFinished);
	systemRegisterUnit->outTag(systemRegisterTag);
	systemRegisterUnit->outResultValid(systemRegisterResultValid);
	systemRegisterUnit->outResultTag(systemRegisterResultTag);
	systemRegisterUnit->outResultData(systemRegisterResultData);
	systemRegisterUnit->outResultLRValid(systemRegisterResultLRValid);
	systemRegisterUnit->outResultLRTag(systemRegisterResultLRTag);
	systemRegisterUnit->outResultLRData(systemRegisterResultLRData);
	systemRegisterUnit->outResultCTRValid(systemRegisterResultCTRValid);
	systemRegisterUnit->outResultCTRTag(systemRegisterResultCTRTag);
	systemRegisterUnit->outResultCTRData(systemRegisterResultCTRData);
	systemRegisterUnit->outResultCRValid(systemRegisterResultCRValid);
	systemRegisterUnit->outResultCRTag(systemRegisterResultCRTag);
	systemRegisterUnit->outResultCRData(systemRegisterResultCRData);
	systemRegisterUnit->outCR(systemRegisterCRData);
	
	completionUnit->inSystemRegisterFinished(systemRegisterFinished);
	completionUnit->inSystemRegisterTag(systemRegisterTag);
	completionUnit->inSystemRegisterCRData(systemRegisterCRData);
	
	dispatchUnit->inSystemRegisterResultValid(systemRegisterResultValid);
	dispatchUnit->inSystemRegisterResultTag(systemRegisterResultTag);
	dispatchUnit->inSystemRegisterResultData(systemRegisterResultData);
	dispatchUnit->inSystemRegisterResultLRValid(systemRegisterResultLRValid);
	dispatchUnit->inSystemRegisterResultLRData(systemRegisterResultLRData);
	dispatchUnit->inSystemRegisterResultLRTag(systemRegisterResultLRTag);
	dispatchUnit->inSystemRegisterResultCTRValid(systemRegisterResultCTRValid);
	dispatchUnit->inSystemRegisterResultCTRTag(systemRegisterResultCTRTag);
	dispatchUnit->inSystemRegisterResultCTRData(systemRegisterResultCTRData);
	dispatchUnit->inSystemRegisterResultCRValid(systemRegisterResultCRValid);
	dispatchUnit->inSystemRegisterResultCRTag(systemRegisterResultCRTag);
	dispatchUnit->inSystemRegisterResultCRData(systemRegisterResultCRData);
	
	registerFile->inWriteRename[SystemRegisterWriteRenamePort](systemRegisterResultValid);
	registerFile->inWriteRenameNumber[SystemRegisterWriteRenamePort](systemRegisterResultTag);
	registerFile->inWriteRenameData[SystemRegisterWriteRenamePort](systemRegisterResultData);
	
	linkRegister->inWriteRenameLR[SystemRegisterWriteRenameLRPort](systemRegisterResultLRValid);	
	linkRegister->inWriteRenameLRNumber[SystemRegisterWriteRenameLRPort](systemRegisterResultLRTag);	
	linkRegister->inWriteRenameLRData[SystemRegisterWriteRenameLRPort](systemRegisterResultLRData);
	
	countRegister->inWriteRenameCTR[SystemRegisterWriteRenameCTRPort](systemRegisterResultCTRValid);	
	countRegister->inWriteRenameCTRNumber[SystemRegisterWriteRenameCTRPort](systemRegisterResultCTRTag);	
	countRegister->inWriteRenameCTRData[SystemRegisterWriteRenameCTRPort](systemRegisterResultCTRData);
	
	conditionRegister->inWriteRenameCR[SystemRegisterWriteRenameCRPort](systemRegisterResultCRValid);	
	conditionRegister->inWriteRenameCRNumber[SystemRegisterWriteRenameCRPort](systemRegisterResultCRTag);	
	conditionRegister->inWriteRenameCRData[SystemRegisterWriteRenameCRPort](systemRegisterResultCRData);
	
	completionUnit->inSystemRegisterResultValid(systemRegisterResultValid);
	completionUnit->inSystemRegisterResultTag(systemRegisterResultTag);
	completionUnit->inSystemRegisterResultLRValid(systemRegisterResultLRValid);
	completionUnit->inSystemRegisterResultLRTag(systemRegisterResultLRTag);
	completionUnit->inSystemRegisterResultCTRValid(systemRegisterResultCTRValid);
	completionUnit->inSystemRegisterResultCTRTag(systemRegisterResultCTRTag);
	completionUnit->inSystemRegisterResultCRValid(systemRegisterResultCRValid);
	completionUnit->inSystemRegisterResultCRTag(systemRegisterResultCRTag);
	
	dispatchUnit->inSystemRegisterFinished(systemRegisterFinished);	
	
	/* Write back */
	for(i = 0; i < nWriteBackPorts; i++)
	{
		completionUnit->outWriteBack[i](writeBack[i]);
		completionUnit->outWriteBackRegisterNumber[i](writeBackRegisterNumber[i]);
		completionUnit->outWriteBackRenameNumber[i](writeBackRenameNumber[i]);
		registerFile->inWriteBack[i](writeBack[i]);
		registerFile->inWriteBackRegisterNumber[i](writeBackRegisterNumber[i]);
		registerFile->inWriteBackRenameNumber[i](writeBackRenameNumber[i]);
	}

	for(i = 0; i < nWriteBackFloatingPointPorts; i++)
	{
		completionUnit->outWriteBackFloatingPoint[i](writeBackFloatingPoint[i]);
		completionUnit->outWriteBackFloatingPointRegisterNumber[i](writeBackFloatingPointRegisterNumber[i]);
		completionUnit->outWriteBackFloatingPointRenameNumber[i](writeBackFloatingPointRenameNumber[i]);
		registerFile->inWriteBackFloatingPoint[i](writeBackFloatingPoint[i]);
		registerFile->inWriteBackFloatingPointRegisterNumber[i](writeBackFloatingPointRegisterNumber[i]);
		registerFile->inWriteBackFloatingPointRenameNumber[i](writeBackFloatingPointRenameNumber[i]);
	}
	
	for(i = 0; i < nWriteBackCRPorts; i++)
	{
		completionUnit->outWriteBackCR[i](writeBackCR[i]);
		completionUnit->outWriteBackCRRenameNumber[i](writeBackCRRenameNumber[i]);
		completionUnit->outWriteBackCRFieldNumber[i](writeBackCRFieldNumber[i]);
		conditionRegister->inWriteBackCR[i](writeBackCR[i]);
		conditionRegister->inWriteBackCRRenameNumber[i](writeBackCRRenameNumber[i]);
		conditionRegister->inWriteBackCRFieldNumber[i](writeBackCRFieldNumber[i]);
	}

	for(i = 0; i < nWriteBackLRPorts; i++)
	{
		completionUnit->outWriteBackLR[i](writeBackLR[i]);
		completionUnit->outWriteBackLRRenameNumber[i](writeBackLRRenameNumber[i]);
		linkRegister->inWriteBackLR[i](writeBackLR[i]);
		linkRegister->inWriteBackLRRenameNumber[i](writeBackLRRenameNumber[i]);
	}
	
	for(i = 0; i < nWriteBackCTRPorts; i++)
	{
		completionUnit->outWriteBackCTR[i](writeBackCTR[i]);
		completionUnit->outWriteBackCTRRenameNumber[i](writeBackCTRRenameNumber[i]);
		countRegister->inWriteBackCTR[i](writeBackCTR[i]);
		countRegister->inWriteBackCTRRenameNumber[i](writeBackCTRRenameNumber[i]);
	}
	
	for(i = 0; i < nMemoryWriteBackPorts; i++)
	{
		completionUnit->outMemoryWriteBack[i](memoryWriteBack[i]);
		completionUnit->outMemoryWriteBackTag[i](memoryWriteBackTag[i]);
		loadStoreUnit->inWriteBack[i](memoryWriteBack[i]);
		loadStoreUnit->inWriteBackTag[i](memoryWriteBackTag[i]);
	}
	
	completionUnit->outWriteOverflow(writeOverflow);
	completionUnit->outOverflow(writeOverflowData);
	completionUnit->outWriteCarry(writeCarry);
	completionUnit->outCarry(writeCarryData);
	xerRegister->inWriteCarry(writeCarry);
	xerRegister->inCarry(writeCarryData);
	xerRegister->inWriteOverflow(writeOverflow);
	xerRegister->inOverflow(writeOverflowData);
	
	/* XER Register */
	xerRegister->outCarry(carry);
	xerRegister->outOverflow(overflow);
	xerRegister->outSummaryOverflow(summaryOverflow);
	
	/* Condition Register */
	conditionRegister->inSummaryOverflow(summaryOverflow);
	conditionRegister->outCR(cr);
	dispatchUnit->inCR(cr);
	systemRegisterUnit->inCR(cr);
	
	completionUnit->outWriteCR(writeCR);
	completionUnit->outCRM(crm);
	completionUnit->outCR(writeCRData);
	conditionRegister->inWriteCR(writeCR);
	conditionRegister->inCRM(crm);
	conditionRegister->inCR(writeCRData);
	
	/* Link Register */
	linkRegister->outLR(lr);
	dispatchUnit->inLR(lr);
	systemRegisterUnit->inLR(lr);
	
	/* Count Register */
	countRegister->outCTR(ctr);
	dispatchUnit->inCTR(ctr);
	systemRegisterUnit->inCTR(ctr);	
	
	/* Interlock */
	completionUnit->outClearInterlock(clearInterlock);
	dispatchUnit->inClearInterlock(clearInterlock);
	
	/* signals to ensure pipeline order */
	completionUnit->outDecode(decode);
	dispatchUnit->inDecode(decode);
	dispatchUnit->outFinish(finish);
	completionUnit->inFinish(finish);

}

void PowerPC::Reset()
{
	int i;
	
	/* Instruction cache */	
	icacheCancel.write(false);
	fetch2icacheReq.write(false);
	fetch2icacheAddr.write(0);
	fetch2icacheSize.write(false);
	icache2fetchAck.write(false);
	icache2fetchHit.write(false);
	
	/* Bus interface */
	icache2biuReq.write(false);
	icache2biuAddr.write(0);
	biu2icacheAck.write(false);
	
	/* Branch Prediction */
	readBHTPC.write(0);
	readBTICPC.write(0);
	
	readBHTTakenBranch.write(false);
		
	readBTICInstructionsCount.write(0);
	readBTICHit.write(false);
	
	for(i = 0; i < nInstructionsPerBTICEntry; i++)
		readBTICInstructions[i].write(0);
	
	updateBTIC.write(false);
	updateBTICPC.write(0);
	updateBTICInstructionsCount.write(0);
	
	for(i = 0; i < nInstructionsPerBTICEntry; i++)
		updateBTICInstructions[i].write(0);

	/* Instructions into instructio queue */
	instructionsInQueue.write(0);
	branchsInQueue.write(0);
	dispatchedInstructions.write(0);
	dispatchedBranchs.write(0);
	
	/* Register File */
	for(i = 0; i < nReadRegisterPorts; i++)
	{
		readRegisterNumber[i].write(0);
		readRegisterData[i].write(0);
	}
	
	for(i = 0; i < nReadFloatingPointRegisterPorts; i++)
	{
		readFloatingPointRegisterNumber[i].write(0);
		readFloatingPointRegisterData[i].write(0);
	}
	
	for(i = 0; i < nReadRenamePorts; i++)
	{
		readRenameNumber[i].write(0);
		readRenameData[i].write(0);
	}
	
	for(i = 0; i < nReadFloatingPointRenamePorts; i++)
	{
		readFloatingPointRenameNumber[i].write(0);
		readFloatingPointRenameData[i].write(0);
	}
	/* Condition Register */
	for(i = 0; i < nReadRenameCRPorts; i++)
	{
		readRenameCRNumber[i].write(0);
		readRenameCRData[i].write(0);
	}
	cr.write(conditionRegister->cr);
	
	/* Link Register */
	for(i = 0; i < nReadRenameLRPorts; i++)
	{
		readRenameLRNumber[i].write(0);
		readRenameLRData[i].write(0);
	}
	lr.write(linkRegister->lr);
	
	/* Count Register */
	for(i = 0; i < nReadRenameCRPorts; i++)
	{
		readRenameCTRNumber[i].write(0);
		readRenameCTRData[i].write(0);
	}
	ctr.write(countRegister->ctr);
	
	/* XER Register */
	overflow.write(xerRegister->overflow);
	carry.write(xerRegister->carry);
	summaryOverflow.write(xerRegister->summaryOverflow);
	
	/* Integer units */
	for(i = 0; i < nIntegerUnit; i++)
	{
		integerBusy[i].write(false);
		integerDispatched[i].write(false);
		integerExecute[i].write(false);
	}
	
	/* Floating Point units */
	for(i = 0; i < nFloatingPointUnits; i++)
	{
		floatingPointBusy[i].write(false);
		floatingPointDispatched[i].write(false);
		floatingPointExecute[i].write(false);
	}
	
	/* Load/store unit */
	loadStoreBusy.write(false);
	loadStoreDispatched.write(false);
	loadStoreBranchCounter.write(0);
	storeQueueStalled.write(false);
	loadQueueStalled.write(false);
	
	/* Branch unit */
	branchBusy.write(false);
	branchDispatched.write(false);
	
	/* system register unit */
	systemRegisterDispatched.write(false);
	systemRegisterBusy.write(false);
	systemRegisterExecute.write(false);

	/* L1 Data cache */	
	lsu2dcacheReq.write(false);
	lsu2dcacheWrite.write(false);
	lsu2dcacheAddr.write(0);
	lsu2dcacheSize.write(0);
	dcache2lsuAck.write(false);
	dcache2lsuHit.write(false);

	/* Bus interface */
	dcache2biuReq.write(false);
	dcache2biuWrite.write(false);
	dcache2biuAddr.write(0);

	biu2dcacheAck.write(false);
	
	/* Integer units results */
	for(i = 0; i < nIntegerUnit; i++)
	{
		integerFinished[i].write(false);
		integerTag[i].write(-1);
		integerResultValid[i].write(false);
		integerResultTag[i].write(-1);
		integerResultData[i].write(0);
		integerResultCRValid[i].write(false);
		integerResultCRTag[i].write(-1);
		integerResultCRData[i].write(0);
		integerOverflow[i].write(false);
		integerCarry[i].write(false);
	}
	
	/* Floating point units results */
	for(i = 0; i < nFloatingPointUnits; i++)
	{
		floatingPointFinished[i].write(false);
		floatingPointTag[i].write(-1);
		floatingPointResultValid[i].write(false);
		floatingPointResultTag[i].write(-1);
		floatingPointResultData[i].write(0);
		floatingPointResultCRValid[i].write(false);
		floatingPointResultCRTag[i].write(-1);
		floatingPointResultCRData[i].write(0);
	}
	
	/* Load/Store unit results */
	for(i = 0; i < 2; i++)
	{
		loadStoreFinished[i].write(false);
		loadStoreTag[i].write(-1);
	}
	for(i = 0; i < 2; i++)
	{
		loadStoreResultValid[i].write(false);
		loadStoreResultTag[i].write(-1);
		loadStoreResultData[i].write(0);
	}
	loadStoreFloatingPointResultValid.write(false);
	loadStoreFloatingPointResultData.write(0);
	
	/* Branch unit results */
	branchFinished.write(false);
	branchTag.write(-1);
	
	branchResultLRValid.write(false);
	branchResultLRTag.write(-1);
	branchResultLRData.write(0);
	
	branchResultCTRValid.write(false);
	branchResultCTRTag.write(-1);
	branchResultCTRData.write(0);
	
	branchPC.write(0);
	mispredictedBranch.write(false);
	
	updateBHT.write(false);
	updateBHTPC.write(0);
	updateBHTTakenBranch.write(false);
	
	/* System register unit results */
	systemRegisterFinished.write(false);
	systemRegisterTag.write(-1);
	
	systemRegisterResultValid.write(false);
	systemRegisterResultTag.write(-1);
	systemRegisterResultData.write(0);
	
	systemRegisterResultLRValid.write(false);
	systemRegisterResultLRTag.write(-1);
	systemRegisterResultLRData.write(0);
	
	systemRegisterResultCTRValid.write(false);
	systemRegisterResultCTRTag.write(-1);
	systemRegisterResultCTRData.write(0);

	systemRegisterResultCRValid.write(false);
	systemRegisterResultCRTag.write(-1);
	systemRegisterResultCRData.write(0);
	
	/* Write back */
	for(i = 0; i < nWriteBackPorts; i++)
	{
		writeBack[i].write(false);
		writeBackRegisterNumber[i].write(0);
		writeBackRenameNumber[i].write(0);
	}
	
	for(i = 0; i < nWriteBackFloatingPointPorts; i++)
	{
		writeBackFloatingPoint[i].write(false);
		writeBackFloatingPointRegisterNumber[i].write(0);
		writeBackFloatingPointRenameNumber[i].write(0);
	}
	
	for(i = 0; i < nWriteBackCRPorts; i++)
	{
		writeBackCR[i].write(false);
		writeBackCRRenameNumber[i].write(0);
		writeBackCRFieldNumber[i].write(0);
	}
	
	for(i = 0; i < nWriteBackLRPorts; i++)
	{
		writeBackLR[i].write(false);
		writeBackLRRenameNumber[i].write(0);
	}

	for(i = 0; i < nWriteBackCTRPorts; i++)
	{
		writeBackCTR[i].write(false);
		writeBackCTRRenameNumber[i].write(0);
	}
	
	for(i = 0; i < nMemoryWriteBackPorts; i++)
	{
		memoryWriteBack[i].write(false);
		memoryWriteBackTag[i].write(0);
	}

	writeCarry.write(false);
	writeCarryData.write(false);
	writeOverflow.write(false);
	writeOverflowData.write(false);
	
	writeCR.write(false);
	
	/* Interlock */
	clearInterlock.write(false);
	
	fetchUnit->Reset();
	dcache->Reset();
	icache->Reset();
	busInterfaceUnit->Reset();
	dispatchUnit->Reset();
	
	for(i = 0; i < nIntegerUnit; i++)
		integerUnit[i]->Reset();
		
	for(i = 0; i < nFloatingPointUnits; i++)
		floatingPointUnit[i]->Reset();
		
	loadStoreUnit->Reset();
	branchUnit->Reset();
	systemRegisterUnit->Reset();
	completionUnit->Reset();
	reorderBuffer->Flush();
	registersBinding->Reset();
}

bool PowerPC::LoadBinary(UInt32 addr, const char *filename)
{
	FILE *fobj = fopen(filename, "rb");
	if(fobj == NULL) return false;
	
	long size;
	fseek(fobj, 0, SEEK_END);
	size = ftell(fobj);
	fseek(fobj, 0, SEEK_SET);

	UInt8 *buffer = (UInt8 *) malloc(size);
	fread(buffer, size, 1, fobj);
	busInterfaceUnit->MemoryWrite(addr, buffer, size);
	free(buffer);
	fclose(fobj);
	return true;
}

bool PowerPC::LoadProgram(const char *filename, int argc, char *argv[], char *envp[])
{
	Elf32_Ehdr *hdr;
	Elf32_Phdr *phdr;
	Elf32_Shdr *shdr;
	char *string_table;
	Elf32_Shdr *shdr_new_section;
	Elf32_Word new_section_size, new_section_type;
	void *new_section;
	Elf32_Addr new_section_addr;
	int i;	
	FILE *fobj;
	UInt32 data_base = 0;
	UInt32 data_size = 0;

	/* Load the program into the simulator memory */
	fobj = fopen(filename, "rb");
	if(fobj == NULL) return false;

	hdr = ReadElfHeader(fobj);
	
	if(hdr == NULL)
	{
		fprintf(stderr, "Could not read ELF32 header or this is not an ELF32 file.\n");
		exit(1);
	}
		
	phdr = ReadProgramHeaders(hdr, fobj);

	for(i=0; i<hdr->e_phnum; i++)
	{
	    if ( (phdr[i].p_type == PT_LOAD) /* Loadable Program Segment */
		 &&
		 ((phdr->p_flags & PF_X) != 0 /* not text segment => data segment */))
	    {
		data_base = phdr[i].p_vaddr;
		data_size = phdr[i].p_memsz;
	    }
	}

	shdr = ReadSectionHeaders(hdr, fobj);
	
	if(shdr == NULL)
	{
		fprintf(stderr, "Can't read section headers from executable\n");
		exit(1);
	}
	string_table = LoadStringTable(hdr, shdr, fobj);

	for(i = 0; i < hdr->e_shnum; i++)
	{
		shdr_new_section = &shdr[i];

		new_section_type = GetSectionType(shdr_new_section);
		if ( (new_section_type == SHT_PROGBITS)
			||
			(new_section_type == SHT_NOBITS) )
		{
			new_section_size = shdr_new_section ? GetSectionSize(shdr_new_section) : 0;
			new_section_addr = GetSectionAddr(shdr_new_section);
			
//			cerr << "Loading " << GetSymbolName(shdr[i].sh_name, string_table) << " (" << new_section_size << " bytes) of type " << new_section_type << " at "; WriteHex(cerr, (UInt32) new_section_addr); cerr << endl;
		
			if (new_section_size)
			{
				new_section = malloc(new_section_size);
				LoadSection(shdr_new_section, new_section, fobj);
				if ( (strcmp(GetSymbolName(shdr[i].sh_name, string_table), ".bss") == 0)
						||
					(strcmp(GetSymbolName(shdr[i].sh_name, string_table), ".sbss") == 0) )
				{
					/* unitialized section => write 0s */
					busInterfaceUnit->ZeroMemory(new_section_addr, new_section_size);
				}
				else
				{
					/* initialized section => copy from objfile */
					if (strcmp(GetSymbolName(shdr[i].sh_name, string_table), ".text") == 0)
					{
						busInterfaceUnit->MemoryWrite(new_section_addr, (UInt8 *) new_section, new_section_size);
						busInterfaceUnit->ZeroMemory(new_section_addr + new_section_size, 128);
						fetchUnit->programCounter = new_section_addr;
					}
					else
					{
						busInterfaceUnit->MemoryWrite(new_section_addr, (UInt8 *) new_section, new_section_size);
					}

				}
		
				free(new_section);
			}
		}
	}
	
	fclose(fobj);
	
	UInt32 envAddr, argAddr;
	int nEnvStrings;
	UInt32 strSize;
	
	busInterfaceUnit->ZeroMemory(StackBase - StackSize, StackSize);
	
	UInt32& sp = registerFile->registers[1];
	sp = StackBase - MaxEnviron;
	
	envAddr = sp;
	argAddr = sp;
	
	for(nEnvStrings = 0; envp[nEnvStrings]; nEnvStrings++)
	{
		strSize = strlen(envp[nEnvStrings]) + 1;
		busInterfaceUnit->MemoryWrite(envAddr, (UInt8 *) envp[nEnvStrings], strSize);
		envAddr += strSize;
	}
	
	for(i = argc - 1; i >= 0; i--)
	{
		strSize = strlen(argv[i]) + 1;
		sp -= strSize;
		busInterfaceUnit->MemoryWrite(sp, (UInt8 *) argv[i], strSize);
	}
	
	sp -= sp % 4;
	sp -= 4;
	busInterfaceUnit->WriteWord(sp, 0);
	
	for(i = nEnvStrings - 1; i >= 0; i--)
	{
		strSize = strlen(envp[i]) + 1;
		envAddr -= strSize;
		sp -= 4;
		busInterfaceUnit->WriteWord(sp, envAddr);
	}
	
	sp -= 4;
	busInterfaceUnit->WriteWord(sp, 0);
	
	for(i = argc - 1; i >= 0; i--)
	{
		strSize = strlen(argv[i]) + 1;
		argAddr -= strSize;
		sp -= 4;
		busInterfaceUnit->WriteWord(sp, argAddr);
	}
	
	sp -= 4;
	busInterfaceUnit->WriteWord(sp, argc);
	return true;
}

#ifdef EMULATE
void PowerPC::CheckRegisters()
{
	int num;
	
	for(num = 0; num < nRegisters; num++)
	{
		if(GPR(num) != powerpc->registerFile->registers[num])
		{
			cout << "Simulator r" << num << " (";
			WriteHex(cout, powerpc->registerFile->registers[num]);
			cout << ") != " << " Emulator r" << num << " (";
			WriteHex(cout, (UInt32) GPR(num));
			cout << ")  !" << endl;
			ppc_debug_dump_registers(stdout);
			cout << *powerpc->registerFile;
			cout << "XER[SO] = " << powerpc->xerRegister->summaryOverflow << endl;
			cout << "XER[OV] = " << powerpc->xerRegister->overflow << endl;
			cout << "XER[CA] = " << powerpc->xerRegister->carry << endl;
			cout << "PC = "; WriteHex(cout, PC); cout << endl;
			cout << "time stamp = " << sim_time_stamp() << endl;
			ABORT();
		}
	}
	for(num = 0; num < nFloatingPointRegisters; num++)
	{
		if(FPR_DW(num) != powerpc->registerFile->floatingPointRegisters[num])
		{
			cout << "Simulator f" << num << " (";
			WriteHex(cout, powerpc->registerFile->floatingPointRegisters[num]);
			cout << ") != " << " Emulator f" << num << " (";
			WriteHex(cout, (UInt64) FPR_DW(num));
			cout << ")  !" << endl;
			ppc_debug_dump_registers(stdout);
			cout << *powerpc->registerFile;
			cout << "XER[SO] = " << powerpc->xerRegister->summaryOverflow << endl;
			cout << "XER[OV] = " << powerpc->xerRegister->overflow << endl;
			cout << "XER[CA] = " << powerpc->xerRegister->carry << endl;
			cout << "PC = "; WriteHex(cout, PC); cout << endl;
			cout << "time stamp = " << sim_time_stamp() << endl;
			ABORT();
		}
	}
	if(LR != powerpc->linkRegister->lr)
	{
		cout << "Simulator LR (";
		WriteHex(cout, powerpc->linkRegister->lr);
		cout << ") != " << " Emulator LR (";
		WriteHex(cout, (UInt32) LR);
		cout << ")  !" << endl;
		ppc_debug_dump_registers(stdout);
		cout << *powerpc->registerFile;
		cout << "XER[SO] = " << powerpc->xerRegister->summaryOverflow << endl;
		cout << "XER[OV] = " << powerpc->xerRegister->overflow << endl;
		cout << "XER[CA] = " << powerpc->xerRegister->carry << endl;
		cout << "PC = "; WriteHex(cout, PC); cout << endl;
		cout << "time stamp = " << sim_time_stamp() << endl;
		ABORT();
	}
	if(CTR != powerpc->countRegister->ctr)
	{
		cout << "Simulator CTR (";
		WriteHex(cout, powerpc->countRegister->ctr);
		cout << ") != " << " Emulator CTR (";
		WriteHex(cout, (UInt32) CTR);
		cout << ")  !" << endl;
		ppc_debug_dump_registers(stdout);
		cout << *powerpc->registerFile;
		cout << "XER[SO] = " << powerpc->xerRegister->summaryOverflow << endl;
		cout << "XER[OV] = " << powerpc->xerRegister->overflow << endl;
		cout << "XER[CA] = " << powerpc->xerRegister->carry << endl;
		cout << "PC = "; WriteHex(cout, PC); cout << endl;
		cout << "time stamp = " << sim_time_stamp() << endl;
		ABORT();
	}
	if(CR != powerpc->conditionRegister->cr)
	{
		cout << "Simulator CR (";
		WriteHex(cout, powerpc->conditionRegister->cr);
		cout << ") != " << " Emulator CR (";
		WriteHex(cout, (UInt32) CR);
		cout << ")  !" << endl;
		cout << "time stamp = " << sim_time_stamp() << endl;
		ppc_debug_dump_registers(stdout);
		cout << *powerpc->registerFile;
		cout << "XER[SO] = " << powerpc->xerRegister->summaryOverflow << endl;
		cout << "XER[OV] = " << powerpc->xerRegister->overflow << endl;
		cout << "XER[CA] = " << powerpc->xerRegister->carry << endl;
		cout << "PC = "; WriteHex(cout, PC); cout << endl;
		cout << "time stamp = " << sim_time_stamp() << endl;
		ABORT();
	}
		
	bool carry = ((XER >> 29) & 0x1) ? true : false;
	if(carry != powerpc->xerRegister->carry)
	{
		cout << "Simulator carry (" << powerpc->xerRegister->carry << ") != Emulator carry (" << carry << ")   !" << endl;
		cout << "PC = "; WriteHex(cout, PC); cout << endl;
		cout << "time stamp = " << sim_time_stamp() << endl;
		ABORT();
	}
	
	bool overflow = ((XER >> 30) & 0x1) ? true : false;
	if(overflow != powerpc->xerRegister->overflow)
	{
		cout << "Simulator overflow (" << powerpc->xerRegister->overflow << ") != Emulator overflow (" << overflow << ")   !" << endl;
		cout << "PC = "; WriteHex(cout, PC); cout << endl;
		cout << "time stamp = " << sim_time_stamp() << endl;
		ABORT();
	}
	
	bool summaryOverflow = ((XER >> 31) & 0x1) ? true : false;
	if(summaryOverflow != powerpc->xerRegister->summaryOverflow)
	{
		cout << "Simulator summary overflow (" << powerpc->xerRegister->summaryOverflow << ") != Emulator summary overflow (" << summaryOverflow << ")   !" << endl;
		cout << "PC = "; WriteHex(cout, PC); cout << endl;
		cout << "time stamp = " << sim_time_stamp() << endl;
		ABORT();
	}
}
#endif

void PowerPC::write_gpr(void *instance, int num, word_t value)
{
	powerpc->registerFile->registers[num] = value;
	GPR(num) = value;
}

word_t PowerPC::read_gpr(void *instance, int num)
{
#ifdef EMULATE
	powerpc->CheckRegisters();
#endif
	return powerpc->registerFile->registers[num];
}

void PowerPC::write_fpr(void *instance, int num, dword_t value)
{
	powerpc->registerFile->floatingPointRegisters[num] = value;
}

dword_t PowerPC::read_fpr(void *instance, int num)
{
#ifdef EMULATE
	powerpc->CheckRegisters();
#endif
	return powerpc->registerFile->floatingPointRegisters[num];
}

void PowerPC::write_cr(void *instance, word_t value)
{
	powerpc->conditionRegister->cr = value;
}

word_t PowerPC::read_cr(void *instance)
{
#ifdef EMULATE
	powerpc->CheckRegisters();
#endif
	return powerpc->conditionRegister->cr;
}

void PowerPC::write_lr(void *instance, word_t value)
{
	powerpc->linkRegister->lr = value;
}

word_t PowerPC::read_lr(void *instance)
{
#ifdef EMULATE
	powerpc->CheckRegisters();
#endif
	return powerpc->linkRegister->lr;
}

void PowerPC::write_ctr(void *instance, word_t value)
{
	powerpc->countRegister->ctr = value;
}

word_t PowerPC::read_ctr(void *instance)
{
#ifdef EMULATE
	powerpc->CheckRegisters();
#endif
	return powerpc->countRegister->ctr;
}

void PowerPC::write_xer(void *instance, word_t value)
{
	powerpc->xerRegister->overflow = ((value >> 30) & 1) ? true : false;
	powerpc->xerRegister->summaryOverflow = ((value >> 31) & 1) ? true : false;
	powerpc->xerRegister->carry = ((value >> 29) & 1) ? true : false;
}

word_t PowerPC::read_xer(void *instance)
{
#ifdef EMULATE
	powerpc->CheckRegisters();
#endif
	return (powerpc->xerRegister->overflow ? (1 << 30) : 0)
	     | (powerpc->xerRegister->summaryOverflow ? (1 << 31) : 0)
	     | (powerpc->xerRegister->carry ? (1 << 29) : 0);
}

void PowerPC::write_fpscr(void *instance, word_t value)
{
	/* unimplemented */
}

word_t PowerPC::read_fpscr(void *instance)
{
#ifdef EMULATE
	powerpc->CheckRegisters();
#endif
	/* unimplemented */
	return 0;
}

void PowerPC::write_pc(void *instance, word_t value)
{
	powerpc->fetchUnit->programCounter = value;
}

word_t PowerPC::read_pc(void *instance)
{
#ifdef EMULATE
	powerpc->CheckRegisters();
#endif
	return powerpc->fetchUnit->programCounter;
}



void PowerPC::reset_cr0so(void *instance)
{
	powerpc->conditionRegister->Write(powerpc->conditionRegister->cr & 0xefffffff); // CR0[SO] = 1
	CR = CR & 0xefffffff;
}

void PowerPC::set_cr0so(void *instance)
{
	powerpc->conditionRegister->Write(powerpc->conditionRegister->cr | 0x10000000); // CR0[SO] = 0
	CR = CR | 0x10000000;
}

byte_t PowerPC::mem_read_byte(void *instance, addr_t addr)
{
	byte_t value;
	
	if(powerpc->dcache->ReadByte(addr, value))
	{
#ifdef EMULATE		
		if(::MEM_READ_BYTE(addr) != value)
		{
			cout << "PowerPC::mem_read_byte() : data at "; WriteHex(cout, addr); cout << " is not the same into emulator and simulator (data cache)" << endl;
			StopSimulation();
		}
#endif		
		return value;
	}
	value = powerpc->busInterfaceUnit->ReadByte(addr);
	
#ifdef EMULATE		
	if(::MEM_READ_BYTE(addr) != value)
	{
		cout << "PowerPC::mem_read_byte() : data at "; WriteHex(cout, addr); cout << " is not the same into emulator and simulator (memory)" << endl;
		StopSimulation();
	}
#endif
	return value;
}

void PowerPC::mem_write_byte(void *instance, addr_t addr, byte_t value)
{
	powerpc->dcache->WriteByte(addr, value);
	powerpc->busInterfaceUnit->WriteByte(addr, value);
	::MEM_WRITE_BYTE(addr, value);
}

halfword_t PowerPC::mem_read_half_word(void *instance, addr_t addr)
{
	halfword_t value;
	
	if(powerpc->dcache->ReadHalfWord(addr, value))
	{
#ifdef EMULATE		
		if(::MEM_READ_HALF_WORD(addr) != value)
		{
			cout << "PowerPC::mem_read_halfword() : data at "; WriteHex(cout, addr); cout << " is not the same into emulator and simulator (data cache)" << endl;
			StopSimulation();
		}
#endif		
		return value;
	}
	value = powerpc->busInterfaceUnit->ReadHalfWord(addr);
	
#ifdef EMULATE		
	if(::MEM_READ_HALF_WORD(addr) != value)
	{
		cout << "PowerPC::mem_read_halfword() : data at "; WriteHex(cout, addr); cout << " is not the same into emulator and simulator (memory)" << endl;
		StopSimulation();
	}
#endif
	return value;
}

void PowerPC::mem_write_half_word(void *instance, addr_t addr, halfword_t value)
{
	powerpc->dcache->WriteHalfWord(addr, value);
	powerpc->busInterfaceUnit->WriteHalfWord(addr, value);
	::MEM_WRITE_HALF_WORD(addr, value);
}

word_t PowerPC::mem_read_word(void *instance, addr_t addr)
{
	word_t value;
	
	if(powerpc->dcache->ReadWord(addr, value))
	{
#ifdef EMULATE		
		if(::MEM_READ_WORD(addr) != value)
		{
			cout << "PowerPC::mem_read_word() : data at "; WriteHex(cout, addr); cout << " is not the same into emulator and simulator (data cache)" << endl;
			StopSimulation();
		}
#endif		
		return value;
	}
	value = powerpc->busInterfaceUnit->ReadWord(addr);
	
#ifdef EMULATE		
	if(::MEM_READ_WORD(addr) != value)
	{
		cout << "PowerPC::mem_read_word() : data at "; WriteHex(cout, addr); cout << " is not the same into emulator and simulator (memory)" << endl;
		StopSimulation();
	}
#endif
	return value;
}

void PowerPC::mem_write_word(void *instance, addr_t addr, word_t value)
{
	powerpc->dcache->WriteWord(addr, value);
	powerpc->busInterfaceUnit->WriteWord(addr, value);
	::MEM_WRITE_WORD(addr, value);
}

dword_t PowerPC::mem_read_dword(void *instance, addr_t addr)
{
	dword_t value;
	
	if(powerpc->dcache->ReadDWord(addr, value))
	{
#ifdef EMULATE		
		if(::MEM_READ_DWORD(addr) != value)
		{
			cout << "PowerPC::mem_read_dword() : data at "; WriteHex(cout, addr); cout << " is not the same into emulator and simulator (data cache)" << endl;
			StopSimulation();
		}
#endif		
		return value;
	}
	value = powerpc->busInterfaceUnit->ReadDWord(addr);
	
#ifdef EMULATE		
	if(::MEM_READ_DWORD(addr) != value)
	{
		cout << "PowerPC::mem_read_dword() : data at "; WriteHex(cout, addr); cout << " is not the same into emulator and simulator (memory)" << endl;
		StopSimulation();
	}
#endif
	return value;
}

void PowerPC::mem_write_dword(void *instance, addr_t addr, dword_t value)
{
	powerpc->dcache->WriteDWord(addr, value);
	powerpc->busInterfaceUnit->WriteDWord(addr, value);
	::MEM_WRITE_DWORD(addr, value);
}

void PowerPC::mem_set(void *instance, addr_t addr, byte_t value, int size)
{
	powerpc->dcache->MemorySet(addr, value, size);
	powerpc->busInterfaceUnit->MemorySet(addr, value, size);
	::MEM_SET(addr, value, size);
}

void PowerPC::mem_read(void *instance, void *buf, addr_t addr, int size)
{
	powerpc->busInterfaceUnit->MemoryRead((UInt8 *) buf, addr, size);
	powerpc->dcache->MemoryRead((UInt8 *) buf, addr, size);
#ifdef EMULATE
	void *buf2 = malloc(size);
	::MEM_READ(buf2, addr, size);
	if(memcmp(buf, buf2, size) != 0)
	{
		cout << "PowerPC::mem_read : data at "; WriteHex(cout, addr); cout << " is not the same into emulator and simulator" << endl;
	}
	free(buf2);
#endif	
}

void PowerPC::mem_write(void *instance, addr_t addr, void *buf, int size)
{
	powerpc->dcache->MemoryWrite(addr, (UInt8 *) buf, size);
	powerpc->busInterfaceUnit->MemoryWrite(addr, (UInt8 *) buf, size);
	::MEM_WRITE(addr, buf, size);
}

void PowerPC::Stop(int ninstructions)
{
	completionUnit->Stop(ninstructions);
	dcache->ResetStats();
	icache->ResetStats();
}
