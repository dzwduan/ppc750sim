/***************************************************************************
                          systemregisters.h  -  description
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

#ifndef __SYSTEMREGISTERS_H__
#define __SYSTEMREGISTERS_H__

#include <misc.h>
#include <translate.h>

#if defined(__cplusplus)
extern "C" {
#endif
extern void mcrfs_impl(ppc_inst_t inst);
extern void mffsx_impl(ppc_inst_t inst);
extern void mtfsb0x_impl(ppc_inst_t inst);
extern void mtfsb1x_impl(ppc_inst_t inst);
extern void mtfsfix_impl(ppc_inst_t inst);
extern void mtfsfx_impl(ppc_inst_t inst);
extern void mcrxr_impl(ppc_inst_t inst);
extern void mfcr_impl(ppc_inst_t inst);
extern void mfmsr_impl(ppc_inst_t inst);
extern void mfsr_impl(ppc_inst_t inst);
extern void mfsrin_impl(ppc_inst_t inst);
extern void mtmsr_impl(ppc_inst_t inst);
extern void mtsr_impl(ppc_inst_t inst);
extern void mtsrin_impl(ppc_inst_t inst);
extern void mfspr_impl(ppc_inst_t inst);
extern void mftb_impl(ppc_inst_t inst);
extern void mtcrf_impl(ppc_inst_t inst);
extern void mtspr_impl(ppc_inst_t inst);
extern void crand_impl(ppc_inst_t inst);
extern void crandc_impl(ppc_inst_t inst);
extern void creqv_impl(ppc_inst_t inst);
extern void crnand_impl(ppc_inst_t inst);
extern void crnor_impl(ppc_inst_t inst);
extern void cror_impl(ppc_inst_t inst);
extern void crorc_impl(ppc_inst_t inst);
extern void crxor_impl(ppc_inst_t inst);
extern void mcrf_impl(ppc_inst_t inst);
extern void rfi_impl(ppc_inst_t inst);
extern void rfid_impl(ppc_inst_t inst);	
extern void sync_impl(ppc_inst_t inst);
extern void isync_impl(ppc_inst_t inst);



extern char *mcrfs_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mffsx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mtfsb0x_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mtfsb1x_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mtfsfix_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mtfsfx_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mcrxr_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mfcr_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mfmsr_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mfsr_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mfsrin_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mtmsr_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mtsr_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mtsrin_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mfspr_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mftb_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mtcrf_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mtspr_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *crand_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *crandc_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *creqv_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *crnand_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *crnor_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *cror_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *crorc_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *crxor_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mcrf_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *rfi_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *rfid_bin2c(ppc_inst_t inst, addr_t pc, char *buf);	
extern char *sync_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *isync_bin2c(ppc_inst_t inst, addr_t pc, char *buf);



extern char *mcrfs_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mffsx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mtfsb0x_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mtfsb1x_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mtfsfix_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mtfsfx_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mcrxr_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mfcr_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mfmsr_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mfsr_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mfsrin_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mtmsr_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mtsr_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mtsrin_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mfspr_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mftb_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mtcrf_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mtspr_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *crand_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *crandc_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *creqv_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *crnand_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *crnor_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *cror_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *crorc_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *crxor_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *mcrf_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *rfi_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *rfid_disasm(ppc_inst_t inst, addr_t pc, char *buf);	
extern char *sync_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern char *isync_disasm(ppc_inst_t inst, addr_t pc, char *buf);



extern void mcrfs_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mffsx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mtfsb0x_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mtfsb1x_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mtfsfix_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mtfsfx_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mcrxr_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mfcr_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mfmsr_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mfsr_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mfsrin_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mtmsr_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mtsr_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mtsrin_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mfspr_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mftb_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mtcrf_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mtspr_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void crand_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void crandc_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void creqv_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void crnand_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void crnor_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void cror_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void crorc_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void crxor_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void mcrf_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void rfi_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void rfid_translate(ppc_inst_t inst, struct DecodedInstruction *di);	
extern void sync_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void isync_translate(ppc_inst_t inst, struct DecodedInstruction *di);





extern void mcrfs_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mffsx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mtfsb0x_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mtfsb1x_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mtfsfix_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mtfsfx_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mcrxr_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mfcr_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mfmsr_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mfsr_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mfsrin_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mtmsr_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mtsr_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mtsrin_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mfspr_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mftb_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mtcrf_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mtspr_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void crand_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void crandc_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void creqv_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void crnand_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void crnor_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void cror_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void crorc_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void crxor_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void mcrf_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void rfi_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void rfid_xml(ppc_inst_t inst, addr_t pc, char *buf);	
extern void sync_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void isync_xml(ppc_inst_t inst, addr_t pc, char *buf);

#if defined(__cplusplus)
}
#endif


#endif
