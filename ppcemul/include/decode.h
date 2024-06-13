/***************************************************************************
                          decode.h  -  description
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

#ifndef __DECODE_H__
#define __DECODE_H__

#include <misc.h>
#include <translate.h>
#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif
extern void ppc_init_decode();
extern void ppc_execute(ppc_inst_t inst);
extern void ppc_execute_check_path(ppc_inst_t inst, addr_t pc);
extern char *ppc_disassemble(ppc_inst_t inst, addr_t pc, char *buf);
extern char *ppc_disassemble2(ppc_inst_t inst, addr_t pc);
extern void ppc_disassemble_to_stream(ppc_inst_t inst, addr_t pc, FILE *stream);
extern void ppc_xmlize(ppc_inst_t inst, addr_t pc, char *buf);
extern void ppc_xmlize_to_stream(ppc_inst_t, addr_t pc, FILE *stream);
extern char *ppc_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern void ppc_bin2c_to_stream(ppc_inst_t, addr_t pc, FILE *stream);

extern void ppc_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void ppc_dump_instruction_counters();
extern void ppc_clear_instruction_counters();

extern BOOL ppc_is_a_branch(ppc_inst_t inst);
extern BOOL ppc_is_conditional_branch(ppc_inst_t inst);
extern BOOL ppc_is_unconditional_branch(ppc_inst_t inst);
extern addr_t ppc_get_unconditional_branch_target_addr(ppc_inst_t inst, addr_t pc);
extern addr_t ppc_get_conditional_branch_target_addr(ppc_inst_t inst, addr_t pc);

#if defined(__cplusplus)
}
#endif

#endif
