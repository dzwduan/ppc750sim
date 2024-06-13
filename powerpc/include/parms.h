/***************************************************************************
              parms.h  -  High level microarchitecture parameters
                             -------------------
    begin                : Tue Mar 20 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@cea.fr, mouchard@lri.fr
 ***************************************************************************/

#ifndef __PARMS_H__
#define __PARMS_H__

/********************************************
* High level microarchitecture parameters  *
********************************************/

/* Dispatch/Completion calibration */
const int nInstructionQueueEntries = 6; /* Size of the instruction queue */
const int nBranchQueueEntries = 1; /* Size of the branch queue (for branch folding) */
const int nMaxBranchsBeforeFetchStall = 2; /* Fetch stop when branch count reach this limit */
const int nMaxBranchsBeforeDispatchStall = 1; /* Dispatch stop when branch count reach this limit */
const int nIntegerRS = 1; /* Number of reservation stations per integer unit */
const int nLoadStoreRS = 2; /* Number of reservation stations for the load/store unit */
const int nBranchRS = 1; /* Number of reservation stations for the branch unit */
const int nFloatingPointRS = 1; /* Number of reservation stations per floating point units */
const int nSystemRegisterRS = 1; /* Number of reservation stations for the system register unit */
const int nIQ = 2; /* Max number of dispatched instructions per clock cycle */
const int nIQBranchs = 1; /* Max number of branch instructions dispatched per clock cycle */
const int nCQ = 2; /* Max number of commited instructions per clock cycle */
const int nReorderBufferEntries = 6; /* Reorder buffer size */

/* Units */
const int nIntegerUnit = 2; /* Number of integer units */
const int nFloatingPointUnits = 1; /* Number of floating point units */

/* Rename buffers */
const int nRenameBuffers = 6; /* Number of rename buffers */
const int nCRRenameBuffers = 8; /* Number of CR field rename buffers */
const int nLRRenameBuffers = 1; /* Number of LR rename buffers */
const int nCTRRenameBuffers = 1; /* Number of CTR rename buffers */
const int nFloatingPointRenameBuffers = 6; /* Number of floating point rename buffers */

/* Memory calibration */
const int nStoreQueueEntries = 2; /* Store queue size */
const int cachesize = 32768; /* Cache size (in bytes) for both the L1 Data and Instruction Cache */
const int associativity = 8; /* Cache associativity */
const int linesize = 32; /* Cache line size (in bytes) */
const int lsu2dcache = 8; /* Interface size (in bytes) between the load/store unit and the L1 data cache */
const int fetch2icache = 16; /* Interface size (in bytes) between the fetch unit and the instruction cache */
const int dcache2biu = 8; /* Interface size (in bytes) between the L1 data cache and the L2 Cache/Bus interface unit */
const int icache2biu = 8; /* Interface size (in bytes) between the L1 instruction cache and the L2 Cache/Bus interface unit */
const int memoryWriteLatency = 8; /* Memory Write Latency (cache -> bus interface unit for the moment) */
const int memoryReadLatency = 8; /* Memory Read Latency (cache -> bus interface unit for the moment) */

const int memoryWord = 8; /* Memory word size */

/* Branch prediction calibration */
const int nBHTEntries = 512; /* Branch History Table size */
const int nBHTLevels = 4; /* Number of level for the branch prediction */
const int nBTICEntries = 64; /* Branch Target Instruction Cache size (in entries) */
const int BTICAssociativity = 4; /* Branch Target Instruction cache associativity */
const int nInstructionsPerBTICEntry = 2; /* Number of instructions per BTIC entry */


/* Integer unit capabilities */
const int intflags[nIntegerUnit] = { ICIntALU | ICIntMulDiv, ICIntALU };



/***************************************************
* Some usefull computed parameters (do not modify) *
****************************************************/

const int nRegisters = 32; /* Number of general purpose registers */
const int nReadRegisterPorts = nIQ * 3 /* three operands max per instruction */ + 1 /* one port for the system register unit */;
const int nReadRenamePorts = nIQ * 3; /* three operands max per instruction */
const int nReadRenameCRPorts = 1; /* one port for the branch unit */
const int nReadRenameLRPorts = 1; /* one port for the branch unit */
const int nReadRenameCTRPorts = 1; /* one port for the branch unit */
const int SystemRegisterReadRegisterPort = nReadRegisterPorts - 1;
const int nFloatingPointRegisters = 32;
const int nReadFloatingPointRegisterPorts = nIQ * 3 /* three operands max per floating point instruction */;
const int nReadFloatingPointRenamePorts = nIQ * 3 /* three operands max per floating point instruction */;
const int IntegerWriteRenamePort = 0;
const int LoadStoreWriteRenamePort = IntegerWriteRenamePort + nIntegerUnit;
const int SystemRegisterWriteRenamePort = LoadStoreWriteRenamePort + 2;
const int nWriteRenamePorts = SystemRegisterWriteRenamePort + 1; /* Number of write port on rename buffers file */
const int FloatingPointWriteFloatingPointRenamePort = 0;
const int LoadStoreWriteFloatingPointRenamePort = FloatingPointWriteFloatingPointRenamePort + nFloatingPointUnits;
const int nWriteFloatingPointRenamePorts = LoadStoreWriteFloatingPointRenamePort + 1;
const int IntegerWriteRenameCRPort = 0;
const int FloatingPointWriteRenameCRPort = IntegerWriteRenameCRPort + nIntegerUnit;
const int SystemRegisterWriteRenameCRPort = FloatingPointWriteRenameCRPort + nFloatingPointUnits;
const int nWriteRenameCRPorts = SystemRegisterWriteRenameCRPort + 1;
const int BranchUnitWriteRenameLRPort = 0;
const int SystemRegisterWriteRenameLRPort = BranchUnitWriteRenameLRPort + 1;
const int nWriteRenameLRPorts = SystemRegisterWriteRenameLRPort + 1;
const int BranchUnitWriteRenameCTRPort = 0;
const int SystemRegisterWriteRenameCTRPort = BranchUnitWriteRenameCTRPort + 1;
const int nWriteRenameCTRPorts = SystemRegisterWriteRenameCTRPort + 1;
const int nWriteBackPorts = nCQ;
const int nWriteBackFloatingPointPorts = nCQ;
const int nWriteBackCRPorts = nCQ;
const int nWriteBackLRPorts = nCQ;
const int nWriteBackCTRPorts = nCQ;
const int nMemoryWriteBackPorts = nCQ;
const int nlines = cachesize / linesize / associativity;
const int nBTICLines = nBTICEntries / BTICAssociativity;
const UInt32 StackBase = 0x80000000;
const UInt32 MaxEnviron = 128 * 1024;
const UInt32 StackSize = 1024 * 1024;

#endif
