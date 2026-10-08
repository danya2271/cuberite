
#pragma once

#include "Entity.h"





// tolua_begin
class cFallingBlock :
	public cEntity
{
	// tolua_end

	using Super = cEntity;

public:  // tolua_export
	struct sStaticBlock
	{
		Vector3i Position;
		BLOCKTYPE BlockType;
		NIBBLETYPE BlockMeta;
	};

	CLASS_PROTODEF(cFallingBlock)

	/** Creates a new falling block.
	a_Position is expected in world coords */
	cFallingBlock(Vector3d a_Position, BLOCKTYPE a_BlockType, NIBBLETYPE a_BlockMeta, bool a_IsStatic = false);

	// tolua_begin

	BLOCKTYPE  GetBlockType(void) const { return m_BlockType; }
	NIBBLETYPE GetBlockMeta(void) const { return m_BlockMeta; }
	bool IsStatic(void) const { return m_IsStatic; }

	// tolua_end

	static bool HasStaticAt(cWorld & a_World, Vector3i a_BlockPos);
	static bool DestroyStaticAt(cWorld & a_World, Vector3i a_BlockPos);
	static bool PlaceStaticBlocks(cWorld & a_World, std::initializer_list<sStaticBlock> a_Blocks);

	virtual bool DoesPreventBlockPlacement(void) const override { return false; }
	virtual void GetDrops(cItems & a_Drops, cEntity * a_Killer = nullptr) override;

	// cEntity overrides:
	virtual void SpawnOn(cClientHandle & a_ClientHandle) override;
	virtual void Tick(std::chrono::milliseconds a_Dt, cChunk & a_Chunk) override;

private:
	BLOCKTYPE  m_BlockType;
	NIBBLETYPE m_BlockMeta;
	bool       m_IsStatic;
} ;  // tolua_export
