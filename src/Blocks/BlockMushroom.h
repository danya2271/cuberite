
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
		UNUSED(a_ChunkInterface);

		const auto AbovePos = a_RelPos.addedY(1);
		if (cChunkDef::IsValidHeight(AbovePos))
		{
			const auto Light = std::max(a_Chunk.GetBlockLight(AbovePos), a_Chunk.GetSkyLightAltered(AbovePos));
			if (Light > 12)
			{
				a_Chunk.FastSetBlock(a_RelPos, E_BLOCK_AIR, 0);
				return;
			}
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
					if ((Block == E_BLOCK_BROWN_MUSHROOM) || (Block == E_BLOCK_RED_MUSHROOM))
					{
						++Count;
					}
				}
			}
		}
		if ((Count >= 5) || (GetRandomProvider().RandInt(24) != 0))
		{
			return;
		}

		for (int Attempt = 0; Attempt < 4; ++Attempt)
		{
			const auto Target = a_RelPos + Vector3i(GetRandomProvider().RandInt(-4, 4), GetRandomProvider().RandInt(-1, 1), GetRandomProvider().RandInt(-4, 4));
			if (TrySpreadTo(a_Chunk, a_PluginInterface, Target))
			{
				return;
			}
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

		const auto AbovePos = RelPos.addedY(1);
		if (!cChunkDef::IsValidHeight(AbovePos))
		{
			return false;
		}
		const auto Light = std::max(Chunk->GetBlockLight(RelPos), Chunk->GetSkyLightAltered(RelPos));
		if ((Light > 12) || !CanGrowOn(Chunk->GetBlock(AbovePos)))
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
		return cBlockInfo::IsSolid(a_Block) && !cBlockInfo::IsTransparent(a_Block);
	}

	virtual bool CanBeAt(const cChunk & a_Chunk, const Vector3i a_Position, const NIBBLETYPE a_Meta) const override
	{
		const auto BasePos = a_Position.addedY(-1);
		if (!cChunkDef::IsValidHeight(BasePos))
		{
			return false;
		}

		const auto AbovePos = a_Position.addedY(1);
		if (cChunkDef::IsValidHeight(AbovePos))
		{
			const auto Light = std::max(a_Chunk.GetBlockLight(AbovePos), a_Chunk.GetSkyLightAltered(AbovePos));
			if (Light > 12)
			{
				return false;
			}
		}

		switch (a_Chunk.GetBlock(BasePos))
		{
			case E_BLOCK_AIR:
			{
				return false;
			}
		}
		return cBlockInfo::IsSolid(a_Chunk.GetBlock(BasePos)) && !cBlockInfo::IsTransparent(a_Chunk.GetBlock(BasePos));
	}





	virtual ColourID GetMapBaseColourID(NIBBLETYPE a_Meta) const override
	{
		UNUSED(a_Meta);
		return 0;
	}
} ;




