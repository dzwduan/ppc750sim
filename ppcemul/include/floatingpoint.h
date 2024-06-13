/***************************************************************************
                          floatingpoint.h  -  description
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

#ifndef __PPC_FLOATINGPOINT_H__
#define __PPC_FLOATINGPOINT_H__

#include <misc.h>
#include <translate.h>

#if defined(__cplusplus)
extern "C" {
#endif

extern BOOL IsNan(UInt64 x);
extern BOOL IsSNan(UInt64 x);

extern void faddsx_impl(ppc_inst_t inst);
extern void fdivsx_impl(ppc_inst_t inst);
extern void fmaddsx_impl(ppc_inst_t inst);
extern void fmsubsx_impl(ppc_inst_t inst);
extern void fmulsx_impl(ppc_inst_t inst);
extern void fnmaddsx_impl(ppc_inst_t inst);
extern void fnmsubsx_impl(ppc_inst_t inst);
extern void fresx_impl(ppc_inst_t inst);
extern void fsqrtsx_impl(ppc_inst_t inst);
extern void fsubsx_impl(ppc_inst_t inst);
extern void fabsx_impl(ppc_inst_t inst);
extern void fcmpo_impl(ppc_inst_t inst);
extern void fcmpu_impl(ppc_inst_t inst);
extern void fctiwx_impl(ppc_inst_t inst);
extern void fctiwzx_impl(ppc_inst_t inst);
extern void fmrx_impl(ppc_inst_t inst);
extern void fnabsx_impl(ppc_inst_t inst);
extern void fnegx_impl(ppc_inst_t inst);
extern void frspx_impl(ppc_inst_t inst);
extern void faddx_impl(ppc_inst_t inst);
extern void fdivx_impl(ppc_inst_t inst);
extern void fmaddx_impl(ppc_inst_t inst);
extern void fmsubx_impl(ppc_inst_t inst);
extern void fmulx_impl(ppc_inst_t inst);
extern void fnmaddx_impl(ppc_inst_t inst);
extern void fnmsubx_impl(ppc_inst_t inst);
extern void frsqrtex_impl(ppc_inst_t inst);
extern void fselx_impl(ppc_inst_t inst);
extern void fsqrtx_impl(ppc_inst_t inst);
extern void fsubx_impl(ppc_inst_t inst);




extern char *faddsx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fdivsx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fmaddsx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fmsubsx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fmulsx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fnmaddsx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fnmsubsx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fresx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fsqrtsx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fsubsx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fabsx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fcmpo_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fcmpu_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fctiwx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fctiwzx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fmrx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fnabsx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fnegx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *frspx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *faddx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fdivx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fmaddx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fmsubx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fmulx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fnmaddx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fnmsubx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *frsqrtex_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fselx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fsqrtx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fsubx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);



extern char *faddsx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fdivsx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fmaddsx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fmsubsx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fmulsx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fnmaddsx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fnmsubsx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fresx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fsqrtsx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fsubsx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fabsx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fcmpo_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fcmpu_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fctiwx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fctiwzx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fmrx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fnabsx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fnegx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *frspx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *faddx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fdivx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fmaddx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fmsubx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fmulx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fnmaddx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fnmsubx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *frsqrtex_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fselx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fsqrtx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *fsubx_disasm(ppc_inst_t inst, addr_t pc, char *buf);



extern void faddsx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fdivsx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fmaddsx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fmsubsx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fmulsx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fnmaddsx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fnmsubsx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fresx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fsqrtsx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fsubsx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fabsx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fcmpo_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fcmpu_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fctiwx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fctiwzx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fmrx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fnabsx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fnegx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void frspx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void faddx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fdivx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fmaddx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fmsubx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fmulx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fnmaddx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fnmsubx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void frsqrtex_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fselx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fsqrtx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void fsubx_translate(ppc_inst_t inst, struct DecodedInstruction *di);







extern void faddsx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fdivsx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fmaddsx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fmsubsx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fmulsx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fnmaddsx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fnmsubsx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fresx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fsqrtsx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fsubsx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fabsx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fcmpo_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fcmpu_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fctiwx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fctiwzx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fmrx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fnabsx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fnegx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void frspx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void faddx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fdivx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fmaddx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fmsubx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fmulx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fnmaddx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fnmsubx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void frsqrtex_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fselx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fsqrtx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void fsubx_xml(ppc_inst_t inst, addr_t pc, char *buf);

#if defined(__cplusplus)
}
#endif

#endif
