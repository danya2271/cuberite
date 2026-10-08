
#pragma once

#include "ItemHandler.h"
#include "Blocks/BlockChest.h"





class cItemChestHandler final :
	public cItemHandler
{
	using Super = cItemHandler;

public:

	using Super::Super;

	cItemChestHandler(const cItemChestHandler &) = delete;

private:

	virtual bool CommitPlacement(cPlayer & a_Player, const cItem & a_HeldItem, const Vector3i a_PlacePosition, const eBlockFace a_ClickedBlockFace, const Vector3i a_CursorPosition) const override
	{
		// Use one same-type neighbor for double-chest orientation when available.
		static const Vector3i CrossCoords[] =
		{
			{-1, 0,  0},
			{ 0, 0, -1},
			{ 1, 0,  0},
			{ 0, 0,  1},
		};

		auto & World = *a_Player.GetWorld();
		int NeighborIdx = -1;

		for (size_t i = 0; i < ARRAYCOUNT(CrossCoords); i++)
		{
			const auto NeighborPos = a_PlacePosition + CrossCoords[i];
			if (World.GetBlock(NeighborPos) != m_ItemType)
			{
				continue;
			}
			if (NeighborIdx < 0)
			{
				NeighborIdx = static_cast<int>(i);
			}

		}  // for i

		// Get the meta of the placed chest; take existing neighbors into account:
		BLOCKTYPE ChestBlockType = static_cast<BLOCKTYPE>(m_ItemType);
		NIBBLETYPE Meta;
		const auto yaw = a_Player.GetYaw();
		switch (NeighborIdx)
		{
			case 0:
			case 2:
			{
				// The neighbor is in the X axis, form a X-axis-aligned dblchest:
				Meta = ((yaw >= -90) && (yaw < 90)) ? E_META_CHEST_FACING_ZM : E_META_CHEST_FACING_ZP;
				break;
			}
			case 1:
			case 3:
			{
				// The neighbor is in the Z axis, form a Z-axis-aligned dblchest:
				Meta = (yaw < 0) ? E_META_CHEST_FACING_XM : E_META_CHEST_FACING_XP;
				break;
			}
			default:
			{
				// No neighbor, place based on yaw:
				Meta = cBlockChestHandler::YawToMetaData(yaw);
				break;
			}
		}  // switch (NeighborIdx)

		// Place the new chest:
		if (!a_Player.PlaceBlock(a_PlacePosition, ChestBlockType, Meta))
		{
			return false;
		}

		// Adjust the existing chest, if any:
		if (NeighborIdx != -1)
		{
			World.FastSetBlock(a_PlacePosition + CrossCoords[NeighborIdx], ChestBlockType, Meta);
		}

		return true;
	}
};
