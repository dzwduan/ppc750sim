/***************************************************************************
                          loadstore.h  -  description
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

#ifndef __PPC_LOADSTORE_H__
#define __PPC_LOADSTORE_H__

#include <misc.h>
#include <translate.h>

#if defined(__cplusplus)
extern "C" {
#endif
extern void lbz_impl(ppc_inst_t inst);
extern void lbzu_impl(ppc_inst_t inst);
extern void lfd_impl(ppc_inst_t inst);
extern void lfdu_impl(ppc_inst_t inst);
extern void lfs_impl(ppc_inst_t inst);
extern void lfsu_impl(ppc_inst_t inst);
extern void lha_impl(ppc_inst_t inst);
extern void lhau_impl(ppc_inst_t inst);
extern void lhz_impl(ppc_inst_t inst);
extern void lhzu_impl(ppc_inst_t inst);
extern void lmw_impl(ppc_inst_t inst);
extern void lwz_impl(ppc_inst_t inst);
extern void lwzu_impl(ppc_inst_t inst);
extern void stb_impl(ppc_inst_t inst);
extern void stbu_impl(ppc_inst_t inst);
extern void stfd_impl(ppc_inst_t inst);
extern void stfdu_impl(ppc_inst_t inst);
extern void stfs_impl(ppc_inst_t inst);
extern void stfsu_impl(ppc_inst_t inst);
extern void sth_impl(ppc_inst_t inst);
extern void sthu_impl(ppc_inst_t inst);
extern void stmw_impl(ppc_inst_t inst);
extern void stw_impl(ppc_inst_t inst);
extern void stwu_impl(ppc_inst_t inst);
extern void dcba_impl(ppc_inst_t inst);
extern void dcbf_impl(ppc_inst_t inst);
extern void dcbi_impl(ppc_inst_t inst);
extern void dcbst_impl(ppc_inst_t inst);
extern void dcbt_impl(ppc_inst_t inst);
extern void dcbtst_impl(ppc_inst_t inst);
extern void dcbz_impl(ppc_inst_t inst);
extern void eciwx_impl(ppc_inst_t inst);
extern void ecowx_impl(ppc_inst_t inst);
extern void eieio_impl(ppc_inst_t inst);
extern void icbi_impl(ppc_inst_t inst);
extern void lbzux_impl(ppc_inst_t inst);
extern void lbzx_impl(ppc_inst_t inst);
extern void lfdux_impl(ppc_inst_t inst);
extern void lfdx_impl(ppc_inst_t inst);
extern void lfsux_impl(ppc_inst_t inst);
extern void lfsx_impl(ppc_inst_t inst);
extern void lhaux_impl(ppc_inst_t inst);
extern void lhax_impl(ppc_inst_t inst);
extern void lhbrx_impl(ppc_inst_t inst);
extern void lhzux_impl(ppc_inst_t inst);
extern void lhzx_impl(ppc_inst_t inst);
extern void lswi_impl(ppc_inst_t inst);
extern void lswx_impl(ppc_inst_t inst);
extern void lwarx_impl(ppc_inst_t inst);
extern void lwbrx_impl(ppc_inst_t inst);
extern void lwzux_impl(ppc_inst_t inst);
extern void lwzx_impl(ppc_inst_t inst);
extern void stbux_impl(ppc_inst_t inst);
extern void stbx_impl(ppc_inst_t inst);
extern void stfdux_impl(ppc_inst_t inst);
extern void stfdx_impl(ppc_inst_t inst);
extern void stfiwx_impl(ppc_inst_t inst);
extern void stfsux_impl(ppc_inst_t inst);
extern void stfsx_impl(ppc_inst_t inst);
extern void sthbrx_impl(ppc_inst_t inst);
extern void sthux_impl(ppc_inst_t inst);
extern void sthx_impl(ppc_inst_t inst);
extern void stswi_impl(ppc_inst_t inst);
extern void stswx_impl(ppc_inst_t inst);
extern void stwbrx_impl(ppc_inst_t inst);
extern void stwcxd_impl(ppc_inst_t inst);
extern void stwux_impl(ppc_inst_t inst);
extern void stwx_impl(ppc_inst_t inst);
extern void sync_impl(ppc_inst_t inst);
extern void tlbia_impl(ppc_inst_t inst);
extern void tlbie_impl(ppc_inst_t inst);
extern void tlbsync_impl(ppc_inst_t inst);


extern char *lbz_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lbzu_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lfd_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lfdu_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lfs_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lfsu_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lha_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lhau_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lhz_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lhzu_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lmw_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lwz_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lwzu_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stb_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stbu_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stfd_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stfdu_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stfs_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stfsu_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *sth_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *sthu_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stmw_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stw_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stwu_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *dcba_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *dcbf_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *dcbi_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *dcbst_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *dcbt_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *dcbtst_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *dcbz_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *eciwx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *ecowx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *eieio_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *icbi_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lbzux_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lbzx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lfdux_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lfdx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lfsux_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lfsx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lhaux_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lhax_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lhbrx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lhzux_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lhzx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lswi_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lswx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lwarx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lwbrx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lwzux_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lwzx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stbux_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stbx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stfdux_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stfdx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stfiwx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stfsux_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stfsx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *sthbrx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *sthux_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *sthx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stswi_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stswx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stwbrx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stwcxd_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stwux_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stwx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *sync_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *tlbia_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *tlbie_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *tlbsync_bin2c(ppc_inst_t inst, addr_t pc, char *buf);




extern char *lbz_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lbzu_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lfd_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lfdu_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lfs_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lfsu_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lha_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lhau_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lhz_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lhzu_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lmw_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lwz_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lwzu_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stb_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stbu_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stfd_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stfdu_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stfs_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stfsu_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *sth_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *sthu_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stmw_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stw_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stwu_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *dcba_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *dcbf_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *dcbi_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *dcbst_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *dcbt_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *dcbtst_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *dcbz_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *eciwx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *ecowx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *eieio_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *icbi_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lbzux_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lbzx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lfdux_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lfdx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lfsux_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lfsx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lhaux_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lhax_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lhbrx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lhzux_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lhzx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lswi_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lswx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lwarx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lwbrx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lwzux_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *lwzx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stbux_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stbx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stfdux_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stfdx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stfiwx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stfsux_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stfsx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *sthbrx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *sthux_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *sthx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stswi_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stswx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stwbrx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stwcxd_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stwux_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *stwx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *sync_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *tlbia_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *tlbie_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *tlbsync_disasm(ppc_inst_t inst, addr_t pc, char *buf);



extern void lbz_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lbzu_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lfd_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lfdu_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lfs_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lfsu_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lha_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lhau_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lhz_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lhzu_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lmw_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lwz_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lwzu_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stb_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stbu_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stfd_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stfdu_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stfs_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stfsu_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void sth_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void sthu_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stmw_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stw_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stwu_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void dcba_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void dcbf_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void dcbi_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void dcbst_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void dcbt_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void dcbtst_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void dcbz_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void eciwx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void ecowx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void eieio_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void icbi_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lbzux_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lbzx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lfdux_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lfdx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lfsux_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lfsx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lhaux_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lhax_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lhbrx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lhzux_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lhzx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lswi_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lswx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lwarx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lwbrx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lwzux_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void lwzx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stbux_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stbx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stfdux_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stfdx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stfiwx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stfsux_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stfsx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void sthbrx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void sthux_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void sthx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stswi_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stswx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stwbrx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stwcxd_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stwux_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void stwx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void sync_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void tlbia_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void tlbie_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void tlbsync_translate(ppc_inst_t inst, struct DecodedInstruction *di);






extern void lbz_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lbzu_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lfd_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lfdu_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lfs_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lfsu_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lha_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lhau_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lhz_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lhzu_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lmw_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lwz_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lwzu_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stb_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stbu_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stfd_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stfdu_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stfs_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stfsu_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void sth_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void sthu_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stmw_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stw_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stwu_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void dcba_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void dcbf_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void dcbi_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void dcbst_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void dcbt_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void dcbtst_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void dcbz_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void eciwx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void ecowx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void eieio_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void icbi_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lbzux_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lbzx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lfdux_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lfdx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lfsux_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lfsx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lhaux_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lhax_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lhbrx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lhzux_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lhzx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lswi_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lswx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lwarx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lwbrx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lwzux_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void lwzx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stbux_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stbx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stfdux_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stfdx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stfiwx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stfsux_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stfsx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void sthbrx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void sthux_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void sthx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stswi_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stswx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stwbrx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stwcxd_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stwux_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void stwx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void sync_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void tlbia_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void tlbie_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void tlbsync_xml(ppc_inst_t inst, addr_t pc, char *buf);

#if defined(__cplusplus)
}
#endif

#endif
