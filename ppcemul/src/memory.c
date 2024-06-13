/***************************************************************************
                          memory.c  -  description
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

#include <memory.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

memory_t *mem = NULL;

memory_t *mem_create()
{
	memory_t *mem = (memory_t *) malloc(sizeof(memory_t));
	memset(mem->primary_hash_table, 0, sizeof(mem->primary_hash_table));
	return mem;
}

void mem_delete(memory_t *mem)
{
	int i, j;
	
	for(i = 0; i < PRIMARY_MEMORY_HASH_TABLE_SIZE; i++)
	{
		secondary_memory_hash_table_t *secondary_hash_table = mem->primary_hash_table[i];
		
		if(secondary_hash_table)
		{
			for(j = 0; j < SECONDARY_MEMORY_HASH_TABLE_SIZE; j++)
			{
				memory_page_table_entry_t *pte = secondary_hash_table->pte[j];
				memory_page_table_entry_t *nextpte;

				if(pte)
				{
					do
					{
						nextpte = pte->next;
						free(pte->storage);
						free(pte);
					} while((pte = nextpte) != 0);
				}				
			}
			free(secondary_hash_table);
		}
	}
	free(mem);
}

UInt32 mem_hash1(addr_t addr)
{
	return (addr / MEMORY_PAGE_SIZE) % PRIMARY_MEMORY_HASH_TABLE_SIZE;
}

UInt32 mem_hash2(addr_t addr)
{
	return (addr / MEMORY_PAGE_SIZE / PRIMARY_MEMORY_HASH_TABLE_SIZE) % SECONDARY_MEMORY_HASH_TABLE_SIZE;
}

memory_page_table_entry_t *mem_search_page(memory_t *mem, addr_t addr)
{
	UInt32 h1;
	UInt32 h2;
	secondary_memory_hash_table_t *secondary_hash_table;
	
	addr = addr - (addr % MEMORY_PAGE_SIZE);
	
	h1 = mem_hash1(addr);
	secondary_hash_table = mem->primary_hash_table[h1];
	
	if(secondary_hash_table)
	{
		memory_page_table_entry_t *pte;
		h2 = mem_hash2(addr);
		pte = secondary_hash_table->pte[h2];
		
		if(pte)
		{
			do
			{
				if(pte->addr == addr) return pte;
			} while((pte = pte->next) != 0);
		}
	}
	return 0;
}

memory_page_table_entry_t *mem_get_page(memory_t *mem, addr_t addr)
{
	memory_page_table_entry_t *pte;
	
	addr = addr - (addr % MEMORY_PAGE_SIZE);
	pte = mem_search_page(mem, addr);
	if(!pte)
	{
		UInt32 h1, h2;
		secondary_memory_hash_table_t *secondary_hash_table;
		
		h1 = mem_hash1(addr);
		secondary_hash_table = mem->primary_hash_table[h1];
		
		if(!secondary_hash_table)
		{
			secondary_hash_table = (secondary_memory_hash_table_t *) calloc(sizeof(secondary_memory_hash_table_t), 1);
			mem->primary_hash_table[h1] = secondary_hash_table;
		}
		
		h2 = mem_hash2(addr);
		pte = (memory_page_table_entry_t *) malloc(sizeof(memory_page_table_entry_t));
		pte->addr = addr;
		pte->storage = (byte_t *) calloc(MEMORY_PAGE_SIZE, 1);
		pte->next = secondary_hash_table->pte[h2];
		secondary_hash_table->pte[h2] = pte;
	}
	return pte;	
}

byte_t mem_read_byte(memory_t *mem, addr_t addr)
{
	addr_t offset = addr % MEMORY_PAGE_SIZE;
	memory_page_table_entry_t *pte = mem_get_page(mem, addr);
	
	return pte->storage[offset];
}

void mem_write_byte(memory_t *mem, addr_t addr, byte_t value)
{
	addr_t offset = addr % MEMORY_PAGE_SIZE;
	memory_page_table_entry_t *pte = mem_get_page(mem, addr);
	pte->storage[offset] = value;
}

halfword_t mem_read_half_word(memory_t *mem, addr_t addr)
{
	addr_t offset = addr % MEMORY_PAGE_SIZE;
	memory_page_table_entry_t *pte = mem_get_page(mem, addr);
	
	return (pte->storage[offset] << 8) | pte->storage[offset + 1];
}

void mem_write_half_word(memory_t *mem, addr_t addr, halfword_t value)
{
	addr_t offset = addr % MEMORY_PAGE_SIZE;
	memory_page_table_entry_t *pte = mem_get_page(mem, addr);
	pte->storage[offset] = value >> 8;
	pte->storage[offset + 1] = value;
}

word_t mem_read_word(memory_t *mem, addr_t addr)
{
	addr_t offset = addr % MEMORY_PAGE_SIZE;
	memory_page_table_entry_t *pte = mem_get_page(mem, addr);
	
	return (pte->storage[offset] << 24) | (pte->storage[offset + 1] << 16) | (pte->storage[offset + 2] << 8) | pte->storage[offset + 3];
}

void mem_write_word(memory_t *mem, addr_t addr, word_t value)
{
	addr_t offset = addr % MEMORY_PAGE_SIZE;
	memory_page_table_entry_t *pte = mem_get_page(mem, addr);
	pte->storage[offset] = value >> 24;
	pte->storage[offset + 1] = value >> 16;
	pte->storage[offset + 2] = value >> 8;
	pte->storage[offset + 3] = value;
}

dword_t mem_read_dword(memory_t *mem, addr_t addr)
{
	addr_t offset = addr % MEMORY_PAGE_SIZE;
	memory_page_table_entry_t *pte = mem_get_page(mem, addr);
	
	return ((dword_t) pte->storage[offset] << 56) |
	       ((dword_t) pte->storage[offset + 1] << 48) |
	       ((dword_t) pte->storage[offset + 2] << 40) |
	       ((dword_t) pte->storage[offset + 3] << 32) |
	       ((dword_t) pte->storage[offset + 4] << 24) |
	       ((dword_t) pte->storage[offset + 5] << 16) |
	       ((dword_t) pte->storage[offset + 6] << 8) |
	       ((dword_t) pte->storage[offset + 7]);
}

void mem_write_dword(memory_t *mem, addr_t addr, dword_t value)
{
	addr_t offset = addr % MEMORY_PAGE_SIZE;
	memory_page_table_entry_t *pte = mem_get_page(mem, addr);
	pte->storage[offset] = value >> 56;
	pte->storage[offset + 1] = value >> 48;
	pte->storage[offset + 2] = value >> 40;
	pte->storage[offset + 3] = value >> 32;
	pte->storage[offset + 4] = value >> 24;
	pte->storage[offset + 5] = value >> 16;
	pte->storage[offset + 6] = value >> 8;
	pte->storage[offset + 7] = value;
}

halfword_t mem_read_half_word_byte_reverse(memory_t *mem, addr_t addr)
{
	addr_t offset = addr % MEMORY_PAGE_SIZE;
	memory_page_table_entry_t *pte = mem_get_page(mem, addr);
	
	return pte->storage[offset] | (pte->storage[offset + 1] << 8);
}
	
void mem_write_half_word_byte_reverse(memory_t *mem, addr_t addr, halfword_t value)
{
	addr_t offset = addr % MEMORY_PAGE_SIZE;
	memory_page_table_entry_t *pte = mem_get_page(mem, addr);
	pte->storage[offset] = value;
	pte->storage[offset + 1] = value >> 8;
}

word_t mem_read_word_byte_reverse(memory_t *mem, addr_t addr)
{
	addr_t offset = addr % MEMORY_PAGE_SIZE;
	memory_page_table_entry_t *pte = mem_get_page(mem, addr);
	
	return pte->storage[offset] | (pte->storage[offset + 1] << 8) | (pte->storage[offset + 2] << 16) | (pte->storage[offset + 3] << 24);
}

void mem_write_word_byte_reverse(memory_t *mem, addr_t addr, word_t value)
{
	addr_t offset = addr % MEMORY_PAGE_SIZE;
	memory_page_table_entry_t *pte = mem_get_page(mem, addr);
	pte->storage[offset] = value;
	pte->storage[offset + 1] = value >> 8;
	pte->storage[offset + 2] = value >> 16;
	pte->storage[offset + 3] = value >> 24;
}

dword_t mem_read_dword_byte_reverse(memory_t *mem, addr_t addr)
{
	addr_t offset = addr % MEMORY_PAGE_SIZE;
	memory_page_table_entry_t *pte = mem_get_page(mem, addr);
	
	return ((dword_t) pte->storage[offset]) |
	       ((dword_t) pte->storage[offset + 1] << 8) |
	       ((dword_t) pte->storage[offset + 2] << 16) |
	       ((dword_t) pte->storage[offset + 3] << 24) |
	       ((dword_t) pte->storage[offset + 4] << 32) |
	       ((dword_t) pte->storage[offset + 5] << 40) |
	       ((dword_t) pte->storage[offset + 6] << 48) |
	       ((dword_t) pte->storage[offset + 7] << 56);
}

void mem_write_dword_byte_reverse(memory_t *mem, addr_t addr, dword_t value)
{
	addr_t offset = addr % MEMORY_PAGE_SIZE;
	memory_page_table_entry_t *pte = mem_get_page(mem, addr);
	pte->storage[offset] = value;
	pte->storage[offset + 1] = value >> 8;
	pte->storage[offset + 2] = value >> 16;
	pte->storage[offset + 3] = value >> 24;
	pte->storage[offset + 4] = value >> 32;
	pte->storage[offset + 5] = value >> 40;
	pte->storage[offset + 6] = value >> 48;
	pte->storage[offset + 7] = value >> 56;
}

void mem_read(memory_t *mem, void *buf, addr_t addr, int size)
{
	if(size > 0)
	{
		UInt32 offset = addr % MEMORY_PAGE_SIZE;
		memory_page_table_entry_t *pte = mem_get_page(mem, addr);
		UInt32 sz = MEMORY_PAGE_SIZE - offset;	
		if(size > sz)
		{
			memcpy(buf, pte->storage + offset, sz);
			size -= sz;
			addr += sz;
			buf = (byte_t *) buf + sz;
			
			if(size >= MEMORY_PAGE_SIZE)
			{
				do
				{
					pte = mem_get_page(mem, addr);
					memcpy(buf, pte->storage, MEMORY_PAGE_SIZE);
					size -= MEMORY_PAGE_SIZE;
					addr += MEMORY_PAGE_SIZE;
					buf = (byte_t *) buf + MEMORY_PAGE_SIZE;
				} while(size >= MEMORY_PAGE_SIZE);
			}
		
			if(size > 0)
			{
				pte = mem_get_page(mem, addr);
				memcpy(buf, pte->storage, size);
			}
		}
		else
		{
			memcpy(buf, pte->storage + offset, size);
		}
	}
}

void mem_write(memory_t *mem, addr_t addr, void *buf, int size)
{
	if(size > 0)
	{
		UInt32 offset = addr % MEMORY_PAGE_SIZE;
		memory_page_table_entry_t *pte = mem_get_page(mem, addr);
		UInt32 sz = MEMORY_PAGE_SIZE - offset;	
		if(size > sz)
		{
			memcpy(pte->storage + offset, buf, sz);
			size -= sz;
			addr += sz;
			buf = (byte_t *) buf + sz;
			
			if(size >= MEMORY_PAGE_SIZE)
			{
				do
				{
					pte = mem_get_page(mem, addr);
					memcpy(pte->storage, buf, MEMORY_PAGE_SIZE);
					size -= MEMORY_PAGE_SIZE;
					addr += MEMORY_PAGE_SIZE;
					buf = (byte_t *) buf + MEMORY_PAGE_SIZE;
				} while(size >= MEMORY_PAGE_SIZE);
			}
		
			if(size > 0)
			{
				pte = mem_get_page(mem, addr);
				memcpy(pte->storage, buf, size);
			}
		}
		else
		{
			memcpy(pte->storage + offset, buf, size);
		}
	}
}

void mem_set(memory_t *mem, addr_t addr, byte_t value, int size)
{
	if(size > 0)
	{
		UInt32 offset = addr % MEMORY_PAGE_SIZE;
		memory_page_table_entry_t *pte = mem_get_page(mem, addr);
		UInt32 sz = MEMORY_PAGE_SIZE - offset;	
		if(size > sz)
		{
			memset(pte->storage + offset, value, sz);
			size -= sz;
			addr += sz;
			
			if(size >= MEMORY_PAGE_SIZE)
			{
				do
				{
					pte = mem_get_page(mem, addr);
					memset(pte->storage, value, MEMORY_PAGE_SIZE);
					size -= MEMORY_PAGE_SIZE;
					addr += MEMORY_PAGE_SIZE;
				} while(size >= MEMORY_PAGE_SIZE);
			}
		
			if(size > 0)
			{
				pte = mem_get_page(mem, addr);
				memset(pte->storage, value, size);
			}
		}
		else
		{
			memset(pte->storage + offset, value, size);
		}
	}
}

addr_t mem_align_to_page_boundary(addr_t addr)
{
	return addr - (addr % MEMORY_PAGE_SIZE);
}

void mem_save(memory_t *mem, FILE *f)
{
	int i, j;
	
	for(i = 0; i < PRIMARY_MEMORY_HASH_TABLE_SIZE; i++)
	{
		secondary_memory_hash_table_t *secondary_hash_table = mem->primary_hash_table[i];
		
		if(secondary_hash_table)
		{
			for(j = 0; j < SECONDARY_MEMORY_HASH_TABLE_SIZE; j++)
			{
				memory_page_table_entry_t *pte = secondary_hash_table->pte[j];

				if(pte)
				{
					do
					{
						fwrite(&pte->addr, sizeof(&pte->addr), 1, f);
						fwrite(pte->storage, MEMORY_PAGE_SIZE, 1, f);
					} while((pte = pte->next) != 0);
				}				
			}
		}
	}
}

void mem_restore(memory_t *mem, FILE *f)
{
	addr_t addr;
	
	do
	{
		if(fread(&addr, sizeof(&addr), 1, f) == 1)
		{
			memory_page_table_entry_t *pte = mem_get_page(mem, addr);
			fread(pte->storage, MEMORY_PAGE_SIZE, 1, f);
		}
	} while(!feof(f));
}

void mem_duplicate(memory_t *mem, memory_cb_t *cb)
{
	int i, j;
	
	for(i = 0; i < PRIMARY_MEMORY_HASH_TABLE_SIZE; i++)
	{
		secondary_memory_hash_table_t *secondary_hash_table = mem->primary_hash_table[i];
		
		if(secondary_hash_table)
		{
			for(j = 0; j < SECONDARY_MEMORY_HASH_TABLE_SIZE; j++)
			{
				memory_page_table_entry_t *pte = secondary_hash_table->pte[j];

				if(pte)
				{
					do
					{
						cb->mem_write(cb->instance, pte->addr, pte->storage, MEMORY_PAGE_SIZE);
					} while((pte = pte->next) != 0);
				}				
			}
		}
	}	
}

