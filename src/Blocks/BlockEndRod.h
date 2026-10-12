#pragma once

#include "BlockHandler.h"
#include "../BlockInfo.h"
#include "../Chunk.h"
#include "Mixins/Mixins.h"





class cBlockEndRodHandler final:
	public cClearMetaOnDrop<cBlockHandler>
{
	using Super = cClearMetaOnDrop<cBlockHandler>;

public:

	using Super::Super;

	/** Converts end-rod metadata to the direction in which the rod points. */
	static eBlockFace MetaDataToDirection(const NIBBLETYPE a_Meta)
	{
		switch (a_Meta & 0x07)
		{
			case 0: return BLOCK_FACE_YM;
			case 1: return BLOCK_FACE_YP;
			case 2: return BLOCK_FACE_ZM;
			case 3: return BLOCK_FACE_ZP;
			case 4: return BLOCK_FACE_XM;
			case 5: return BLOCK_FACE_XP;
			default: return BLOCK_FACE_NONE;
		}
	}

private:

	static NIBBLETYPE RotateCW(const NIBBLETYPE a_Meta)
	{
		switch (a_Meta & 0x07)
		{
			case 2: return 5;
			case 5: return 3;
			case 3: return 4;
			case 4: return 2;
			default: return a_Meta;
		}
	}

	static NIBBLETYPE RotateCCW(const NIBBLETYPE a_Meta)
	{
		switch (a_Meta & 0x07)
		{
			case 2: return 4;
			case 4: return 3;
			case 3: return 5;
			case 5: return 2;
			default: return a_Meta;
		}
	}

	virtual bool CanBeAt(const cChunk & a_Chunk, const Vector3i a_Position, const NIBBLETYPE a_Meta) const override
	{
		const auto Direction = MetaDataToDirection(a_Meta);
		if (Direction == BLOCK_FACE_NONE)
		{
			return false;
		}

		BLOCKTYPE SupportType;
		if (!a_Chunk.UnboundedRelGetBlockType(AddFaceDirection(a_Position, Direction, true), SupportType))
		{
			return false;
		}

		return cBlockInfo::FullyOccupiesVoxel(SupportType);
	}

	virtual NIBBLETYPE MetaRotateCW(const NIBBLETYPE a_Meta) const override
	{
		return RotateCW(a_Meta);
	}

	virtual NIBBLETYPE MetaRotateCCW(const NIBBLETYPE a_Meta) const override
	{
		return RotateCCW(a_Meta);
	}

	virtual NIBBLETYPE MetaMirrorXY(const NIBBLETYPE a_Meta) const override
	{
		switch (a_Meta & 0x07)
		{
			case 2: return 3;
			case 3: return 2;
			default: return a_Meta;
		}
	}

	virtual NIBBLETYPE MetaMirrorXZ(const NIBBLETYPE a_Meta) const override
	{
		switch (a_Meta & 0x07)
		{
			case 0: return 1;
			case 1: return 0;
			default: return a_Meta;
		}
	}

	virtual NIBBLETYPE MetaMirrorYZ(const NIBBLETYPE a_Meta) const override
	{
		switch (a_Meta & 0x07)
		{
			case 4: return 5;
			case 5: return 4;
			default: return a_Meta;
		}
	}
};
