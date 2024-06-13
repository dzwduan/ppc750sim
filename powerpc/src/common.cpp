/***************************************************************************
                          common.cpp  -  types de données du PowerPC
                             -------------------
    begin                : Mon Oct 29 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <common.h>

void sc_trace(sc_trace_file *tf, const UInt64& v, const sc_string& NAME)
{
	sc_trace(tf, (UInt32)(v >> 32), NAME + " (hi)");
	sc_trace(tf, (UInt32) v, NAME + " (lo)");
}

void sc_trace(sc_trace_file *tf, const SInt64& v, const sc_string& NAME)
{
	sc_trace(tf, (UInt32)(v >> 32), NAME + " (hi)");
	sc_trace(tf, (UInt32) v, NAME + " (lo)");
}

void sc_trace(sc_trace_file *tf, const IntegerOperands& iop, const sc_string& NAME)
{
	sc_trace(tf, iop.data[0], NAME + ".data[0]");
	sc_trace(tf, iop.data[1], NAME + ".data[1]");
}

void sc_trace(sc_trace_file *tf, const IntegerTags& it, const sc_string& NAME)
{
}

void sc_trace(sc_trace_file *tf, const LoadStoreOperands& lsop, const sc_string& NAME)
{
}

void sc_trace(sc_trace_file *tf, const LoadStoreTags& lst, const sc_string& NAME)
{
}

void sc_trace(sc_trace_file *tf, const BranchOperands& brop, const sc_string& NAME)
{
}

void sc_trace(sc_trace_file *tf, const BranchTags& brt, const sc_string& NAME)
{
}

void sc_trace(sc_trace_file *tf, const SystemRegisterTags& srt, const sc_string& NAME)
{
}

void sc_trace(sc_trace_file *tf, const FloatingPointOperands& fpop, const sc_string& NAME)
{
}

void sc_trace(sc_trace_file *tf, const FloatingPointTags& fpt, const sc_string& NAME)
{
}
