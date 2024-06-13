/***************************************************************************
                          CompletionUnit.h  -  unite de completion
                             -------------------
    begin                : Thu Apr 5 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __COMPLETIONUNIT_H__
#define __COMPLETIONUNIT_H__

#include <systemc.h>
#include <common.h>
#include <ReorderBuffer.h>
#include <RegistersBinding.h>

SC_MODULE(CompletionUnit)
{
	sc_in<bool> inClock;
	
	/* From integer units */	
	sc_in<bool> inIntegerFinished[nIntegerUnit];
	sc_in<tag_t> inIntegerTag[nIntegerUnit];
	sc_in<bool> inIntegerResultValid[nIntegerUnit];
	sc_in<tag_t> inIntegerResultTag[nIntegerUnit];
	sc_in<bool> inIntegerResultCRValid[nIntegerUnit];
	sc_in<tag_t> inIntegerResultCRTag[nIntegerUnit];
	sc_in<bool> inIntegerOverflow[nIntegerUnit];
	sc_in<bool> inIntegerCarry[nIntegerUnit];
	
	/* From Floating Point Units */
	sc_in<bool> inFloatingPointFinished[nFloatingPointUnits];
	sc_in<tag_t> inFloatingPointTag[nFloatingPointUnits];
	sc_in<bool> inFloatingPointResultValid[nFloatingPointUnits];
	sc_in<tag_t> inFloatingPointResultTag[nFloatingPointUnits];
	sc_in<bool> inFloatingPointResultCRValid[nFloatingPointUnits];
	sc_in<tag_t> inFloatingPointResultCRTag[nFloatingPointUnits];
	
	/* From load/store unit */
	sc_in<bool> inLoadStoreFinished[2];
	sc_in<tag_t> inLoadStoreTag[2];
	sc_in<bool> inLoadStoreResultValid[2];
	sc_in<bool> inLoadStoreFloatingPointResultValid;
	sc_in<tag_t> inLoadStoreResultTag[2];
	
	/* From Branch Unit */
	sc_in<bool> inBranchFinished;
	sc_in<tag_t> inBranchTag;
	sc_in<bool> inMispredictedBranch;
	sc_in<UInt32> inBranchPC;
	
	sc_in<bool> inBranchResultLRValid;
	sc_in<tag_t> inBranchResultLRTag;
	sc_in<bool> inBranchResultCTRValid;
	sc_in<tag_t> inBranchResultCTRTag;
	
	/* From System Register Unit */
	sc_in<bool> inSystemRegisterFinished;
	sc_in<tag_t> inSystemRegisterTag;
	
	sc_in<bool> inSystemRegisterResultValid;
	sc_in<tag_t> inSystemRegisterResultTag;
	
	sc_in<bool> inSystemRegisterResultLRValid;
	sc_in<tag_t> inSystemRegisterResultLRTag;
	
	sc_in<bool> inSystemRegisterResultCTRValid;
	sc_in<tag_t> inSystemRegisterResultCTRTag;
	
	sc_in<bool> inSystemRegisterResultCRValid;
	sc_in<tag_t> inSystemRegisterResultCRTag;
	
	sc_in<UInt32> inSystemRegisterCRData;
	
	/* To Register file */
	sc_out<Dummy<bool> > outWriteBack[nWriteBackPorts];
	sc_out<regnum_t> outWriteBackRegisterNumber[nWriteBackPorts];
	sc_out<tag_t> outWriteBackRenameNumber[nWriteBackPorts];
	
	sc_out<Dummy<bool> > outWriteBackFloatingPoint[nWriteBackFloatingPointPorts];
	sc_out<regnum_t> outWriteBackFloatingPointRegisterNumber[nWriteBackFloatingPointPorts];
	sc_out<tag_t> outWriteBackFloatingPointRenameNumber[nWriteBackFloatingPointPorts];
	
	/* To Condition Register */
	sc_out<Dummy<bool> > outWriteBackCR[nWriteBackCRPorts];
	sc_out<tag_t> outWriteBackCRRenameNumber[nWriteBackCRPorts];
	sc_out<fieldnum_t> outWriteBackCRFieldNumber[nWriteBackCRPorts];
	
	sc_out<bool> outWriteCR;
	sc_out<UInt8> outCRM;
	sc_out<UInt32> outCR;
	
	/* To Link Register */
	sc_out<Dummy<bool> > outWriteBackLR[nWriteBackLRPorts];
	sc_out<tag_t> outWriteBackLRRenameNumber[nWriteBackLRPorts];
	
	/* To Count Register */
	sc_out<Dummy<bool> > outWriteBackCTR[nWriteBackCTRPorts];
	sc_out<tag_t> outWriteBackCTRRenameNumber[nWriteBackCTRPorts];
	
	/* To XER */
	sc_out<bool> outWriteOverflow;
	sc_out<bool> outOverflow;
	sc_out<bool> outWriteCarry;
	sc_out<bool> outCarry;
	
	/* To Load/Store Unit */
	sc_out<Dummy<bool> > outMemoryWriteBack[nMemoryWriteBackPorts];
	sc_out<tag_t> outMemoryWriteBackTag[nMemoryWriteBackPorts];
	
	/* To Dispatch Unit */
	sc_out<bool> outIntegerExecute[nIntegerUnit];
	sc_out<tag_t> outIntegerExecuteTag[nIntegerUnit];
	sc_out<bool> outFloatingPointExecute[nFloatingPointUnits];
	sc_out<tag_t> outFloatingPointExecuteTag[nFloatingPointUnits];
	sc_out<bool> outSystemRegisterExecute;
	sc_out<tag_t> outSystemRegisterExecuteTag;
	
	sc_out<bool> outClearInterlock;
	
	/* From Dispatch Unit */
	sc_in<Dummy<bool> > inFinish;
	
	/* To Dispatch Unit */
	sc_out<Dummy<bool> > outDecode;

	ReorderBuffer *rob;
	RegistersBinding *registersBinding;
	
	int nInstructionsToRetire;
	UInt64 nRetiredInstructions;
	UInt64 nMaxRetiredInstructions;
	UInt32 programCounter;
	
	void Retire();
	void Finish();
	void OnFallingEdge();
	int GetInstructionsToWriteBack();
	void Flush();
	void Reset();
		
	CompletionUnit(const sc_module_name& name, ReorderBuffer *rob, RegistersBinding *registersBinding);
	void Stop(int ninstructions);
};

#endif
