/***************************************************************************
                          translate.h  -  description
                             -------------------
    begin                : Wed Sep 26 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@cea.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __TRANSLATE_H__
#define __TRANSLATE_H__

#if defined(__cplusplus)
#include <iostream>
#endif

#include "misc.h"
	
typedef SInt8 fieldnum_t;
typedef SInt8 regnum_t;
typedef SInt8 sprnum_t;
typedef int tbrnum_t;
typedef SInt8 bitnum_t;

typedef int OperationIdent;

typedef int InstructionClass;
	
#define	ICIntArithmetic   0x00000001
#define ICIntLogical      0x00000002
#define ICIntRotate       0x00000004
#define ICIntShift        0x00000008
#define	ICIntMul          0x00000010
#define	ICIntDiv          0x00000020
#define	ICIntLoad         0x00000040
#define	ICIntStore        0x00000080
#define	ICBranch          0x00000100
#define	ICMoveToSpr       0x00000200
#define ICMoveFromSpr     0x00000400
#define	ICCRLogical       0x00000800
#define ICMoveFromCR      0x00001000
#define ICSystemCall      0x00002000
#define ICFloatArithmetic 0x00004000
#define ICFloatMul        0x00008000
#define ICFloatDiv        0x00010000
#define ICFloatMulAdd     0x00020000
#define ICFloatMove       0x00040000
#define ICFloatConversion 0x00080000
#define ICFloatLoad       0x00100000
#define ICFloatStore      0x00200000
#define ICMoveFromFPSCR   0x00400000
#define ICMoveToFPSCR     0x00800000
#define ICMoveToCrf       0x01000000
#define ICMoveCrf         0x02000000
#define ICMoveFromTbr     0x04000000

#define ICIntALU (ICIntArithmetic | ICIntLogical | ICIntRotate | ICIntShift)
#define ICIntMulDiv (ICIntMul | ICIntDiv)
#define ICIntLoadStore (ICIntLoad | ICIntStore)
#define ICFloatLoadStore (ICFloatLoad | ICFloatStore)
#define ICLoadStore (ICIntLoadStore | ICFloatLoadStore)
#define ICFloat (ICFloatArithmetic | ICFloatMul | ICFloatDiv | ICFloatMulAdd | ICFloatMove | ICMoveFromFPSCR | ICMoveToFPSCR | ICFloatConversion)

enum
{
	ID_ADDI,
	ID_ADDIC,
	ID_ADDICD,
	ID_ADDIS,
	ID_ANDID,
	ID_ANDISD,
	ID_CMPI,
	ID_CMPLI,
	ID_MULLI,
	ID_ORI,
	ID_ORIS,
	ID_SUBFIC,
	ID_XORI,
	ID_XORIS,
	ID_RLWIMI,
	ID_RLWIMID,
	ID_RLWINM,
	ID_RLWINMD,
	ID_RLWNM,
	ID_RLWNMD,
	ID_AND,
	ID_ANDD,
	ID_ANDC,
	ID_ANDCD,
	ID_CMP,
	ID_CMPL,
	ID_CNTLZW,
	ID_CNTLZWD,
	ID_EQV,
	ID_EQVD,
	ID_EXTSB,
	ID_EXTSBD,
	ID_EXTSH,
	ID_EXTSHD,
	ID_NAND,
	ID_NANDD,
	ID_NOR,
	ID_NORD,
	ID_OR,
	ID_ORD,
	ID_ORC,
	ID_ORCD,
	ID_SLW,
	ID_SLWD,
	ID_SRAW,
	ID_SRAWD,
	ID_SRAWI,
	ID_SRAWID,
	ID_SRW,
	ID_SRWD,
	ID_XOR,
	ID_XORD,
	ID_ADD,
	ID_ADDD,
	ID_ADDO,
	ID_ADDOD,
	ID_ADDC,
	ID_ADDCD,
	ID_ADDCO,
	ID_ADDCOD,
	ID_ADDE,
	ID_ADDED,
	ID_ADDEO,
	ID_ADDEOD,
	ID_ADDME,
	ID_ADDMED,
	ID_ADDMEO,
	ID_ADDMEOD,
	ID_ADDZE,
	ID_ADDZED,
	ID_ADDZEO,
	ID_ADDZEOD,
	ID_DIVW,
	ID_DIVWD,
	ID_DIVWO,
	ID_DIVWOD,
	ID_DIVWU,
	ID_DIVWUD,
	ID_DIVWUO,
	ID_DIVWUOD,
	ID_MULHW,
	ID_MULHWD,
	ID_MULHWO,
	ID_MULHWOD,
	ID_MULHWU,
	ID_MULHWUD,
	ID_MULHWUO,
	ID_MULHWUOD,
	ID_MULLW,
	ID_MULLWD,
	ID_MULLWO,
	ID_MULLWOD,
	ID_NEG,
	ID_NEGD,
	ID_NEGO,
	ID_NEGOD,
	ID_SUBF,
	ID_SUBFD,
	ID_SUBFO,
	ID_SUBFOD,
	ID_SUBFC,
	ID_SUBFCD,
	ID_SUBFCO,
	ID_SUBFCOD,
	ID_SUBFE,
	ID_SUBFED,
	ID_SUBFEO,
	ID_SUBFEOD,
	ID_SUBFME,
	ID_SUBFMED,
	ID_SUBFMEO,
	ID_SUBFMEOD,
	ID_SUBFZE,
	ID_SUBFZED,
	ID_SUBFZEO,
	ID_SUBFZEOD,
	ID_LBZ,
	ID_LBZU,
	ID_LFD,
	ID_LFDU,
	ID_LFS,
	ID_LFSU,
	ID_LHA,
	ID_LHAU,
	ID_LHZ,
	ID_LHZU,
	ID_LMW,
	ID_LWZ,
	ID_LWZU,
	ID_STB,
	ID_STBU,
	ID_STFD,
	ID_STFDU,
	ID_STFS,
	ID_STFSU,
	ID_STH,
	ID_STHU,
	ID_STMW,
	ID_STW,
	ID_STWU,
	ID_LBZUX,
	ID_LBZX,
	ID_LFDUX,
	ID_LFDX,
	ID_LFSUX,
	ID_LFSX,
	ID_LHAUX,
	ID_LHAX,
	ID_LHBRX,
	ID_LHZUX,
	ID_LHZX,
	ID_LSWI,
	ID_LSWX,
	ID_LWARX,
	ID_LWBRX,
	ID_LWZUX,
	ID_LWZX,
	ID_STBUX,
	ID_STBX,
	ID_STFDUX,
	ID_STFDX,
	ID_STFIWX,
	ID_STFSUX,
	ID_STFSX,
	ID_STHBRX,
	ID_STHUX,
	ID_STHX,
	ID_STSWI,
	ID_STSWX,
	ID_STWBRX,
	ID_STWCXD,
	ID_STWUX,
	ID_STWX,
	ID_BC,
	ID_B,
	ID_BCCTR,
	ID_BCLR,
	ID_SC,
	ID_MFFS,
	ID_MFFSD,
	ID_MFCR,
	ID_MFSPR,
	ID_MTCRF,
	ID_MTSPR,
	ID_CRAND,
	ID_CRANDC,
	ID_CREQV,
	ID_CRNAND,
	ID_CRNOR,
	ID_CROR,
	ID_CRORC,
	ID_CRXOR,
	ID_FADDS,
	ID_FADDSD,
	ID_FDIVS,
	ID_FDIVSD,
	ID_FMADDS,
	ID_FMADDSD,
	ID_FMSUBS,
	ID_FMSUBSD,
	ID_FMULS,
	ID_FMULSD,
	ID_FNMADDS,
	ID_FNMADDSD,
	ID_FNMSUBS,
	ID_FNMSUBSD,
	ID_FRES,
	ID_FRESD,
	ID_FSQRTS,
	ID_FSQRTSD,
	ID_FSUBS,
	ID_FSUBSD,
	ID_FABS,
	ID_FABSD,
	ID_FCMPO,
	ID_FCMPU,
	ID_FCTIW,
	ID_FCTIWD,
	ID_FCTIWZ,
	ID_FCTIWZD,
	ID_FMR,
	ID_FMRD,
	ID_FNABS,
	ID_FNABSD,
	ID_FNEG,
	ID_FNEGD,
	ID_FRSP,
	ID_FRSPD,
	ID_FADD,
	ID_FADDD,
	ID_FDIV,
	ID_FDIVD,
	ID_FMADD,
	ID_FMADDD,
	ID_FMSUB,
	ID_FMSUBD,
	ID_FMUL,
	ID_FMULD,
	ID_FNMADD,
	ID_FNMADDD,
	ID_FNMSUB,
	ID_FNMSUBD,
	ID_FRSQRTE,
	ID_FRSQRTED,
	ID_FSEL,
	ID_FSELD,
	ID_FSQRT,
	ID_FSQRTD,
	ID_FSUB,
	ID_FSUBD,
	
	ID_MTFSB0,
	ID_MTFSB0D,
	ID_MTFSB1,
	ID_MTFSB1D,
	ID_MTFSFID,
	ID_MTFSF,
	ID_MTFSFD,
	ID_MTFSFI,
	ID_DCBZ,
	
	ID_MCRF,
	ID_MFTB
};

/* Infos de d�codage d'une instruction enti�re */
struct IntegerOperation
{
	/* informations de d�codage */
	OperationIdent ident;
	
	BOOL nullRA : 1;
	union
	{				
		struct
		{
			fieldnum_t crfD; /* num�ro du champ de CR */
		} compare;
		
		struct
		{
			UInt8 sh : 5;
			UInt8 mb : 5;
			UInt8 me : 5;
		} rotate;
		
		struct
		{
			UInt8 sh : 5;
		} shift;
	} extra;
	
#if defined(__cplusplus)
	int operator == (const IntegerOperation& io) const
	{
		return 0;
	}
	
	friend ostream& operator << (ostream& os, const IntegerOperation& iop)
	{
		os << "IntegerOperation";
		return os;
	}
	operator bool () const
	{
		return true;
	}
#endif
};

struct FloatingPointOperation
{
	OperationIdent ident;
#if defined(__cplusplus)
	int operator == (const FloatingPointOperation& fpo) const
	{
		return 0;
	}
	
	friend ostream& operator << (ostream& os, const FloatingPointOperation& fpop)
	{
		os << "FloatingPointOperation";
		return os;
	}
	operator bool () const
	{
		return true;
	}
#endif
};

/* Infos de decodage d'une instruction load/store */
struct LoadStoreOperation
{
	/* informations de d�codage */
	OperationIdent ident;
	BOOL nullRA : 1; /* RA == 0 */
	BOOL algebraic : 1; /* true si extension de signe */
	BOOL write : 1; /* true si store, false si load */
	BOOL floatingPoint : 1;
	UInt8 size; /* taille de l'acc�s en m�moire en octets */
	
	union
	{
		struct
		{
			UInt8 nb; /* pour instruction travaillant sur plusieurs mots */
		} string;		
	} extra;
	
	#if defined(__cplusplus)
	int operator == (const LoadStoreOperation& lso) const
	{
		return 0;
	}
	friend ostream& operator << (ostream& os, const LoadStoreOperation& lsop)
	{
		os << "LoadStoreOperation";
		return os;
	}
	operator bool () const
	{
		return true;
	}
	#endif
};

/* Infos de decodage d'une instruction de branchement */
struct BranchOperation
{
	OperationIdent ident;
	
	union
	{
		struct
		{
			BOOL aa : 1;
			BOOL lk : 1;
		} b;
		
		struct
		{
			UInt8 bi : 5;
			BOOL aa : 1;
			BOOL lk : 1;
			UInt8 bo : 5;
		} bc;
	
		struct
		{
			UInt8 bi : 5;
			BOOL lk : 1;
			UInt8 bo : 5;
		} bcctr;
		
		struct
		{
			UInt8 bi : 5;
			BOOL lk : 1;
			UInt8 bo : 5;
		} bclr;
	} extra;

	#if defined(__cplusplus)
	int operator == (const BranchOperation& bro) const
	{
		return 0;
	}
	friend ostream& operator << (ostream& os, const BranchOperation& brop)
	{
		os << "BranchOperation";
		return os;
	}
	operator bool () const
	{
		return true;
	}
	#endif
};

struct SystemRegisterOperation
{
	OperationIdent ident;
	
	union
	{
		struct
		{
			sprnum_t spr;
		} mfspr;
		
		struct
		{
			regnum_t gpr;
		} mtspr;
	
		struct
		{
			bitnum_t srcCrb[2]; /* numero de bit source de CR */
			bitnum_t dstCrb; /* numero de bit destination de CR */
		} crlogical;
		
		struct
		{
			regnum_t gpr;
			UInt8 crm;
		} mtcrf;
		
		struct
		{
			fieldnum_t srcCrf;
		} mcrf;
		
		struct
		{
			tbrnum_t tbr;
		} mftb;
	} extra;

	#if defined(__cplusplus)
	int operator == (const SystemRegisterOperation& sro) const
	{
		return 0;
	}
	
	friend ostream& operator << (ostream& os, const SystemRegisterOperation& srop)
	{
		os << "SystemRegisterOperation";
		return os;
	}
	operator bool () const
	{
		return true;
	}
	#endif
};

struct DecodedInstruction
{
	InstructionClass iclass; /* classe de l'instruction */
	BOOL hasImmed;
	UInt32 immed;
	
	union
	{
		struct IntegerOperation integer;
		struct LoadStoreOperation loadStore;
		struct BranchOperation branch;
		struct SystemRegisterOperation systemRegister;
		struct FloatingPointOperation floatingPoint;
	} operation;
	
	union
	{
		struct
		{
			regnum_t srcReg[2]; /* les deux registres sources */
			regnum_t dstReg; /* le registre destination */
			BOOL outCR : 1; /* le champ du registre CR est modifi� par l'instruction */
			fieldnum_t crf; /* numero de champ du registre CR */
			BOOL inCarry : 1; /* l'instruction lit XER[CA] */
			BOOL outCarry : 1; /* l'instruction ecrit XER[CA] */
			BOOL outOverflow : 1;	/* l'instruction ecrit XER[OV] et XER[SO] */
		} integer;
		
		struct
		{
			regnum_t srcReg[3]; /* les deux registres sources */
			regnum_t dstReg[2]; /* les deux registres destinations */
		} loadStore;
		
		struct
		{
			fieldnum_t crf;
			BOOL inCR : 1;
			BOOL inLR : 1;
			BOOL inCTR : 1;
			BOOL outLR : 1;
			BOOL outCTR : 1;
		} branch;
		
		struct
		{
			regnum_t srcReg;
			sprnum_t dstSpr; /* user mode : 1 = XER, 8 = LR, 9 = CTR */
		} mtspr;
		
		struct
		{
			sprnum_t srcSpr; /* user mode : 1 = XER, 8 = LR, 9 = CTR */
			regnum_t dstReg;
		} mfspr;
		
		struct
		{
			fieldnum_t srcCrf[2]; /* numero de champ source de CR */
			fieldnum_t dstCrf; /* numero de champ destination de CR */
		} crlogical;
		
		struct
		{
			regnum_t dstReg;
		} mfcr;
		
		struct
		{
			regnum_t srcReg[3];
			regnum_t dstReg;
			BOOL inFPSCR : 1;
			BOOL outCR : 1;
			fieldnum_t crf;
		} floatingPoint;
		
		struct
		{
			regnum_t srcReg;
			fieldnum_t dstCrf;
		} mtcrf;
		
		struct
		{
			fieldnum_t srcCrf;
			fieldnum_t dstCrf;
		} mcrf;
		
		struct
		{
			tbrnum_t srcTbr;
			regnum_t dstReg;
		} mftb;
	} io;
};

#endif
