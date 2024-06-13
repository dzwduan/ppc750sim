/***************************************************************************
                          branch.h  -  description
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

#ifndef __BRANCH_H__
#define __BRANCH_H__

#include <misc.h>
#include <translate.h>

#if defined(__cplusplus)
extern "C" {
#endif
extern void bx_impl(ppc_inst_t inst);
extern void bcx_impl(ppc_inst_t inst);
extern void bcctrx_impl(ppc_inst_t inst);
extern void bclrx_impl(ppc_inst_t inst);


extern char *bx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *bcx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *bcctrx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *bclrx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);


extern char *bx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *bcx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *bcctrx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *bclrx_disasm(ppc_inst_t inst, addr_t pc, char *buf);


extern void bx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void bcx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void bcctrx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void bclrx_translate(ppc_inst_t inst, struct DecodedInstruction *di);



extern void bx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void bcx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void bcctrx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void bclrx_xml(ppc_inst_t inst, addr_t pc, char *buf);


#if defined(__cplusplus)
}
#endif

#endif
