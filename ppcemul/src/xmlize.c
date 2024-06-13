/***************************************************************************
                          xmlize.c  -  description
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

#include <xmlize.h>
#include <stdio.h>
#include <stdarg.h>

void xmlf(char *buffer, UInt32 pc, char *mnemonic, int noperands, ...)
{
	int i;
	va_list args;
	
	va_start(args, noperands);

	buffer += sprintf(buffer, "<instruction>\n");
	buffer += sprintf(buffer, "<pc>%u</pc>\n", pc);
	buffer += sprintf(buffer, "<mnemonic>%s</mnemonic>", mnemonic);
	
	buffer += sprintf(buffer, "<operands>\n");
	buffer += sprintf(buffer, "<count>%d</count>\n", noperands);
	
	for(i = 0; i < noperands; i++)
	{
		int type = va_arg(args, int);
		buffer += sprintf(buffer, "<operand>\n");		
		
		switch(type)
		{
			case XML_DSTGPR:
				{
					unsigned regnum = va_arg(args, unsigned int);
					buffer += sprintf(buffer, "<direction>destination</direction>\n");
					buffer += sprintf(buffer, "<type>register</type>\n");
					buffer += sprintf(buffer, "<value>r%u</value>\n", regnum);
				}
				break;
				
			case XML_DSTFPR:
				{
					unsigned regnum = va_arg(args, unsigned int);
					buffer += sprintf(buffer, "<direction>destination</direction>\n");
					buffer += sprintf(buffer, "<type>register</type>\n");
					buffer += sprintf(buffer, "<value>f%u</value>\n", regnum);
				}
				break;
			
			case XML_DSTREG:
				{
					char *regname = va_arg(args, char *);
					buffer += sprintf(buffer, "<direction>destination</direction>\n");
					buffer += sprintf(buffer, "<type>register</type>\n");
					buffer += sprintf(buffer, "<value>%s</value>\n", regname);
				}
				break;
			
			case XML_SRCGPR:
				{
					unsigned int regnum = va_arg(args, unsigned int);					
					buffer += sprintf(buffer, "<direction>source</direction>\n");
					buffer += sprintf(buffer, "<type>register</type>\n");
					buffer += sprintf(buffer, "<value>r%u</value>\n", regnum);
				}
				break;

			case XML_SRCFPR:
				{
					unsigned int regnum = va_arg(args, unsigned int);					
					buffer += sprintf(buffer, "<direction>source</direction>\n");
					buffer += sprintf(buffer, "<type>register</type>\n");
					buffer += sprintf(buffer, "<value>f%u</value>\n", regnum);
				}
				break;
			
			case XML_SRCSPR:
				{
					unsigned int regnum = va_arg(args, unsigned int);					
					buffer += sprintf(buffer, "<direction>source</direction>\n");
					buffer += sprintf(buffer, "<type>register</type>\n");
					
					switch(regnum)
					{
						case 1:
							buffer += sprintf(buffer, "<value>xer</value>\n");
							break;
						
						case 8:
							buffer += sprintf(buffer, "<value>lr</value>\n");
							break;
							
						case 9:
							buffer += sprintf(buffer, "<value>ctr</value>\n");
							break;
							
						default:
							buffer += sprintf(buffer, "<value>spr%u</value>\n", regnum);
					}
				}
				break;
			
			case XML_DSTSPR:
				{
					unsigned int regnum = va_arg(args, unsigned int);					
					buffer += sprintf(buffer, "<direction>destination</direction>\n");
					buffer += sprintf(buffer, "<type>register</type>\n");
					
					switch(regnum)
					{
						case 1:
							buffer += sprintf(buffer, "<value>xer</value>\n");
							break;
						
						case 8:
							buffer += sprintf(buffer, "<value>lr</value>\n");
							break;
							
						case 9:
							buffer += sprintf(buffer, "<value>ctr</value>\n");
							break;
							
						default:
							buffer += sprintf(buffer, "<value>spr%u</value>\n", regnum);
					}
				}
				break;
			
			case XML_SRCREG:
				{
					char *regname = va_arg(args, char *);
					buffer += sprintf(buffer, "<direction>source</direction>\n");
					buffer += sprintf(buffer, "<type>register</type>\n");
					buffer += sprintf(buffer, "<value>%s</value>\n", regname);
				}
				break;
			
			case XML_UIMM:
				{
					UInt32 uimm = va_arg(args, UInt32);
					buffer += sprintf(buffer, "<direction>source</direction>\n");
					buffer += sprintf(buffer, "<type>immediate</type>\n");
					buffer += sprintf(buffer, "<value>%u</value>\n", uimm);
				}
				break;
					
			case XML_SIMM:
				{
					UInt32 simm = va_arg(args, SInt32);
					buffer += sprintf(buffer, "<direction>source</direction>\n");
					buffer += sprintf(buffer, "<type>immediate</type>\n");
					buffer += sprintf(buffer, "<value>%d</value>\n", simm);
				}
				break;
				
			case XML_DSTCRF:
				{
					int fieldnum = va_arg(args, int);
					buffer += sprintf(buffer, "<direction>destination</direction>\n");
					buffer += sprintf(buffer, "<type>register</type>\n");
					if(fieldnum >= 0)
						buffer += sprintf(buffer, "<value>cr%u</value>\n", fieldnum);
					else
						buffer += sprintf(buffer, "<value>cr</value>\n");
				}
				break;
				
			case XML_DSTXER:
				{
					unsigned int flags = va_arg(args, unsigned int);
					if(flags & XML_XERCA)
					{
						buffer += sprintf(buffer, "<direction>destination</direction>\n");
						buffer += sprintf(buffer, "<type>register</type>\n");
						buffer += sprintf(buffer, "<value>xer[ca]</value>\n");
					}
					if(flags & XML_XEROV)
					{
						buffer += sprintf(buffer, "<direction>destination</direction>\n");
						buffer += sprintf(buffer, "<type>register</type>\n");
						buffer += sprintf(buffer, "<value>xer[ov]</value>\n");
					}
					
					if(flags & XML_XERSO)
					{
						buffer += sprintf(buffer, "<direction>destination</direction>\n");
						buffer += sprintf(buffer, "<type>register</type>\n");
						buffer += sprintf(buffer, "<value>xer[so]</value>\n");
					}
				}
				break;
		
			case XML_SRCXER:
				{
					unsigned int flags = va_arg(args, unsigned int);
					if(flags & XML_XERCA)
					{
						buffer += sprintf(buffer, "<direction>source</direction>\n");
						buffer += sprintf(buffer, "<type>register</type>\n");
						buffer += sprintf(buffer, "<value>xer[ca]</value>\n");
					}
					if(flags & XML_XEROV)
					{
						buffer += sprintf(buffer, "<direction>source</direction>\n");
						buffer += sprintf(buffer, "<type>register</type>\n");
						buffer += sprintf(buffer, "<value>xer[ov]</value>\n");
					}
					
					if(flags & XML_XERSO)
					{
						buffer += sprintf(buffer, "<direction>source</direction>\n");
						buffer += sprintf(buffer, "<type>register</type>\n");
						buffer += sprintf(buffer, "<value>xer[so]</value>\n");
					}
				}
				break;
			
			case XML_SRCCRF:
				{
					unsigned int fieldnum = va_arg(args, unsigned int);
					buffer += sprintf(buffer, "<direction>source</direction>\n");
					buffer += sprintf(buffer, "<type>register</type>\n");
					buffer += sprintf(buffer, "<value>cr%u</value>\n", fieldnum);
				}
				break;
				
			case XML_BO:
				{
					unsigned int bo = va_arg(args, unsigned int);
					buffer += sprintf(buffer, "<direction>source</direction>\n");
					buffer += sprintf(buffer, "<type>bo</type>\n");
					buffer += sprintf(buffer, "<value>%u</value>\n", bo);
				}
				break;
		
			case XML_BI:
				{
					unsigned int bi = va_arg(args, unsigned int);
					buffer += sprintf(buffer, "<direction>source</direction>\n");
					buffer += sprintf(buffer, "<type>bi</type>\n");
					buffer += sprintf(buffer, "<value>%u</value>\n", bi);
				}
				break;
				
			case XML_SRCFPSCR:
				{
					int dummy = va_arg(args, int);
					buffer += sprintf(buffer, "<direction>source</direction>\n");
					buffer += sprintf(buffer, "<type>register</type>\n");
					buffer += sprintf(buffer, "<value>fpscr</value>\n");
				}
				break;
		
			case XML_DSTFPSCR:
				{
					int dummy = va_arg(args, int);
					buffer += sprintf(buffer, "<direction>destination</direction>\n");
					buffer += sprintf(buffer, "<type>register</type>\n");
					buffer += sprintf(buffer, "<value>fpscr</value>\n");
				}
				break;		
		}
		buffer += sprintf(buffer, "</operand>\n");
	}						
			
	buffer += sprintf(buffer, "</operands>\n");
	buffer += sprintf(buffer, "</instruction>\n");
	
	va_end(args);
}



