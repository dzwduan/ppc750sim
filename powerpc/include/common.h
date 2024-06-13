/***************************************************************************
                          common.h  -  types de donn�es du PowerPC
                             -------------------
    begin                : Tue Feb 27 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@cea.fr, mouchard@lri.fr
 ***************************************************************************/

#ifndef __COMMON_H__
#define __COMMON_H__

#include <systemc.h>
#include <ppcemul.h>
#include <parms.h>
#include <iostream>
#include <string.h>
#include <trace.h>

typedef int UnitIdent;

const UnitIdent IntegerUnitIdent = 0;
const UnitIdent LoadStoreUnitIdent = 1;
const UnitIdent BranchUnitIdent = 2;
const UnitIdent SystemRegisterUnitIdent = 3;
const UnitIdent FloatingPointUnitIdent = 4;

typedef SInt8 tag_t;

extern void sc_trace(sc_trace_file *tf, const UInt64& v, const sc_string& NAME);
extern void sc_trace(sc_trace_file *tf, const SInt64& v, const sc_string& NAME);

inline void WriteHex(ostream& os, UInt64 value)
{
	static char hexChar[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	
	os << "0x";
	os << hexChar[(value >> 60) & 15];
	os << hexChar[(value >> 56) & 15];
	os << hexChar[(value >> 52) & 15];
	os << hexChar[(value >> 48) & 15];
	os << hexChar[(value >> 44) & 15];
	os << hexChar[(value >> 40) & 15];
	os << hexChar[(value >> 36) & 15];
	os << hexChar[(value >> 32) & 15];
	os << hexChar[(value >> 28) & 15];
	os << hexChar[(value >> 24) & 15];
	os << hexChar[(value >> 20) & 15];
	os << hexChar[(value >> 16) & 15];
	os << hexChar[(value >> 12) & 15];
	os << hexChar[(value >> 8) & 15];
	os << hexChar[(value >> 4) & 15];
	os << hexChar[value & 15];
}

inline void WriteHex(ostream& os, UInt32 value)
{
	static char hexChar[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	
	os << "0x";
	os << hexChar[(value >> 28) & 15];
	os << hexChar[(value >> 24) & 15];
	os << hexChar[(value >> 20) & 15];
	os << hexChar[(value >> 16) & 15];
	os << hexChar[(value >> 12) & 15];
	os << hexChar[(value >> 8) & 15];
	os << hexChar[(value >> 4) & 15];
	os << hexChar[value & 15];
}

inline void WriteHex(ostream& os, UInt8 value)
{
	static char hexChar[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	
	os << "0x";
	os << hexChar[(value >> 4) & 15];
	os << hexChar[value & 15];
}


/* Representations des registres (GPR, FP, CR, LR, CTR) pour appliquer Tomasulo's */
struct Register64RSEntry
{
	tag_t tag; /* numero du registre de renommage */
	BOOL valid; /* true ssi data est valide */
	UInt64 data; /* valeur du registre */
	
	Register64RSEntry()
	{
		tag = -1;
		valid = false;
		data = 0;
	}
};

struct IntegerRegisterRSEntry
{
	tag_t tag; /* numero du registre de renommage */
	BOOL valid; /* true ssi data est valide */
	UInt32 data; /* valeur du registre */
	
	IntegerRegisterRSEntry()
	{
		tag = -1;
		valid = false;
		data = 0;
	}
};

struct ConditionRegisterRSEntry
{
	tag_t tag; /* numero du registre de renommage */
	BOOL valid; /* true ssi condition est valide */
	UInt8 crfield; /* CR[BI] */
	
	ConditionRegisterRSEntry()
	{
		tag = -1;
		valid = false;
		crfield = 0;
	}
};

struct LinkRegisterRSEntry
{
	tag_t tag; /* numero du registre de renommage */
	BOOL valid; /* true ssi data est valide */
	UInt32 data; /* valeur du registre */

	LinkRegisterRSEntry()
	{
		tag = -1;
		valid = false;
		data = 0;
	}
};

struct CountRegisterRSEntry
{
	tag_t tag; /* numero du registre de renommage */
	BOOL valid; /* true ssi data est valide */
	UInt32 data; /* valeur du registre */

	CountRegisterRSEntry()
	{
		tag = -1;
		valid = false;
		data = 0;
	}
};

struct FloatingPointRegisterRSEntry
{
	tag_t tag; /* numero du registre de renommage */
	BOOL valid; /* true ssi data est valide */
	UInt64 data; /* valeur du registre */
	
	FloatingPointRegisterRSEntry()
	{
		tag = -1;
		valid = false;
		data = 0;
	}
};


/* Operandes pour une instruction enti�re */
struct IntegerOperands
{
	UInt32 data[2]; // Peut contenir un immediat
	
	IntegerOperands()
	{
		data[0] = data[1] = 0;
	}
	
	int operator == (const IntegerOperands& iop) const
	{
		return 0;
	}

	friend ostream& operator << (ostream& os, const IntegerOperands& iop)
	{
		os << "IntegerOperands(";
		WriteHex(os, iop.data[0]);
		os << ", ";
		WriteHex(os, iop.data[1]);
		os << ")";
		return os;
	}

	operator bool () const
	{
		return true;
	}
};

extern void sc_trace(sc_trace_file *tf, const IntegerOperands& iop, const sc_string& NAME);

/* Tags (registres de renommage) pour une instruction enti�re */
struct IntegerTags
{
	tag_t tag; /* tag de l'instruction (numero dans le ROB) */
	tag_t resultTag; /* tag du registre destination */
	tag_t CRTag; /* tag du champ du registre CR */
	
	int operator == (const IntegerTags& it) const
	{
		return 0;
	}

	friend ostream& operator << (ostream& os, const IntegerTags& it)
	{
		os << "IntegerTags(tag = " << (int) it.tag << ", resultTag = " << (int) it.resultTag << ", CRTag = " << (int) it.CRTag << ")" << endl;
		return os;
	}

	operator bool () const
	{
		return true;
	}
};


extern void sc_trace(sc_trace_file *tf, const IntegerTags& it, const sc_string& NAME);

/* Operandes pour une instruction loadStore */
struct LoadStoreOperands
{
	//UInt32 data[3]; // Peut contenir un immediat
	
	UInt64 data0; // entier 32 bits/flottant 32 bits/flottant 64 bits
	UInt32 data1; // entier 32 bits
	UInt32 data2; // entier 32 bits
	
	LoadStoreOperands()
	{
//		data[0] = data[1] = data[2] = 0;
		data0 = data1 = data2 = 0;
	}
	
	int operator == (const LoadStoreOperands& ldop) const
	{
		return 0;
	}
	
	friend ostream& operator << (ostream& os, const LoadStoreOperands& ldop)
	{
		os << "LoadStoreOperands(";
		WriteHex(os, ldop.data0);
		os << ", ";
		WriteHex(os, ldop.data1);
		os << ", ";
		WriteHex(os, ldop.data2);
		os << ")";
		return os;
	}
	
	operator bool () const
	{
		return true;
	}
};

extern void sc_trace(sc_trace_file *tf, const LoadStoreOperands& lsop, const sc_string& NAME);

/* Tags (registres de renommage) pour une instruction loadStore */
struct LoadStoreTags
{
	tag_t tag; /* tag de l'instruction (numero dans le ROB) */
	tag_t resultTags[2]; /* tag du registres destinations et du registre recevant l'adresse effective (pour un update) */
	
	int operator == (const LoadStoreTags& ld) const
	{
		return 0;
	}

	friend ostream& operator << (ostream& os, const LoadStoreTags& ld)
	{
		os << "LoadStoreTags(tag = " << (int) ld.tag << ", resultTags = " << (int) ld.resultTags[0] << ", " << (int) ld.resultTags[1] << ")" << endl;
		return os;
	}
	
	operator bool () const
	{
		return true;
	}
};

extern void sc_trace(sc_trace_file *tf, const LoadStoreTags& lst, const sc_string& NAME);

/* Op�randes pour une instruction Branch */
struct BranchOperands
{
	UInt32 immed;
	UInt32 pc;
	UInt8 crfield;
	UInt32 lr;
	UInt32 ctr;
	bool branchPredictedTaken;

	BranchOperands()
	{
		immed = pc = crfield = lr = ctr = 0;
		branchPredictedTaken = false;
	}
		
	int operator == (const BranchOperands& bro) const
	{
		return 0;
	}

	friend ostream& operator << (ostream& os, const BranchOperands& bro)
	{
		os << "BranchOperands(immed = ";
		WriteHex(os, bro.immed);
		os << ", pc = ";
		WriteHex(os, bro.pc);
		os << ", crfield = " << (int) bro.crfield;
		os << ", lr = ";
		WriteHex(os, bro.lr);
		os << ", ctr = ";
		WriteHex(os, bro.ctr);
		os << ", branchPredictedTaken = " << bro.branchPredictedTaken;
		os << ")";
		return os;
	}
	
	operator bool () const
	{
		return true;
	}
};

extern void sc_trace(sc_trace_file *tf, const BranchOperands& brop, const sc_string& NAME);

struct BranchTags
{
	tag_t tag; /* tag de l'instruction (numero dans le ROB), -1 s'il est n'est pas dans le ROB */
	tag_t LRTag; /* tag du registre LR */
	tag_t CTRTag; /* tag du registre CTR */
	
	int operator == (const BranchTags& brt) const
	{
		return 0;
	}

	friend ostream& operator << (ostream& os, const BranchTags& brt)
	{
		os << "BranchTags(tag = " << (int) brt.tag << ", LRTag = " << (int) brt.LRTag << ", " << (int) brt.LRTag << ", CTRTag = " << (int) brt.CTRTag << ")";
		return os;
	}

	operator bool () const
	{
		return true;
	}
};

extern void sc_trace(sc_trace_file *tf, const BranchTags& brt, const sc_string& NAME);

struct SystemRegisterTags
{
	tag_t tag; /* tag de l'instruction (numero dans le ROB) */
	tag_t resultTag; /* tag du registre g�n�ral */
	tag_t CRTag; /* tag du registre CR */
	tag_t LRTag; /* tag du registre LR */
	tag_t CTRTag; /* tag du registre CTR */
	
	int operator == (const SystemRegisterTags& srt) const
	{
		return 0;
	}
	
	friend ostream& operator << (ostream& os, const SystemRegisterTags& srt)
	{
		os << "SystemRegisterTags(tag = " << (int) srt.tag << ", resultTag = " <<
		(int) srt.resultTag << ", CRTag = " << (int) srt.CRTag << ", LRTag = " << (int) srt.LRTag << ", CTRTag = " << (int) srt.CTRTag << ")";
		return os;
	}
	
	operator bool () const
	{
		return true;
	}
};

extern void sc_trace(sc_trace_file *tf, const SystemRegisterTags& srt, const sc_string& NAME);

struct FloatingPointOperands
{
	UInt64 data[3];
	
	int operator == (const FloatingPointOperands& fpo) const
	{
		return 0;
	}
	
	double GetDouble(int i) { return *(double *) &data[i]; }
	float GetSingle(int i) { return *(float *) &data[i]; }

	friend ostream& operator << (ostream& os, const FloatingPointOperands& fpo)
	{
		os << "FloatingPointOperands(";
		WriteHex(os, fpo.data[0]);
		os << ", ";
		WriteHex(os, fpo.data[1]);
		os << ", ";
		WriteHex(os, fpo.data[2]);
		os << ")";
		return os;
	}
	
	operator bool () const
	{
		return true;
	}
};

extern void sc_trace(sc_trace_file *tf, const FloatingPointOperands& fpop, const sc_string& NAME);

struct FloatingPointTags
{
	tag_t tag; /* tag de l'instruction (numero dans le ROB) */
	tag_t resultTag; /* tag du registre general */
	tag_t CRTag; /* tag du registre CR */	

	int operator == (const FloatingPointTags& fpt) const
	{
		return 0;
	}

	friend ostream& operator << (ostream& os, const FloatingPointTags& fpt)
	{
		os << "FloatingPointTags(tag = " << (int) fpt.tag << ", resultTag = " <<
		(int) fpt.resultTag << ", CRTag = " << (int) fpt.CRTag << ")";
		return os;
	}
	
	operator bool () const
	{
		return true;
	}
};

extern void sc_trace(sc_trace_file *tf, const FloatingPointTags& fpt, const sc_string& NAME);

/* structure pour �changer des donn�es entre l'unit� load/store, le cache, l'interface de bus, et sur le bus de donn�es */
template <int size>
struct DataArray
{
	UInt8 buffer[size];
	
	int operator == (const DataArray& da) const
	{
		return 0;
	}
	
	UInt8& operator [] (int index)
	{
		return buffer[index];
	}
	
	friend ostream& operator << (ostream& os, const DataArray& da)
	{
		int i;
		
		for(i = 0; i < size; i++)
		{
			WriteHex(os, da.buffer[i]);
			if(i != size - 1) os << " ";
		}
		return os;
	}

	operator bool () const
	{
		return true;
	}
};

template <int size>
void sc_trace(sc_trace_file *tf, const DataArray<size>& da, const sc_string& NAME)
{
	int i;
	for(i = 0; i < size; i++)
		sc_trace(tf, da.buffer[i], NAME + ".buffer");
}

template <class T>
class Dummy
{
	public:
		Dummy() {}
		Dummy(T value) { this->value = value; }
		operator T () const { return value; }
		int operator == (const Dummy& d) const { return 0; }
		
		friend ostream& operator << (ostream& os, const Dummy<T>& d)
		{
			os << d.value;
			return os;
		}
		operator bool () const
		{
			return true;
		}
	private:
		T value;
};

template <class T>
void sc_trace(sc_trace_file *tf, const Dummy<T>& d, const sc_string& NAME)
{
	sc_trace(tf, d.value, NAME);
}
template<>
class Dummy<bool>
{
	public:
		Dummy() {}
		Dummy(bool value) { this->value = value; }
		operator bool () const { return value; }
		int operator == (const Dummy<bool>& d) const { return value == false && d.value == false; }
		friend ostream& operator << (ostream& os, const Dummy<bool>& d)
		{
			os << d.value;
			return os;
		}
		bool value;
};

extern void SystemCall();
extern bool Debug(int what);
extern bool Trace(int what);

const int DebugAll = -1;
const int DebugBHT = 1;
const int DebugBTIC = 1 << 1;
const int DebugBranchPredictionUnit = 1 << 2;
const int DebugBranchRS = 1 << 3;
const int DebugBranchUnit = 1 << 4;
const int DebugBusInterfaceUnit = 1 << 5;
const int DebugCache = 1 << 6;
const int DebugCompletionUnit = 1 << 7;
const int DebugConditionRegister = 1 << 8;
const int DebugCountRegister = 1 << 9;
const int DebugDispatchUnit = 1 << 10;
const int DebugFetchUnit = 1 << 11;
const int DebugFloatingPointRS = 1 << 12;
const int DebugFloatingPointUnit = 1 << 13;
const int DebugInstructionQueue = 1 << 14;
const int DebugIntegerRS = 1 << 15;
const int DebugIntegerUnit = 1 << 16;
const int DebugLinkRegister = 1 << 17;
const int DebugLoadStoreRS = 1 << 18;
const int DebugLoadStoreUnit = 1 << 19;
const int DebugPowerPC = 1 << 20;
const int DebugRegisterFile = 1 << 21;
const int DebugRegistersBinding = 1 << 22;
const int DebugReorderBuffer = 1 << 23;
const int DebugStoreQueue = 1 << 24;
const int DebugSystemRegisterRS = 1 << 25;
const int DebugSystemRegisterUnit = 1 << 26;
const int DebugXERRegister = 1 << 27;

const int TraceAll = -1;
const int TraceBHT = 1;
const int TraceBTIC = 1 << 1;
const int TraceBranchPredictionUnit = 1 << 2;
const int TraceBranchRS = 1 << 3;
const int TraceBranchUnit = 1 << 4;
const int TraceBusInterfaceUnit = 1 << 5;
const int TraceCache = 1 << 6;
const int TraceCompletionUnit = 1 << 7;
const int TraceConditionRegister = 1 << 8;
const int TraceCountRegister = 1 << 9;
const int TraceDispatchUnit = 1 << 10;
const int TraceFetchUnit = 1 << 11;
const int TraceFloatingPointRS = 1 << 12;
const int TraceFloatingPointUnit = 1 << 13;
const int TraceInstructionQueue = 1 << 14;
const int TraceIntegerRS = 1 << 15;
const int TraceIntegerUnit = 1 << 16;
const int TraceLinkRegister = 1 << 17;
const int TraceLoadStoreRS = 1 << 18;
const int TraceLoadStoreUnit = 1 << 19;
const int TracePowerPC = 1 << 20;
const int TraceRegisterFile = 1 << 21;
const int TraceRegistersBinding = 1 << 22;
const int TraceReorderBuffer = 1 << 23;
const int TraceStoreQueue = 1 << 24;
const int TraceSystemRegisterRS = 1 << 25;
const int TraceSystemRegisterUnit = 1 << 26;
const int TraceXERRegister = 1 << 27;

extern void StopSimulation();

#define ABORT() { StopSimulation(); cout << endl << "abort !!!" << endl; return; }

#ifndef SC_HAS_PROCESS
#define SC_HAS_PROCESS(user_module_name)                                      \
    typedef user_module_name SC_CURRENT_USER_MODULE
#endif

extern double sim_time_stamp();

#endif
