/***************************************************************************
                          RegistersBinding.h  -  Renommage des registres
                             -------------------
    begin                : Fri Apr 6 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __REGISTERSBINDING_H__
#define __REGISTERSBINDING_H__

#include <common.h>
#include <iostream>



struct RegistersBinding
{
	tag_t nfree;
	tag_t renames[nRegisters];
	
	struct
	{
		bool valid;
		regnum_t renamedRegister;
		tag_t prev;
		tag_t next;
	} renameBuffers[nRenameBuffers];
	
	tag_t nfpfree;
	tag_t fpRenames[nFloatingPointRegisters];
	
	struct
	{
		bool valid;
		regnum_t renamedRegister;
		tag_t prev;
		tag_t next;
	} fpRenameBuffers[nFloatingPointRenameBuffers];
	
	tag_t crFreeRenames;

	tag_t crRenames[8];
	struct
	{
		bool valid;
		fieldnum_t crfield;
		tag_t prev;
		tag_t next;
	} crRenameBuffers[nCRRenameBuffers];
	
	tag_t lrFreeRenames;
	tag_t lrRename;
	
	struct
	{
		bool renameLR;
		tag_t prev;
		tag_t next;
		bool valid;
	} lrRenameBuffers[nLRRenameBuffers];
	
	tag_t ctrFreeRenames;
	tag_t ctrRename;
	
	struct
	{
		bool renameCTR;
		tag_t prev;
		tag_t next;
		bool valid;
	} ctrRenameBuffers[nCTRRenameBuffers];
	
	RegistersBinding();
	void Reset();
	tag_t AllocateRenameBuffer(regnum_t regNum);
	tag_t AllocateCRRenameBuffer(fieldnum_t crf);
	tag_t AllocateLRRenameBuffer();
	tag_t AllocateCTRRenameBuffer();
	tag_t AllocateFloatingPointRenameBuffer(regnum_t regNum);
	
	tag_t GetRenameBuffer(regnum_t regNum) const;
	tag_t GetCRRenameBuffer(fieldnum_t crf) const;
	tag_t GetLRRenameBuffer() const;
	tag_t GetCTRRenameBuffer() const;
	tag_t GetFloatingPointRenameBuffer(regnum_t regNum) const;
	
	regnum_t GetRegister(tag_t tag) const;
	regnum_t GetFloatingPointRegister(tag_t tag) const;
	fieldnum_t GetField(tag_t tag) const;
	
	void FreeRenameBuffer(tag_t tag);
	void FreeCRRenameBuffer(tag_t tag);	
	void FreeLRRenameBuffer(tag_t tag);	
	void FreeCTRRenameBuffer(tag_t tag);
	void FreeFloatingPointRenameBuffer(tag_t tag);
	
	void FreeRenameBufferReverseOrder(tag_t tag);
	void FreeCRRenameBufferReverseOrder(tag_t tag);	
	void FreeLRRenameBufferReverseOrder(tag_t tag);	
	void FreeCTRRenameBufferReverseOrder(tag_t tag);	
	void FreeFloatingPointRenameBufferReverseOrder(tag_t tag);
	
	tag_t GetFreeRenameBuffers() const { return nfree; }
	tag_t GetFreeCRRenameBuffers() const { return crFreeRenames; }
	tag_t GetFreeLRRenameBuffers() const { return lrFreeRenames; }
	tag_t GetFreeCTRRenameBuffers() const { return ctrFreeRenames; }
	tag_t GetFreeFloatingPointRenameBuffers() const { return nfpfree; }
	
	void ValidateRenameBuffer(tag_t tag);
	void ValidateCRRenameBuffer(tag_t tag);
	void ValidateLRRenameBuffer(tag_t tag);
	void ValidateCTRRenameBuffer(tag_t tag);
	void ValidateFloatingPointRenameBuffer(tag_t tag);
	
	bool RenameBufferValid(tag_t tag) const;
	bool CRRenameBufferValid(tag_t tag) const;
	bool LRRenameBufferValid(tag_t tag) const;
	bool CTRRenameBufferValid(tag_t tag) const;
	bool FloatingPointRenameBufferValid(tag_t tag) const;
	
	bool Precondition();
	
	int operator == (const RegistersBinding& rb) const
	{
		return 0;
	}
	
	friend ostream& operator << (ostream& os, const RegistersBinding& registersBinding);
	
//	void ApplyUpdate(RegistersBindingUpdate& update);
};

#endif
