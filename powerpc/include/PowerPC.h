/***************************************************************************
                          PowerPC.h  -  description
                             -------------------
    begin                : Tue Apr 24 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __POWERPC_H__
#define __POWERPC_H__

#include <systemc.h>
#include <FetchUnit.h>
#include <DispatchUnit.h>
#include <CompletionUnit.h>
#include <IntegerUnit.h>
#include <LoadStoreUnit.h>
#include <RegisterFile.h>
#include <ICache.h>
#include <DCache.h>
#include <BusInterfaceUnit.h>
#include <BranchPredictionUnit.h>
#include <BranchUnit.h>
#include <ConditionRegister.h>
#include <LinkRegister.h>
#include <CountRegister.h>
#include <XERRegister.h>
#include <SystemRegisterUnit.h>
#include <FloatingPointUnit.h>

SC_MODULE(PowerPC)
{
	static PowerPC *powerpc;

	sc_in<bool> inClock;
	
	/* Units */
	ICache *icache;
	FetchUnit *fetchUnit;
	DispatchUnit *dispatchUnit;
	IntegerUnit *integerUnit[nIntegerUnit];
	LoadStoreUnit *loadStoreUnit;
	CompletionUnit *completionUnit;
	RegisterFile *registerFile;
	DCache *dcache;
	BusInterfaceUnit *busInterfaceUnit;
	BranchPredictionUnit *bpu;
	BranchUnit *branchUnit;
	ConditionRegister *conditionRegister;
	LinkRegister *linkRegister;
	CountRegister *countRegister;
	XERRegister *xerRegister;
	SystemRegisterUnit *systemRegisterUnit;
	FloatingPointUnit *floatingPointUnit[nFloatingPointUnits];
	RegistersBinding *registersBinding;
	ReorderBuffer *reorderBuffer;
	
	/* Instruction cache */	
	sc_signal<bool> icacheCancel;
	sc_signal<bool> fetch2icacheReq;
	sc_signal<UInt32> fetch2icacheAddr;
	sc_signal<int> fetch2icacheSize;
	sc_signal<bool> icache2fetchAck;
	sc_signal<DataArray<fetch2icache> > icache2fetchReadData;
	sc_signal<bool> icache2fetchHit;
	
	/* L2 cache / bus Interface */
	sc_signal<bool> icache2biuReq;
	sc_signal<UInt32> icache2biuAddr;
	sc_signal<bool> biu2icacheAck;
	sc_signal<DataArray<icache2biu> > biu2icacheReadData;
	
	/* Prediction de branchement */
	sc_signal<UInt32> readBHTPC;
	sc_signal<UInt32> readBTICPC;
	sc_signal<bool> readBHTTakenBranch; /* true si le branchement est predit pris (du plus recent au plus ancien) */
	sc_signal<int> readBTICInstructionsCount;
	sc_signal<bool> readBTICHit; /* true si hit dans le Branch Target Instruction Cache */
	sc_signal<UInt32> readBTICInstructions[nInstructionsPerBTICEntry]; /* Instructions lues dans le Branch Target Instruction Cache */
	
	sc_signal<Dummy<bool> > updateBTIC;
	sc_signal<UInt32> updateBTICPC;
	sc_signal<UInt32> updateBTICInstructionsCount;
	sc_signal<UInt32> updateBTICInstructions[nInstructionsPerBTICEntry];

	/* instructions dans la file */
	sc_signal<InstructionQueueEntry> instructions[nIQ];
	sc_signal<int> instructionsInQueue;
	sc_signal<InstructionQueueEntry> branchs[nBranchQueueEntries];
	sc_signal<int> branchsInQueue;	
	sc_signal<int> dispatchedInstructions;
	sc_signal<int> dispatchedBranchs;
	
	/* Register File */
	sc_signal<regnum_t> readRegisterNumber[nReadRegisterPorts];
	sc_signal<UInt32> readRegisterData[nReadRegisterPorts];
	
	sc_signal<tag_t> readRenameNumber[nReadRenamePorts];
	sc_signal<UInt32> readRenameData[nReadRenamePorts];
	
	sc_signal<regnum_t> readFloatingPointRegisterNumber[nReadFloatingPointRegisterPorts];
	sc_signal<UInt64> readFloatingPointRegisterData[nReadFloatingPointRegisterPorts];
	
	sc_signal<tag_t> readFloatingPointRenameNumber[nReadFloatingPointRenamePorts];
	sc_signal<UInt64> readFloatingPointRenameData[nReadFloatingPointRenamePorts];
	
	/* Condition Register */
	sc_signal<tag_t> readRenameCRNumber[nReadRenameCRPorts];
	sc_signal<UInt8> readRenameCRData[nReadRenameCRPorts];
	sc_signal<UInt32> cr;
	
	/* Link Register */
	sc_signal<tag_t> readRenameLRNumber[nReadRenameCRPorts];
	sc_signal<UInt32> readRenameLRData[nReadRenameCRPorts];	
	sc_signal<UInt32> lr;
	
	/* Count Register */
	sc_signal<tag_t> readRenameCTRNumber[nReadRenameCRPorts];
	sc_signal<UInt32> readRenameCTRData[nReadRenameCRPorts];	
	sc_signal<UInt32> ctr;
	
	/* XER Register */
	sc_signal<bool> overflow;
	sc_signal<bool> carry;
	sc_signal<bool> summaryOverflow;
	
	/* Integer unit */
	sc_signal<bool> integerBusy[nIntegerUnit];
	sc_signal<bool> integerDispatched[nIntegerUnit];
	sc_signal<IntegerOperation> integerOperation[nIntegerUnit];
	sc_signal<IntegerOperands> integerOperands[nIntegerUnit];
	sc_signal<IntegerTags> integerTags[nIntegerUnit];
	sc_signal<bool> integerExecute[nIntegerUnit];
	sc_signal<tag_t> integerExecuteTag[nIntegerUnit];
	sc_signal<int> integerBranchCounter[nIntegerUnit];
#if defined(DEBUG) || defined(TRACE)
	sc_signal<UInt32> integerInstructionCounter[nIntegerUnit];
	sc_signal<UInt32> integerProgramCounter[nIntegerUnit];
	sc_signal<UInt32> integerInstruction[nIntegerUnit];
#endif
	
	/* Floating Point Unit */
	sc_signal<bool> floatingPointBusy[nFloatingPointUnits];
	sc_signal<bool> floatingPointPipelineEmpty[nFloatingPointUnits];
	sc_signal<bool> floatingPointDispatched[nFloatingPointUnits];
	sc_signal<FloatingPointOperation> floatingPointOperation[nFloatingPointUnits];
	sc_signal<FloatingPointOperands> floatingPointOperands[nFloatingPointUnits];
	sc_signal<FloatingPointTags> floatingPointTags[nFloatingPointUnits];
	sc_signal<bool> floatingPointExecute[nFloatingPointUnits];
	sc_signal<tag_t> floatingPointExecuteTag[nFloatingPointUnits];
	sc_signal<int> floatingPointBranchCounter[nFloatingPointUnits];
#if defined(DEBUG) || defined(TRACE)
	sc_signal<UInt32> floatingPointInstructionCounter[nFloatingPointUnits];
	sc_signal<UInt32> floatingPointProgramCounter[nFloatingPointUnits];
	sc_signal<UInt32> floatingPointInstruction[nFloatingPointUnits];
#endif
	
	/* Load/store Unit */
	sc_signal<bool> loadStoreBusy;
	sc_signal<bool> loadStoreDispatched;
	sc_signal<LoadStoreOperation> loadStoreOperation;
	sc_signal<LoadStoreOperands> loadStoreOperands;
	sc_signal<LoadStoreTags> loadStoreTags;
	sc_signal<int> loadStoreBranchCounter;
#if defined(DEBUG) || defined(TRACE)
	sc_signal<UInt32> loadStoreInstructionCounter;
	sc_signal<UInt32> loadStoreProgramCounter;
	sc_signal<UInt32> loadStoreInstruction;
#endif
	
	sc_signal<bool> storeQueueStalled;
	sc_signal<bool> loadQueueStalled;
	sc_signal<bool> loadStoreEmpty;
	
	/* Branch Unit */
	sc_signal<bool> branchBusy;
	sc_signal<bool> branchDispatched;
	sc_signal<BranchOperation> branchOperation;
	sc_signal<BranchOperands> branchOperands;
	sc_signal<BranchTags> branchTags;
#if defined(DEBUG) || defined(TRACE)
	sc_signal<UInt32> branchInstructionCounter;
	sc_signal<UInt32> branchInstruction;
#endif
	
	/* System Register Unit */
	sc_signal<bool> systemRegisterDispatched;
	sc_signal<SystemRegisterOperation> systemRegisterOperation;
	sc_signal<SystemRegisterTags> systemRegisterTags;
	sc_signal<bool> systemRegisterBusy;
	sc_signal<bool> systemRegisterExecute;
	sc_signal<tag_t> systemRegisterExecuteTag;
	sc_signal<int> systemRegisterBranchCounter;
#if defined(DEBUG) || defined(TRACE)
	sc_signal<UInt32> systemRegisterInstructionCounter;
	sc_signal<UInt32> systemRegisterProgramCounter;
	sc_signal<UInt32> systemRegisterInstruction;
#endif

	/* Data Cache */	
	sc_signal<bool> lsu2dcacheReq;
	sc_signal<DataArray<lsu2dcache> > lsu2dcacheWriteData;
	sc_signal<bool> lsu2dcacheWrite;
	sc_signal<UInt32> lsu2dcacheAddr;
	sc_signal<int> lsu2dcacheSize;
	sc_signal<bool> lsu2dcacheZeroBlock;
	sc_signal<Dummy<bool> > dcache2lsuAck;
	sc_signal<DataArray<lsu2dcache> > dcache2lsuReadData;
	sc_signal<bool> dcache2lsuHit;
	
	sc_signal<bool> dcacheBusy;
	
	/* Bus Interface Unit */
	sc_signal<bool> dcache2biuReq;
	sc_signal<bool> dcache2biuWrite;
	sc_signal<UInt32> dcache2biuAddr;
	sc_signal<DataArray<dcache2biu> > dcache2biuWriteData;

	sc_signal<DataArray<dcache2biu> > biu2dcacheReadData;
	sc_signal<bool> biu2dcacheAck;
	
	/* Integer unit results */
	sc_signal<bool> integerFinished[nIntegerUnit];
	sc_signal<tag_t> integerTag[nIntegerUnit];
	sc_signal<bool> integerResultValid[nIntegerUnit];
	sc_signal<tag_t> integerResultTag[nIntegerUnit];
	sc_signal<UInt32> integerResultData[nIntegerUnit];
	sc_signal<bool> integerResultCRValid[nIntegerUnit];
	sc_signal<tag_t> integerResultCRTag[nIntegerUnit];
	sc_signal<UInt8> integerResultCRData[nIntegerUnit];
	sc_signal<bool> integerOverflow[nIntegerUnit];
	sc_signal<bool> integerCarry[nIntegerUnit];
	
	/* Floating Point Unit Results */
	sc_signal<bool> floatingPointFinished[nFloatingPointUnits];
	sc_signal<tag_t> floatingPointTag[nFloatingPointUnits];
	sc_signal<bool> floatingPointResultValid[nFloatingPointUnits];
	sc_signal<tag_t> floatingPointResultTag[nFloatingPointUnits];
	sc_signal<UInt64> floatingPointResultData[nFloatingPointUnits];
	sc_signal<bool> floatingPointResultCRValid[nFloatingPointUnits];
	sc_signal<tag_t> floatingPointResultCRTag[nFloatingPointUnits];
	sc_signal<UInt8> floatingPointResultCRData[nFloatingPointUnits];
	
	/* Load/Store unit results */
	sc_signal<bool> loadStoreFinished[2];
	sc_signal<tag_t> loadStoreTag[2];
	sc_signal<bool> loadStoreResultValid[2];
	sc_signal<tag_t> loadStoreResultTag[2];
	sc_signal<UInt32> loadStoreResultData[2];
	sc_signal<bool> loadStoreFloatingPointResultValid;
	sc_signal<UInt64> loadStoreFloatingPointResultData;
	
	/* Branch unit results */
	sc_signal<bool> branchFinished;
	sc_signal<tag_t> branchTag;
	
	sc_signal<bool> branchResultLRValid;
	sc_signal<tag_t> branchResultLRTag;
	sc_signal<UInt32> branchResultLRData;
	
	sc_signal<bool> branchResultCTRValid;
	sc_signal<tag_t> branchResultCTRTag;
	sc_signal<UInt32> branchResultCTRData;
	
	sc_signal<UInt32> branchPC;
	sc_signal<bool> mispredictedBranch;
	
	sc_signal<Dummy<bool> > updateBHT;
	sc_signal<UInt32> updateBHTPC;
	sc_signal<bool> updateBHTTakenBranch;
	
	/* System register unit results */
	sc_signal<bool> systemRegisterFinished;
	sc_signal<tag_t> systemRegisterTag;
	
	sc_signal<bool> systemRegisterResultValid;
	sc_signal<tag_t> systemRegisterResultTag;
	sc_signal<UInt32> systemRegisterResultData;
	
	sc_signal<bool> systemRegisterResultLRValid;
	sc_signal<tag_t> systemRegisterResultLRTag;
	sc_signal<UInt32> systemRegisterResultLRData;
	
	sc_signal<bool> systemRegisterResultCTRValid;
	sc_signal<tag_t> systemRegisterResultCTRTag;
	sc_signal<UInt32> systemRegisterResultCTRData;

	sc_signal<bool> systemRegisterResultCRValid;
	sc_signal<tag_t> systemRegisterResultCRTag;
	sc_signal<UInt8> systemRegisterResultCRData;
	
	sc_signal<UInt32> systemRegisterCRData;
	
	/* Write back */	
	sc_signal<Dummy<bool> > writeBack[nWriteBackPorts];
	sc_signal<regnum_t> writeBackRegisterNumber[nWriteBackPorts];
	sc_signal<tag_t> writeBackRenameNumber[nWriteBackPorts];
	
	sc_signal<Dummy<bool> > writeBackFloatingPoint[nWriteBackFloatingPointPorts];
	sc_signal<regnum_t> writeBackFloatingPointRegisterNumber[nWriteBackFloatingPointPorts];
	sc_signal<tag_t> writeBackFloatingPointRenameNumber[nWriteBackFloatingPointPorts];
	
	sc_signal<Dummy<bool> > writeBackCR[nWriteBackCRPorts];
	sc_signal<regnum_t> writeBackCRRenameNumber[nWriteBackCRPorts];
	sc_signal<tag_t> writeBackCRFieldNumber[nWriteBackCRPorts];

	sc_signal<Dummy<bool> > writeBackLR[nWriteBackLRPorts];
	sc_signal<tag_t> writeBackLRRenameNumber[nWriteBackLRPorts];

	sc_signal<Dummy<bool> > writeBackCTR[nWriteBackCTRPorts];
	sc_signal<tag_t> writeBackCTRRenameNumber[nWriteBackCTRPorts];
	
	sc_signal<Dummy<bool> > memoryWriteBack[nMemoryWriteBackPorts];
	sc_signal<tag_t> memoryWriteBackTag[nMemoryWriteBackPorts];

	sc_signal<bool> writeCarry;
	sc_signal<bool> writeCarryData;
	sc_signal<bool> writeOverflow;
	sc_signal<bool> writeOverflowData;
	
	sc_signal<bool> writeCR;
	sc_signal<UInt8> crm;
	sc_signal<UInt32> writeCRData;
	
	/* Interlock */
	sc_signal<bool> clearInterlock;
	
	/* signal to ensure pipeline order */
	sc_signal<Dummy<bool> > retire;
	sc_signal<Dummy<bool> > decode;
	sc_signal<Dummy<bool> > finish;
	
	bool LoadProgram(const char *filename, int argc, char *argv[], char *envp[]);
	bool LoadBinary(UInt32 addr, const char *filename);
	
	void Build();
	void Reset();
#ifdef EMULATE
	void CheckRegisters();
#endif

	void MemoryRead(void *buffer, UInt32 addr, UInt32 size);
	void MemoryWrite(UInt32 addr, void *buffer, UInt32 size);
	
	static void write_gpr(void *instance, int num, word_t value);
	static word_t read_gpr(void *instance, int num);
	static void write_fpr(void *instance, int num, dword_t);
	static dword_t read_fpr(void *instance, int num);
	static void write_cr(void *instance, word_t value);
	static word_t read_cr(void *instance);
	static void write_lr(void *instance, word_t value);
	static word_t read_lr(void *instance);
	static void write_ctr(void *instance, word_t value);
	static word_t read_ctr(void *instance);
	static void write_xer(void *instance, word_t value);
	static word_t read_xer(void *instance);
	static void write_fpscr(void *instance, word_t value);
	static word_t read_fpscr(void *instance);
	static void write_pc(void *instance, word_t value);
	static word_t read_pc(void *instance);
	
	static void reset_cr0so(void *instance);
	static void set_cr0so(void *instance);
	static byte_t mem_read_byte(void *instance, addr_t addr);
	static void mem_write_byte(void *instance, addr_t addr, byte_t value);
	static halfword_t mem_read_half_word(void *instance, addr_t addr);
	static void mem_write_half_word(void *instance, addr_t addr, halfword_t value);
	static word_t mem_read_word(void *instance, addr_t addr);
	static void mem_write_word(void *instance, addr_t addr, word_t value);
	static dword_t mem_read_dword(void *instance, addr_t addr);
	static void mem_write_dword(void *instance, addr_t addr, dword_t value);
	static void mem_set(void *instance, addr_t addr, byte_t value, int size);
	static void mem_read(void *instance, void *buf, addr_t addr, int size);
	static void mem_write(void *instance, addr_t addr, void *buf, int size);

	PowerPC(const sc_module_name& name);
	
	void Emulate(int ninstructions);
	
	static void sigint_handler(int signum);
	
	void Stop(int ninstructions);
};

#endif
