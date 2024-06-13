/***************************************************************************
                          misc.h  -  description
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

#ifndef __MISC_H__
#define __MISC_H__

typedef unsigned long long UInt64;
typedef signed long long SInt64;
typedef unsigned int UInt32;
typedef signed int SInt32;
typedef unsigned short UInt16;
typedef signed short SInt16;
typedef unsigned char UInt8;
typedef signed char SInt8;

typedef UInt32 addr_t;
typedef UInt64 dword_t;
typedef SInt64 sdword_t;
typedef UInt32 word_t;
typedef SInt32 sword_t;
typedef UInt16 halfword_t;
typedef SInt16 shalfword_t;
typedef UInt8 byte_t;
typedef SInt8 sbyte_t;
typedef UInt32 ppc_inst_t;
typedef UInt8 BOOL;

#define TRUE 1
#define FALSE 0

#define RA ((inst >> 16) & 0x1f)
#define RB ((inst >> 11) & 0x1f)
#define RC ((inst >> 6) & 0x1f)
#define RD ((inst >> 21) & 0x1f)
#define RS ((inst >> 21) & 0x1f)
#define FA ((inst >> 16) & 0x1f)
#define FB ((inst >> 11) & 0x1f)
#define FC ((inst >> 6) & 0x1f)
#define FD ((inst >> 21) & 0x1f)
#define FS ((inst >> 21) & 0x1f)
#define IMM (inst & 0xffff)
#define UIMM (inst & 0xffff)
#define OFS (inst & 0xffff)
#define OPCD ((inst >> 26) & 0x3f)
#define XO_10 ((inst >> 1) & 0x3ff)
#define XO_9 ((inst >> 1) & 0x1ff)
#define XO_5 ((inst >> 1) & 0x1f)
#define Rc (inst & 1)
#define SH ((inst >> 11) & 0x1f)
#define MB ((inst >> 6) & 0x1f)
#define ME ((inst >> 1) & 0x1f)
#define OE ((inst >> 10) & 1)
#define TO ((inst >> 21) & 0x1f)
#define CRFD ((inst >> 23) & 0x7)
#define CRFS ((inst >> 18) & 0x7)
#define CRBD ((inst >> 21) & 0x1f)
#define CRBA ((inst >> 16) & 0x1f)
#define CRBB ((inst >> 11) & 0x1f)
#define L ((inst >> 21) & 1)
#define NB ((inst >> 11) & 0x1f)
#define AA ((inst >> 1) & 1)
#define LK (inst & 1)
#define LI ((inst >> 2) & 0xffffff)
#define BO ((inst >> 21) & 0x1f)
#define BI ((inst >> 16) & 0x1f)
#define BD ((inst >> 2) & 0x3fff)

#define MTFSFI_IMM ((inst >> 12) & 0xf)
#define FM ((inst >> 17) & 0xff)
#define SR ((inst >> 16) & 0xf)
#define SPR ((inst >> 11) & 0x3ff)
#define TBR ((inst >> 11) & 0x3ff)
#define CRM ((inst >> 12) & 0xff)

#define SET_CR0_LT (my_regs.cr = my_regs.cr | (1 << 31))
#define SET_CR0_GT ( my_regs.cr = my_regs.cr | (1 << 30))
#define SET_CR0_EQ ( my_regs.cr = my_regs.cr | (1 << 29))
#define SET_CR0_SO ( my_regs.cr = my_regs.cr | (1 << 28))
#define RESET_CR0_LT ( my_regs.cr = my_regs.cr & (~(1 << 31)))
#define RESET_CR0_GT ( my_regs.cr = my_regs.cr & (~(1 << 30)))
#define RESET_CR0_EQ ( my_regs.cr = my_regs.cr & (~(1 << 29)))
#define RESET_CR0_SO ( my_regs.cr = my_regs.cr & (~(1 << 28)))

#define SET_CR_LT(n) ( my_regs.cr = my_regs.cr | (1 << (3 + 4 * (7 - n))))
#define SET_CR_GT(n) ( my_regs.cr = my_regs.cr | (1 << (2 + 4 * (7 - n))))
#define SET_CR_EQ(n) ( my_regs.cr = my_regs.cr | (1 << (1 + 4 * (7 - n))))
#define SET_CR_SO(n) ( my_regs.cr = my_regs.cr | (1 << (4 * (7 - n))))
#define RESET_CR_LT(n) ( my_regs.cr = my_regs.cr & (~(1 << (3 + 4 * (7 - n)))))
#define RESET_CR_GT(n) ( my_regs.cr = my_regs.cr & (~(1 << (2 + 4 * (7 - n)))))
#define RESET_CR_EQ(n) ( my_regs.cr = my_regs.cr & (~(1 << (1 + 4 * (7 - n)))))
#define RESET_CR_SO(n) ( my_regs.cr = my_regs.cr & (~(1 << (4 * (7 - n)))))

#define SET_XER_CA ( my_regs.xer = my_regs.xer | (1 << 29))
#define SET_XER_OV ( my_regs.xer = my_regs.xer | (1 << 30))
#define SET_XER_SO ( my_regs.xer = my_regs.xer | (1 << 31))
#define RESET_XER_CA ( my_regs.xer = my_regs.xer & (~(1 << 29)))
#define RESET_XER_OV ( my_regs.xer = my_regs.xer & (~(1 << 30)))
#define RESET_XER_SO ( my_regs.xer = my_regs.xer & (~(1 << 31)))

#define XER_SO ((my_regs.xer >> 31) & 1)
#define XER_OV ((my_regs.xer >> 30) & 1)
#define XER_CA ((my_regs.xer >> 29) & 1)


#define FPSCR_RN (my_regs.fpscr & 3)
#define GPR(n) my_regs.gpr[n]
#define FPR_DW(n) my_regs.fpr[n]
#define FPR_DBL(n) (*(double *)(my_regs.fpr + (n)))
#define DBL2DW(a) (*(dword_t *)(&a))
#define W2FLT(a) (*(float *)(&a))

#define LR my_regs.lr
#define CTR my_regs.ctr
#define PC my_regs.pc
#define NPC my_regs.npc
#define SP my_regs.gpr[1]
#define CR my_regs.cr
#define XER my_regs.xer
#define FPSCR my_regs.fpscr
#define TBL my_regs.tbl
#define TBU my_regs.tbu

#define SEXT8(x) ((SInt32)(SInt8) (x))
#define SEXT16(x) ((SInt32)(SInt16) (x))
#define SEXT26(x) (((x) & 0x2000000) ? ((SInt32)(x) | 0xfc000000) : (SInt32)(x))

#define DECREMENTS_CTR ((BO & 4) == 0)
#define BR_IF_CTR_ZERO (BO & 2)
#define CONDITIONAL_BR ((BO & 16) == 0)
#define BR_IF_TRUE (BO & 8)


#define STACK_BASE 0x80000000
#define MAX_ENVIRON (128 * 1024)
#define STACK_SIZE (1024 * 1024)

#endif
