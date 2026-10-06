#pragma once

#include "../../BoundingBox.h"
#include "../../Blocks/BlockComparator.h"
#include "../../Entities/Minecart.h"

namespace DetectorRailHandler
{
	static PowerLevel GetPowerDeliveredToPosition(const cChunk & a_Chunk, Vector3i a_Position, BLOCKTYPE a_BlockType, Vector3i a_QueryPosition, BLOCKTYPE a_QueryBlockType, bool a_IsLinked)
	{
		UNUSED(a_BlockType);
		if (((a_Chunk.GetMeta(a_Position) & 0x08) == 0) || (a_IsLinked && (a_QueryPosition != a_Position + OffsetYM)))
		{
			return 0;
		}
		if (!a_IsLinked && ((a_QueryBlockType == E_BLOCK_ACTIVE_COMPARATOR) || (a_QueryBlockType == E_BLOCK_INACTIVE_COMPARATOR)))
		{
			auto Relative = a_QueryPosition;
			const auto QueryChunk = a_Chunk.GetRelNeighborChunkAdjustCoords(Relative);
			if ((QueryChunk != nullptr) && QueryChunk->IsValid() &&
				(cBlockComparatorHandler::GetRearCoordinate(a_QueryPosition, QueryChunk->GetMeta(Relative) & 0x03) == a_Position))
			{
				return DataForChunk(a_Chunk).GetCachedPowerData(a_Position);
			}
		}
		return 15;
	}

	static void Update(cChunk & a_Chunk, cChunk & a_CurrentlyTicking, Vector3i a_Position, BLOCKTYPE a_BlockType, NIBBLETYPE a_Meta, const PowerLevel a_Power)
	{
		UNUSED(a_BlockType);
		UNUSED(a_Power);
		bool Occupied = false;
		bool FoundInventory = false;
		PowerLevel AnalogPower = 0;
		const auto Absolute = cChunkDef::RelativeToAbsolute(a_Position, a_Chunk.GetPos());
		const cBoundingBox DetectionBox(Vector3d(Absolute) + Vector3d(0.2, 0, 0.2), Vector3d(Absolute) + Vector3d(0.8, 0.8, 0.8));
		a_Chunk.GetWorld()->ForEachEntityInBox(DetectionBox, [&](cEntity & a_Entity)
		{
			if (!a_Entity.IsMinecart() || (a_Entity.GetHealth() <= 0))
			{
				return false;
			}
			Occupied = true;
			const auto & Minecart = static_cast<const cMinecart &>(a_Entity);
			if (!FoundInventory && (Minecart.GetPayload() == cMinecart::mpChest))
			{
				FoundInventory = true;
				const auto & Chest = static_cast<const cMinecartWithChest &>(Minecart);
				constexpr int NumSlots = cMinecartWithChest::ContentsWidth * cMinecartWithChest::ContentsHeight;
				double Fullness = 0;
				for (int Slot = 0; Slot < NumSlots; ++Slot)
				{
					const auto & Item = Chest.GetSlot(Slot);
					if (!Item.IsEmpty())
					{
						Fullness += static_cast<double>(Item.m_ItemCount) / Item.GetMaxStackSize();
					}
				}
				AnalogPower = (Fullness == 0) ? 0 : static_cast<PowerLevel>(1 + std::min(1.0, Fullness / NumSlots) * 14);
			}
			return false;
		});
		auto & Data = DataForChunk(a_Chunk);
		const auto PreviousAnalogPower = Data.ExchangeUpdateOncePowerData(a_Position, AnalogPower);
		const bool PreviouslyOccupied = (a_Meta & 0x08) != 0;
		if ((Occupied != PreviouslyOccupied) || (AnalogPower != PreviousAnalogPower))
		{
			if (Occupied != PreviouslyOccupied)
			{
				a_Chunk.SetMeta(a_Position, Occupied ? (a_Meta | 0x08) : (a_Meta & 0x07));
			}
			UpdateAdjustedRelatives(a_Chunk, a_CurrentlyTicking, a_Position, RelativeAdjacents);
		}
	}

	static void ForValidSourcePositions(const cChunk & a_Chunk, Vector3i a_Position, BLOCKTYPE a_BlockType, NIBBLETYPE a_Meta, ForEachSourceCallback & a_Callback)
	{
		UNUSED(a_Chunk);
		UNUSED(a_Position);
		UNUSED(a_BlockType);
		UNUSED(a_Meta);
		UNUSED(a_Callback);
	}
}
