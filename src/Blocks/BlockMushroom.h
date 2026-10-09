
#pragma once

#include "BlockHandler.h"
#include "BlockPluginInterface.h"
#include "../FastRandom.h"





/** Handler for the small (singleblock) mushrooms. */
class cBlockMushroomHandler final :
	public cClearMetaOnDrop<cBlockHandler>
{
	using Super = cClearMetaOnDrop<cBlockHandler>;

public:

	using Super::Super;

private:

	virtual void OnUpdate(
		cChunkInterface & a_ChunkInterface,
		cWorldInterface & a_WorldInterface,
		cBlockPluginInterface & a_PluginInterface,
		cChunk & a_Chunk,
		const Vector3i a_RelPos
	) const override
	{
		UNUSED(a_WorldInterface);

		if (!a_Chunk.IsLightValid())
		{
			a_Chunk.GetWorld()->QueueLightChunk(a_Chunk.GetPosX(), a_Chunk.GetPosZ());
			return;
		}

		if (!CanBeAt(a_Chunk, a_RelPos, a_Chunk.GetMeta(a_RelPos)))
		{
			a_ChunkInterface.DropBlockAsPickups(a_Chunk.RelativeToAbsolute(a_RelPos));
			return;
		}

		if (GetRandomProvider().RandInt(24) != 0)
		{
			return;
		}

		int Count = 0;
		for (int x = -4; x <= 4; ++x)
		{
			for (int y = -1; y <= 1; ++y)
			{
				for (int z = -4; z <= 4; ++z)
				{
					BLOCKTYPE Block;
					NIBBLETYPE Meta;
					if (!a_Chunk.UnboundedRelGetBlock(a_RelPos + Vector3i(x, y, z), Block, Meta))
					{
						continue;
					}
					if (Block == m_BlockType)
					{
						++Count;
					}
				}
			}
		}
		if (Count >= 5)
		{
			return;
		}

		auto Target = a_RelPos + Vector3i(
			GetRandomProvider().RandInt(-1, 1),
			GetRandomProvider().RandInt(0, 1) - GetRandomProvider().RandInt(0, 1),
			GetRandomProvider().RandInt(-1, 1)
		);
		for (int Attempt = 0; Attempt < 4; ++Attempt)
		{
			if (TrySpreadTo(a_Chunk, a_PluginInterface, Target))
			{
				return;
			}

			Target += Vector3i(
				GetRandomProvider().RandInt(-1, 1),
				GetRandomProvider().RandInt(0, 1) - GetRandomProvider().RandInt(0, 1),
				GetRandomProvider().RandInt(-1, 1)
			);
		}
	}

	bool TrySpreadTo(cChunk & a_Chunk, cBlockPluginInterface & a_PluginInterface, const Vector3i a_RelPos) const
	{
		if (!cChunkDef::IsValidHeight(a_RelPos))
		{
			return false;
		}

		auto RelPos = a_RelPos;
		auto Chunk = a_Chunk.GetRelNeighborChunkAdjustCoords(RelPos);
		if ((Chunk == nullptr) || !Chunk->IsValid())
		{
			return false;
		}

		if (Chunk->GetBlock(RelPos) != E_BLOCK_AIR)
		{
			return false;
		}

		if (!Chunk->IsLightValid())
		{
			return false;
		}

		if ((Chunk->GetBlock(RelPos) != E_BLOCK_AIR) || !CanBeAt(*Chunk, RelPos, 0))
		{
			return false;
		}

		const auto WorldPos = Chunk->RelativeToAbsolute(RelPos);
		if (a_PluginInterface.CallHookBlockSpread(WorldPos, ssMushroomSpread))
		{
			return false;
		}
		Chunk->FastSetBlock(RelPos, m_BlockType, 0);
		return true;
	}

	static bool CanGrowOn(const BLOCKTYPE a_Block)
	{
		return cBlockInfo::FullyOccupiesVoxel(a_Block);
	}

	virtual bool CanBeAt(const cChunk & a_Chunk, const Vector3i a_Position, const NIBBLETYPE a_Meta) const override
	{
		const auto BasePos = a_Position.addedY(-1);
		if (!cChunkDef::IsValidHeight(BasePos))
		{
			return false;
		}

		const auto Base = a_Chunk.GetBlock(BasePos);
		if ((Base == E_BLOCK_MYCELIUM) || ((Base == E_BLOCK_DIRT) && (a_Chunk.GetMeta(BasePos) == E_META_DIRT_PODZOL)))
		{
			return true;
		}

		return (std::max(a_Chunk.GetBlockLight(a_Position), a_Chunk.GetSkyLightAltered(a_Position)) < 13) && CanGrowOn(Base);
	}





	virtual ColourID GetMapBaseColourID(NIBBLETYPE a_Meta) const override
	{
		UNUSED(a_Meta);
		return 0;
	}
} ;




