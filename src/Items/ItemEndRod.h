#pragma once

#include "ItemHandler.h"





class cItemEndRodHandler final:
	public cItemHandler
{
	using Super = cItemHandler;

public:

	using Super::Super;

private:

	static NIBBLETYPE BlockFaceToMetaData(const eBlockFace a_BlockFace)
	{
		switch (a_BlockFace)
		{
			case BLOCK_FACE_BOTTOM: return 0;
			case BLOCK_FACE_TOP:    return 1;
			case BLOCK_FACE_ZM:     return 2;
			case BLOCK_FACE_ZP:     return 3;
			case BLOCK_FACE_XM:     return 4;
			case BLOCK_FACE_XP:     return 5;
			default: UNREACHABLE("Unsupported block face");
		}
	}

	virtual bool CommitPlacement(
		cPlayer & a_Player, const cItem & a_HeldItem, const Vector3i a_PlacePosition,
		const eBlockFace a_ClickedBlockFace, const Vector3i a_CursorPosition
	) const override
	{
		UNUSED(a_HeldItem);
		UNUSED(a_CursorPosition);
		if (a_ClickedBlockFace == BLOCK_FACE_NONE)
		{
			return false;
		}

		return a_Player.PlaceBlock(a_PlacePosition, E_BLOCK_END_ROD, BlockFaceToMetaData(a_ClickedBlockFace));
	}

	virtual bool IsPlaceable(void) const override
	{
		return true;
	}
};
