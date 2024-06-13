/***************************************************************************
                          integer.h  -  description
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

#ifndef __PPC_INTEGER_H__
#define __PPC_INTEGER_H__

#include <misc.h>
#include <translate.h>

#if defined(__cplusplus)
extern "C" {
#endif
extern void addi_impl(ppc_inst_t inst);
extern void addic_impl(ppc_inst_t inst);
extern void addicd_impl(ppc_inst_t inst);
extern void addis_impl(ppc_inst_t inst);
extern void andid_impl(ppc_inst_t inst);
extern void andisd_impl(ppc_inst_t inst);
extern void cmpi_impl(ppc_inst_t inst);
extern void cmpli_impl(ppc_inst_t inst);
extern void mulli_impl(ppc_inst_t inst);
extern void ori_impl(ppc_inst_t inst);
extern void oris_impl(ppc_inst_t inst);
extern void subfic_impl(ppc_inst_t inst);
extern void twi_impl(ppc_inst_t inst);
extern void xori_impl(ppc_inst_t inst);
extern void xoris_impl(ppc_inst_t inst);
extern void rlwimix_impl(ppc_inst_t inst);
extern void rlwinmx_impl(ppc_inst_t inst);
extern void rlwnmx_impl(ppc_inst_t inst);
extern void andx_impl(ppc_inst_t inst);
extern void andcx_impl(ppc_inst_t inst);
extern void cmp_impl(ppc_inst_t inst);
extern void cmpl_impl(ppc_inst_t inst);
extern void cntlzwx_impl(ppc_inst_t inst);
extern void eqvx_impl(ppc_inst_t inst);
extern void extsbx_impl(ppc_inst_t inst);
extern void extshx_impl(ppc_inst_t inst);
extern void nandx_impl(ppc_inst_t inst);
extern void norx_impl(ppc_inst_t inst);
extern void orx_impl(ppc_inst_t inst);
extern void orcx_impl(ppc_inst_t inst);
extern void slwx_impl(ppc_inst_t inst);
extern void srawx_impl(ppc_inst_t inst);
extern void srawix_impl(ppc_inst_t inst);
extern void srwx_impl(ppc_inst_t inst);
extern void tw_impl(ppc_inst_t inst);
extern void xorx_impl(ppc_inst_t inst);
extern void addx_impl(ppc_inst_t inst);
extern void addcx_impl(ppc_inst_t inst);
extern void addex_impl(ppc_inst_t inst);
extern void addmex_impl(ppc_inst_t inst);
extern void addzex_impl(ppc_inst_t inst);
extern void divwx_impl(ppc_inst_t inst);
extern void divwux_impl(ppc_inst_t inst);
extern void mulhwx_impl(ppc_inst_t inst);
extern void mulhwux_impl(ppc_inst_t inst);
extern void mullwx_impl(ppc_inst_t inst);
extern void negx_impl(ppc_inst_t inst);
extern void subfx_impl(ppc_inst_t inst);
extern void subfcx_impl(ppc_inst_t inst);
extern void subfex_impl(ppc_inst_t inst);
extern void subfmex_impl(ppc_inst_t inst);
extern void subfzex_impl(ppc_inst_t inst);

extern char *addi_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *addic_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *addicd_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *addis_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *andid_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *andisd_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *cmpi_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *cmpli_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mulli_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *ori_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *oris_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *subfic_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *twi_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *xori_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *xoris_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *rlwimix_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *rlwinmx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *rlwnmx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *andx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *andcx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *cmp_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *cmpl_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *cntlzwx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *eqvx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *extsbx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *extshx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *nandx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *norx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *orx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *orcx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *slwx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *srawx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *srawix_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *srwx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *tw_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *xorx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *addx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *addcx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *addex_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *addmex_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *addzex_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *divwx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *divwux_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mulhwx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mulhwux_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mullwx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *negx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *subfx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *subfcx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *subfex_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *subfmex_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *subfzex_bin2c(ppc_inst_t inst, addr_t pc, char *buf);


extern char *addi_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *addic_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *addicd_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *addis_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *andid_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *andisd_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *cmpi_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *cmpli_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mulli_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *ori_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *oris_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *subfic_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *twi_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *xori_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *xoris_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *rlwimix_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *rlwinmx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *rlwnmx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *andx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *andcx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *cmp_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *cmpl_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *cntlzwx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *eqvx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *extsbx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *extshx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *nandx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *norx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *orx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *orcx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *slwx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *srawx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *srawix_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *srwx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *tw_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *xorx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *addx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *addcx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *addex_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *addmex_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *addzex_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *divwx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *divwux_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mulhwx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mulhwux_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mullwx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *negx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *subfx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *subfcx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *subfex_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *subfmex_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *subfzex_disasm(ppc_inst_t inst, addr_t pc, char *buf);



extern void addi_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void addic_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void addicd_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void addis_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void andid_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void andisd_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void cmpi_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void cmpli_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mulli_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void ori_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void oris_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void subfic_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void twi_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void xori_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void xoris_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void rlwimix_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void rlwinmx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void rlwnmx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void andx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void andcx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void cmp_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void cmpl_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void cntlzwx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void eqvx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void extsbx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void extshx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void nandx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void norx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void orx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void orcx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void slwx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void srawx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void srawix_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void srwx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void tw_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void xorx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void addx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void addcx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void addex_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void addmex_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void addzex_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void divwx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void divwux_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mulhwx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mulhwux_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mullwx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void negx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void subfx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void subfcx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void subfex_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void subfmex_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void subfzex_translate(ppc_inst_t inst, struct DecodedInstruction *di);





extern void addi_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void addic_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void addicd_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void addis_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void andid_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void andisd_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void cmpi_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void cmpli_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mulli_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void ori_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void oris_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void subfic_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void twi_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void xori_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void xoris_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void rlwimix_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void rlwinmx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void rlwnmx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void andx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void andcx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void cmp_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void cmpl_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void cntlzwx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void eqvx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void extsbx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void extshx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void nandx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void norx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void orx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void orcx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void slwx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void srawx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void srawix_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void srwx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void tw_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void xorx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void addx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void addcx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void addex_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void addmex_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void addzex_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void divwx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void divwux_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mulhwx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mulhwux_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mullwx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void negx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void subfx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void subfcx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void subfex_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void subfmex_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void subfzex_xml(ppc_inst_t inst, addr_t pc, char *buf);


#if defined(__cplusplus)
}
#endif

#endif
