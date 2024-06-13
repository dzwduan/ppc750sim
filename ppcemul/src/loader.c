/***************************************************************************
                          loader.c  -  description
                             -------------------
    begin                : Wed Sep 26 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    email                : gilles.mouchard@lri.fr, gilles.mouchard@cea.fr
    author               : Gilles Mouchard
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <ppcemul.h>
#include <loader.h>
#include <read_elf.h>
#include <stdlib.h>
#include <string.h>
#include <xmlize.h>

UInt32 text_addr = 0;
UInt32 text_size = 0;

void xmlize_program(char *srcFilename, char *dstFilename)
{
	FILE *fxml;
	Elf32_Ehdr *hdr;
	Elf32_Phdr *phdr;
	Elf32_Shdr *shdr;
	char *string_table;
	Elf32_Shdr *shdr_new_section;
	Elf32_Word new_section_size, new_section_type;
	void *new_section;
	Elf32_Addr new_section_addr;
	int i;	
	FILE *fobj;
	UInt32 data_base = 0;
	UInt32 data_size = 0;
	
	fobj = fopen(srcFilename, "rb");
	if(fobj == NULL)
	{
		fprintf(stderr, "Can't open executable\n");
		abort();
	}
	
	fxml = fopen(dstFilename, "wt");
	if(fxml == NULL)
	{
		fprintf(stderr, "Can't create XML file\n");
		abort();
	}
	
	fprintf(fxml, "<asm>\n");

	hdr = ReadElfHeader(fobj);
	
	if(hdr == NULL)
	{
		fprintf(stderr, "Could not read ELF32 header or this is not an ELF32 file.\n");
		abort();
	}
		
	phdr = ReadProgramHeaders(hdr, fobj);

	for(i=0; i<hdr->e_phnum; i++)
	{
	    if ( (phdr[i].p_type == PT_LOAD) /* Loadable Program Segment */
		 &&
		 ((phdr->p_flags & PF_X) != 0 /* not text segment => data segment */))
	    {
		data_base = phdr[i].p_vaddr;
		data_size = phdr[i].p_memsz;
	    }
	}

	shdr = ReadSectionHeaders(hdr, fobj);
	
	if(shdr == NULL)
	{
		fprintf(stderr, "Can't read section headers from executable\n");
		exit(1);
	}
	string_table = LoadStringTable(hdr, shdr, fobj);

	for(i = 0; i < hdr->e_shnum; i++)
	{
		shdr_new_section = &shdr[i];

		new_section_type = GetSectionType(shdr_new_section);
		if ( (new_section_type == SHT_PROGBITS)
			||
			(new_section_type == SHT_NOBITS) )
		{
			new_section_size = shdr_new_section ? GetSectionSize(shdr_new_section) : 0;
			new_section_addr = GetSectionAddr(shdr_new_section);
			
/*			fprintf(stderr, "Loading %s (%u bytes) at address 0x%08x\n", GetSymbolName(shdr[i].sh_name, string_table), new_section_size, new_section_addr);*/
		
			if (new_section_size)
			{
				new_section = malloc(new_section_size);
				LoadSection(shdr_new_section, new_section, fobj);
				if ( (strcmp(GetSymbolName(shdr[i].sh_name, string_table), ".bss") == 0)
						||
					(strcmp(GetSymbolName(shdr[i].sh_name, string_table), ".sbss") == 0) )
				{
					/* unitialized section => write 0s */
				}
				else
				{
					/* initialized section => copy from objfile */
					UInt32 offset;
					char *buffer = new_section;
					UInt32 pc = new_section_addr;
					
					for(offset = 0; offset < new_section_size; offset += 4, pc += 4)
					{
						UInt32 inst = (buffer[offset] << 24)
						            | (buffer[offset + 1] << 16)
						            | (buffer[offset + 2] << 8)
						            | (buffer[offset + 3]);
						
						ppc_xmlize_to_stream(inst, pc, fxml);
					}
				}
		
				free(new_section);
			}
		}
	}
	fprintf(fxml, "</asm>\n");
	
	fclose(fobj);
	fclose(fxml);
}

void load_program(char *filename, int argc, char *argv[], char *envp[])
{
	Elf32_Ehdr *hdr;
	Elf32_Phdr *phdr;
	Elf32_Shdr *shdr;
	char *string_table;
	Elf32_Shdr *shdr_new_section;
	Elf32_Word new_section_size, new_section_type;
	void *new_section;
	Elf32_Addr new_section_addr;
	int i;	
	FILE *fobj;
	UInt32 data_base = 0;
	UInt32 data_size = 0;
	UInt32 envAddr, argAddr;
	int nEnvStrings;
	UInt32 strSize;
	
	fobj = fopen(filename, "rb");
	if(fobj == NULL)
	{
		fprintf(stderr, "Can't open executable %s\n", filename);
		abort();
	}

	hdr = ReadElfHeader(fobj);
	
	if(hdr == NULL)
	{
		fprintf(stderr, "Could not read ELF32 header or this is not an ELF32 file.\n");
		abort();
	}
		
	phdr = ReadProgramHeaders(hdr, fobj);

	for(i=0; i<hdr->e_phnum; i++)
	{
	    if ( (phdr[i].p_type == PT_LOAD) /* Loadable Program Segment */
		 &&
		 ((phdr->p_flags & PF_X) != 0 /* not text segment => data segment */))
	    {
		data_base = phdr[i].p_vaddr;
		data_size = phdr[i].p_memsz;
	    }
	}

	shdr = ReadSectionHeaders(hdr, fobj);
	
	if(shdr == NULL)
	{
		fprintf(stderr, "Can't read section headers from executable\n");
		exit(1);
	}
	string_table = LoadStringTable(hdr, shdr, fobj);

	for(i = 0; i < hdr->e_shnum; i++)
	{
		shdr_new_section = &shdr[i];

		new_section_type = GetSectionType(shdr_new_section);
		if ( (new_section_type == SHT_PROGBITS)
			||
			(new_section_type == SHT_NOBITS) )
		{
			new_section_size = shdr_new_section ? GetSectionSize(shdr_new_section) : 0;
			new_section_addr = GetSectionAddr(shdr_new_section);
			
/*			fprintf(stderr, "Loading %s (%u bytes) at address 0x%08x\n", GetSymbolName(shdr[i].sh_name, string_table), new_section_size, new_section_addr);*/
		
			if (new_section_size)
			{
				new_section = malloc(new_section_size);
				LoadSection(shdr_new_section, new_section, fobj);
				if ( (strcmp(GetSymbolName(shdr[i].sh_name, string_table), ".bss") == 0)
						||
					(strcmp(GetSymbolName(shdr[i].sh_name, string_table), ".sbss") == 0) )
				{
					/* unitialized section => write 0s */
					MEM_SET(new_section_addr, 0, new_section_size);
				}
				else
				{
					/* initialized section => copy from objfile */
					if (strcmp(GetSymbolName(shdr[i].sh_name, string_table), ".text") == 0)
					{
						MEM_WRITE(new_section_addr, new_section, new_section_size);
						MEM_SET(new_section_addr + new_section_size, 0, 128);
						PC = new_section_addr;
					}
					else
					{
						MEM_WRITE(new_section_addr, new_section, new_section_size);
					}
				}
		
				free(new_section);
			}
		}
	}
	
	fclose(fobj);
	
	
	MEM_SET(STACK_BASE - STACK_SIZE, 0, STACK_SIZE);
	
	SP = STACK_BASE - MAX_ENVIRON;
	
	envAddr = SP;
	argAddr = SP;
	
	for(nEnvStrings = 0; envp[nEnvStrings]; nEnvStrings++)
	{
		strSize = strlen(envp[nEnvStrings]) + 1;
		MEM_WRITE(envAddr, envp[nEnvStrings], strSize);
		envAddr += strSize;
	}
	
	for(i = argc - 1; i >= 0; i--)
	{
		strSize = strlen(argv[i]) + 1;
		SP -= strSize;
		MEM_WRITE(SP, (UInt8 *) argv[i], strSize);
	}
	
	SP -= SP % 4;
	SP -= 4;
	MEM_WRITE_WORD(SP, 0);
	
	for(i = nEnvStrings - 1; i >= 0; i--)
	{
		strSize = strlen(envp[i]) + 1;
		envAddr -= strSize;
		SP -= 4;
		MEM_WRITE_WORD(SP, envAddr);
	}
	
	SP -= 4;
	MEM_WRITE_WORD(SP, 0);
	
	for(i = argc - 1; i >= 0; i--)
	{
		strSize = strlen(argv[i]) + 1;
		argAddr -= strSize;
		SP -= 4;
		MEM_WRITE_WORD(SP, argAddr);
	}
	
	SP -= 4;
	MEM_WRITE_WORD(SP, argc);
	
	syscall_setbrk(mem_align_to_page_boundary(data_base + data_size + MEMORY_PAGE_SIZE));
}

void bin2c_program(char *srcFilename, char *dstFilename)
{
	FILE *fc;
	Elf32_Ehdr *hdr;
	Elf32_Phdr *phdr;
	Elf32_Shdr *shdr;
	char *string_table;
	Elf32_Shdr *shdr_new_section;
	Elf32_Word new_section_size, new_section_type;
	void *new_section;
	Elf32_Addr new_section_addr;
	int i;	
	FILE *fobj;
	UInt32 data_base = 0;
	UInt32 data_size = 0;
	
	fobj = fopen(srcFilename, "rb");
	if(fobj == NULL)
	{
		fprintf(stderr, "Can't open executable\n");
		abort();
	}
	
	fc = fopen(dstFilename, "wt");
	if(fc == NULL)
	{
		fprintf(stderr, "Can't create C file\n");
		abort();
	}
	
	fprintf(fc, "#include <ppcemul.h>\n");
	fprintf(fc, "extern void *pc_table[];\n");
	
	hdr = ReadElfHeader(fobj);
	
	if(hdr == NULL)
	{
		fprintf(stderr, "Could not read ELF32 header or this is not an ELF32 file.\n");
		abort();
	}
		
	phdr = ReadProgramHeaders(hdr, fobj);

	for(i=0; i<hdr->e_phnum; i++)
	{
	    if ( (phdr[i].p_type == PT_LOAD) /* Loadable Program Segment */
		 &&
		 ((phdr->p_flags & PF_X) != 0 /* not text segment => data segment */))
	    {
		data_base = phdr[i].p_vaddr;
		data_size = phdr[i].p_memsz;
	    }
	}

	shdr = ReadSectionHeaders(hdr, fobj);
	
	if(shdr == NULL)
	{
		fprintf(stderr, "Can't read section headers from executable\n");
		exit(1);
	}
	string_table = LoadStringTable(hdr, shdr, fobj);

	
	
	for(i = 0; i < hdr->e_shnum; i++)
	{
		shdr_new_section = &shdr[i];

		new_section_type = GetSectionType(shdr_new_section);
		if ( (new_section_type == SHT_PROGBITS)
			||
			(new_section_type == SHT_NOBITS) )
		{
			new_section_size = shdr_new_section ? GetSectionSize(shdr_new_section) : 0;
			new_section_addr = GetSectionAddr(shdr_new_section);
			
			if (new_section_size)
			{
				new_section = malloc(new_section_size);
				LoadSection(shdr_new_section, new_section, fobj);
				
				if(strcmp(GetSymbolName(shdr[i].sh_name, string_table), ".text") == 0)
				{
					UInt32 offset;
					unsigned char *buffer = new_section;
					UInt32 pc;
					
					text_addr = new_section_addr;
					text_size = new_section_size;
						
					fprintf(fc, "void run(addr_t pc)\n");
					fprintf(fc, "{\n");
					fprintf(fc, "void *pc_table[] = {\n");
					for(offset = 0, pc = new_section_addr; offset < new_section_size; offset += 4, pc += 4)
					{
						fprintf(fc, "&&pc_%08x", pc);
						if(offset < new_section_size - 4) fprintf(fc, ",");
						fprintf(fc, "\n");							
					}
					fprintf(fc, "};");
					/*
					for(offset = 0, pc = new_section_addr; offset < new_section_size; offset += 4, pc += 4)
					{
						fprintf(fc, "__label__ pc_%08x;\n", pc);
					}
					*/
						
					for(offset = 0, pc = new_section_addr; offset < new_section_size; offset += 4, pc += 4)
					{
						UInt32 inst = (buffer[offset] << 24)
						            | (buffer[offset + 1] << 16)
						            | (buffer[offset + 2] << 8)
						            | (buffer[offset + 3]);
						
						ppc_bin2c_to_stream(inst, pc, fc);
					}
					fprintf(fc, "}\n");					
						
				}
		
				free(new_section);
			}
		}
	}
	
	fclose(fobj);
	fclose(fc);
}
