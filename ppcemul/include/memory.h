/***************************************************************************
                          memory.h  -  description
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

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <misc.h>
#include <stdio.h>

#define MEMORY_PAGE_SIZE 4096
#define PRIMARY_MEMORY_HASH_TABLE_SIZE 4096
#define SECONDARY_MEMORY_HASH_TABLE_SIZE 16

typedef struct
{
	/* Memory interface */
	byte_t (*mem_read_byte)(void *, addr_t);
	void (*mem_write_byte)(void *, addr_t, byte_t);
	halfword_t (*mem_read_half_word)(void *, addr_t);
	void (*mem_write_half_word)(void *, addr_t, halfword_t);
	word_t (*mem_read_word)(void *, addr_t);
	void (*mem_write_word)(void *, addr_t, word_t);
	dword_t (*mem_read_dword)(void *, addr_t);
	void (*mem_write_dword)(void *, addr_t, dword_t);
	void (*mem_set)(void *, addr_t, byte_t, int);
	void (*mem_read)(void *, void *, addr_t, int);
	void (*mem_write)(void *, addr_t, void *, int);

	
	void *instance;
} memory_cb_t;

typedef struct memory_page_table_entry_t
{
	addr_t addr;
	struct memory_page_table_entry_t *next;
	byte_t *storage;
} memory_page_table_entry_t;

typedef struct
{
	memory_page_table_entry_t *pte[SECONDARY_MEMORY_HASH_TABLE_SIZE];
} secondary_memory_hash_table_t;

typedef struct
{
	secondary_memory_hash_table_t *primary_hash_table[PRIMARY_MEMORY_HASH_TABLE_SIZE];
} memory_t;

extern memory_t *mem;

#if defined(__cplusplus)
extern "C" {
#endif
extern memory_t *mem_create();
extern byte_t mem_read_byte(memory_t *mem, addr_t addr);
extern void mem_write_byte(memory_t *mem, addr_t addr, byte_t value);
extern halfword_t mem_read_half_word(memory_t *mem, addr_t addr);
extern void mem_write_half_word(memory_t *mem, addr_t addr, halfword_t value);
extern word_t mem_read_word(memory_t *mem, addr_t addr);
extern void mem_write_word(memory_t *mem, addr_t addr, word_t value);
extern dword_t mem_read_dword(memory_t *mem, addr_t addr);
extern void mem_write_dword(memory_t *mem, addr_t addr, dword_t value);
extern halfword_t mem_read_half_word_byte_reverse(memory_t *mem, addr_t addr);
extern void mem_write_half_word_byte_reverse(memory_t *mem, addr_t addr, halfword_t value);
extern word_t mem_read_word_byte_reverse(memory_t *mem, addr_t addr);
extern void mem_write_word_byte_reverse(memory_t *mem, addr_t addr, word_t value);
extern dword_t mem_read_dword_byte_reverse(memory_t *mem, addr_t addr);
extern void mem_write_dword_byte_reverse(memory_t *mem, addr_t addr, dword_t value);
extern void mem_set(memory_t *mem, addr_t addr, byte_t value, int size);
extern void mem_read(memory_t *mem, void *buf, addr_t addr, int size);
extern void mem_write(memory_t *mem, addr_t, void *buf, int size);
extern addr_t mem_align_to_page_boundary(addr_t addr);
extern void mem_delete(memory_t *mem);
extern void mem_save(memory_t *mem, FILE *f);
extern void mem_restore(memory_t *mem, FILE *f);
extern void mem_duplicate(memory_t *mem, memory_cb_t *cb);
#if defined(__cplusplus)
}
#endif

#define MEM_READ_BYTE(addr) mem_read_byte(mem, addr)
#define MEM_WRITE_BYTE(addr, value) mem_write_byte(mem, addr, value)
#define MEM_READ_HALF_WORD(addr) mem_read_half_word(mem, addr)
#define MEM_WRITE_HALF_WORD(addr, value) mem_write_half_word(mem, addr, value)
#define MEM_READ_WORD(addr) mem_read_word(mem, addr)
#define MEM_WRITE_WORD(addr, value) mem_write_word(mem, addr, value)
#define MEM_READ_DWORD(addr) mem_read_dword(mem, addr)
#define MEM_WRITE_DWORD(addr, value) mem_write_dword(mem, addr, value)
#define MEM_READ_HALF_WORD_BYTE_REVERSE(addr) mem_read_half_word_byte_reverse(mem, addr)
#define MEM_WRITE_HALF_WORD_BYTE_REVERSE(addr, value) mem_write_half_word_byte_reverse(mem, addr, value)
#define MEM_READ_WORD_BYTE_REVERSE(addr) mem_read_word_byte_reverse(mem, addr)
#define MEM_WRITE_WORD_BYTE_REVERSE(addr, value) mem_write_word_byte_reverse(mem, addr, value)
#define MEM_READ_DWORD_BYTE_REVERSE(addr) mem_read_dword_byte_reverse(mem, addr)
#define MEM_WRITE_DWORD_BYTE_REVERSE(addr, value) mem_write_dword_byte_reverse(mem, addr, value)
#define MEM_SET(addr, value, size) mem_set(mem, addr, value, size)
#define MEM_READ(buf, addr, size) mem_read(mem, buf, addr, size)
#define MEM_WRITE(addr, buf, size) mem_write(mem, addr, buf, size)


#endif
