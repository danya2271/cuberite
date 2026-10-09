
#pragma once

#include "BlockHandler.h"
#include "BlockPluginInterface.h"
#include "../FastRandom.h"





class cBlockMyceliumHandler final :
	public cBlockHandler
{
public:

	using cBlockHandler::cBlockHandler;

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
		const auto AbovePos = a_RelPos.addedY(1);
		if (cChunkDef::IsValidHeight(AbovePos))
		{
			const auto Above = a_Chunk.GetBlock(AbovePos);
			if ((Above != E_BLOCK_SNOW) && (!cBlockInfo::IsTransparent(Above) || IsBlockWater(Above)))
			{
				a_ChunkInterface.SetBlock(a_Chunk.RelativeToAbsolute(a_RelPos), E_BLOCK_DIRT, E_META_DIRT_NORMAL);
				return;
			}
		}

		const auto Light = cChunkDef::IsValidHeight(AbovePos) ?
			std::max(a_Chunk.GetBlockLight(AbovePos), a_Chunk.GetSkyLightAltered(AbovePos)) : 15;
		if (Light < 9)
		{
			return;
		}

		auto & Random = GetRandomProvider();
		for (int Attempt = 0; Attempt < 2; ++Attempt)
		{
			const auto Target = a_RelPos + Vector3i(Random.RandInt(-1, 1), Random.RandInt(-3, 1), Random.RandInt(-1, 1));
			TrySpreadTo(a_Chunk, a_PluginInterface, Target);
		}
	}

	static void TrySpreadTo(cChunk & a_Chunk, cBlockPluginInterface & a_PluginInterface, const Vector3i a_RelPos)
	{
		if (!cChunkDef::IsValidHeight(a_RelPos))
		{
			return;
		}

		auto RelPos = a_RelPos;
		auto Chunk = a_Chunk.GetRelNeighborChunkAdjustCoords(RelPos);
		if ((Chunk == nullptr) || !Chunk->IsValid())
		{
			return;
		}

		BLOCKTYPE Block;
		NIBBLETYPE Meta;
		Chunk->GetBlockTypeMeta(RelPos, Block, Meta);
		if ((Block != E_BLOCK_DIRT) || (Meta != E_META_DIRT_NORMAL))
		{
			return;
		}

		const auto AbovePos = RelPos.addedY(1);
		if (!cChunkDef::IsValidHeight(AbovePos))
		{
			return;
		}
		const auto Above = Chunk->GetBlock(AbovePos);
		const auto Light = std::max(Chunk->GetBlockLight(AbovePos), Chunk->GetSkyLightAltered(AbovePos));
		if (
			(Light < 9) || !cBlockInfo::IsTransparent(Above) ||
			IsBlockLava(Above) || IsBlockWaterOrIce(Above)
		)
		{
			return;
		}

		const auto WorldPos = Chunk->RelativeToAbsolute(RelPos);
		if (!a_PluginInterface.CallHookBlockSpread(WorldPos, ssMycelSpread))
		{
			Chunk->FastSetBlock(RelPos, E_BLOCK_MYCELIUM, 0);
		}
	}

	virtual cItems ConvertToPickups(const NIBBLETYPE a_BlockMeta, const cItem * const a_Tool) const override
	{
		return cItem(E_BLOCK_DIRT, 1, 0);
	}





	virtual ColourID GetMapBaseColourID(NIBBLETYPE a_Meta) const override
	{
		UNUSED(a_Meta);
		return 24;
	}
} ;




