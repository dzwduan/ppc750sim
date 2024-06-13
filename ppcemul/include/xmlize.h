/***************************************************************************
                          xmlize.h  -  Generate XML from a binary program
                             -------------------
    begin                : Mon Feb 11 2002
    copyright            : (C) 2002 by Gilles Mouchard
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

#ifndef __XMLIZE_H__
#define __XMLIZE_H__

#include <misc.h>

#define XML_DSTGPR 1
#define XML_SRCGPR 2
#define XML_UIMM 3
#define XML_SIMM 4
#define XML_DSTCRF 5
#define XML_SRCCRF 6
#define XML_DSTREG 7
#define XML_SRCREG 8
#define XML_DSTXER 9
#define XML_SRCXER 10
#define XML_SRCFPR 11
#define XML_DSTFPR 12
#define XML_DSTSPR 13
#define XML_SRCSPR 14
#define XML_BO 15
#define XML_BI 16
#define XML_SRCFPSCR 17
#define XML_DSTFPSCR 18

#define XML_XERSO 1
#define XML_XEROV 2
#define XML_XERCA 3

extern void xmlf(char *buffer, UInt32 pc, char *mnemonic, int noperands, ...);

#endif
