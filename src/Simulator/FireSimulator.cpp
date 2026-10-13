
#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "FireSimulator.h"
#include "FirePhysics.h"
#include "../EffectID.h"
#include "../BlockInfo.h"
#include "../World.h"
#include "../Defines.h"
#include "../Chunk.h"
#include "../Root.h"
#include "../Bindings/PluginManager.h"





// Easy switch for turning on debugging logging:
#if 0
	#define FIRE_FLOG FLOGD
#else
	#define FIRE_FLOG(...)
#endif





// The base chance that in a tick, rain will extinguish a fire block.
#define CHANCE_BASE_RAIN_EXTINGUISH 0.2
// The additional chance, multiplied by the meta of the fire block, that rain
// will extinguish a fire block in a tick.
#define CHANCE_AGE_M_RAIN_EXTINGUISH 0.03





static constexpr Vector3i gCrossCoords[] =
{
	{ 1, 0,  0},
	{-1, 0,  0},
	{ 0, 0,  1},
	{ 0, 0, -1},
} ;





static constexpr Vector3i gNeighborCoords[] =
{
	{ 1,  0,  0},
	{-1,  0,  0},
	{ 0,  1,  0},
	{ 0, -1,  0},
	{ 0,  0,  1},
	{ 0,  0, -1},
};





////////////////////////////////////////////////////////////////////////////////
// cFireSimulator:

cFireSimulator::cFireSimulator(cWorld & a_World, cIniFile & a_IniFile) :
	cSimulator(a_World)
{
	// Read params from the ini file:
	m_BurnStepTimeFuel    = FirePhysics::BurnStepTime(a_IniFile.GetValueSetI("FireSimulator", "BurnStepTimeFuel",     500));
	m_BurnStepTimeNonfuel = FirePhysics::BurnStepTime(a_IniFile.GetValueSetI("FireSimulator", "BurnStepTimeNonfuel",  100));
	m_Flammability        = a_IniFile.GetValueSetI("FireSimulator", "Flammability",          50);
}





void cFireSimulator::SimulateChunk(std::chrono::milliseconds a_Dt, int a_ChunkX, int a_ChunkZ, cChunk * a_Chunk)
{
	cCoordWithIntList & Data = a_Chunk->GetFireSimulatorData();
	const auto Fires = CollectNearbyFires(*a_Chunk);
	auto Remaining = Data.size();

	int NumMSecs = static_cast<int>(a_Dt.count());
	for (cCoordWithIntList::iterator itr = Data.begin(); (itr != Data.end()) && (Remaining > 0); --Remaining)
	{
		Vector3i relPos(itr->x, itr->y, itr->z);
		auto blockType = a_Chunk->GetBlock(relPos);

		if (!IsAllowedBlock(blockType))
		{
			// The block is no longer eligible (not a fire block anymore; a player probably placed a block over the fire)
			FIRE_FLOG("FS: Removing block {0}", absPos);
			itr = Data.erase(itr);
			continue;
		}

		auto BurnsForever = ((relPos.y > 0) && DoesBurnForever(a_Chunk->GetBlock(relPos.addedY(-1))));
		auto BlockMeta = a_Chunk->GetMeta(relPos);

		auto Raining = std::any_of(std::begin(gCrossCoords), std::end(gCrossCoords), [a_Chunk, relPos](Vector3i cc)
		{
			auto Adjusted = relPos + cc;
			const auto Chunk = a_Chunk->GetRelNeighborChunkAdjustCoords(Adjusted);
			if ((Chunk != nullptr) && Chunk->IsValid())
			{
				return Chunk->IsWeatherWetAt(Adjusted);
			}
			return false;
		});

		// Randomly burn out the fire if it is raining:
		if (!BurnsForever && Raining && GetRandomProvider().RandBool(CHANCE_BASE_RAIN_EXTINGUISH + (BlockMeta * CHANCE_AGE_M_RAIN_EXTINGUISH)))
		{
			a_Chunk->SetBlock(relPos, E_BLOCK_AIR, 0);
			itr = Data.erase(itr);
			continue;
		}

		// Try to spread the fire:
		const auto FireCount = FirePhysics::CountNearbyFires(Fires, relPos);
		ShatterGlass(*a_Chunk, relPos, FireCount);
		TrySpreadFire(a_Chunk, relPos, Fires);

		itr->Data -= NumMSecs;
		if (itr->Data >= 0)
		{
			// Not yet, wait for it longer
			++itr;
			continue;
		}

		// FIRE_FLOG("FS: Fire at {0} is stepping", absPos);

		// TODO: Add some randomness into this
		const auto BurnStep = GetBurnStepTime(a_Chunk, relPos, FireCount);
		if (BurnStep == 0)
		{
			// Fire has no fuel or ground block, extinguish flame
			a_Chunk->SetBlock(relPos, E_BLOCK_AIR, 0);
			itr = Data.erase(itr);
			continue;
		}

		// Has the fire burnt out?
		if (BlockMeta == 0x0f)
		{
			// The fire burnt out completely
			FIRE_FLOG("FS: Fire at {0} burnt out, removing the fire block", absPos);
			a_Chunk->SetBlock(relPos, E_BLOCK_AIR, 0);
			RemoveFuelNeighbors(a_Chunk, relPos, Fires);
			itr = Data.erase(itr);
			continue;
		}

		// Burn out the fire one step by increasing the meta:
		if (!BurnsForever)
		{
			a_Chunk->SetMeta(relPos, BlockMeta + 1);
		}

		itr->Data = BurnStep;
		++itr;
	}  // for itr - Data[]
}





bool cFireSimulator::IsAllowedBlock(BLOCKTYPE a_BlockType)
{
	return (a_BlockType == E_BLOCK_FIRE);
}





bool cFireSimulator::IsFuel(BLOCKTYPE a_BlockType)
{
	switch (a_BlockType)
	{
		case E_BLOCK_PLANKS:
		case E_BLOCK_DOUBLE_WOODEN_SLAB:
		case E_BLOCK_WOODEN_SLAB:
		case E_BLOCK_OAK_WOOD_STAIRS:
		case E_BLOCK_SPRUCE_WOOD_STAIRS:
		case E_BLOCK_BIRCH_WOOD_STAIRS:
		case E_BLOCK_JUNGLE_WOOD_STAIRS:
		case E_BLOCK_LEAVES:
		case E_BLOCK_NEW_LEAVES:
		case E_BLOCK_LOG:
		case E_BLOCK_NEW_LOG:
		case E_BLOCK_WOOL:
		case E_BLOCK_BOOKCASE:
		case E_BLOCK_FENCE:
		case E_BLOCK_SPRUCE_FENCE:
		case E_BLOCK_BIRCH_FENCE:
		case E_BLOCK_JUNGLE_FENCE:
		case E_BLOCK_DARK_OAK_FENCE:
		case E_BLOCK_ACACIA_FENCE:
		case E_BLOCK_OAK_FENCE_GATE:
		case E_BLOCK_SPRUCE_FENCE_GATE:
		case E_BLOCK_BIRCH_FENCE_GATE:
		case E_BLOCK_JUNGLE_FENCE_GATE:
		case E_BLOCK_DARK_OAK_FENCE_GATE:
		case E_BLOCK_ACACIA_FENCE_GATE:
		case E_BLOCK_TNT:
		case E_BLOCK_VINES:
		case E_BLOCK_HAY_BALE:
		case E_BLOCK_TALL_GRASS:
		case E_BLOCK_BIG_FLOWER:
		case E_BLOCK_DANDELION:
		case E_BLOCK_FLOWER:
		case E_BLOCK_CARPET:
		{
			return true;
		}
	}
	return false;
}





bool cFireSimulator::DoesBurnForever(BLOCKTYPE a_BlockType)
{
	return (a_BlockType == E_BLOCK_NETHERRACK);
}





void cFireSimulator::AddBlock(cChunk & a_Chunk, Vector3i a_Position, BLOCKTYPE a_Block)
{
	if (!IsAllowedBlock(a_Block))
	{
		return;
	}

	// Check for duplicates:
	cFireSimulatorChunkData & ChunkData = a_Chunk.GetFireSimulatorData();
	for (cCoordWithIntList::iterator itr = ChunkData.begin(), end = ChunkData.end(); itr != end; ++itr)
	{
		const Vector3i ItrPos{itr->x, itr->y, itr->z};
		if (ItrPos == a_Position)
		{
			return;
		}
	}  // for itr - ChunkData[]

	FIRE_FLOG("FS: Adding block {0}", a_Block);
	ChunkData.emplace_back(a_Position.x, a_Position.y, a_Position.z, 100);
}





int cFireSimulator::GetBurnStepTime(cChunk * a_Chunk, Vector3i a_RelPos, int a_FireCount)
{
	bool IsBlockBelowSolid = false;
	if (a_RelPos.y > 0)
	{
		BLOCKTYPE BlockBelow = a_Chunk->GetBlock(a_RelPos.addedY(-1));
		if (DoesBurnForever(BlockBelow))
		{
			// Is burning atop of netherrack, burn forever (re-check in 10 sec)
			return 10000;
		}
		if (IsFuel(BlockBelow, a_FireCount))
		{
			return static_cast<int>(m_BurnStepTimeFuel);
		}
		IsBlockBelowSolid = cBlockInfo::IsSolid(BlockBelow);
	}

	for (const auto & cross: gNeighborCoords)
	{
		BLOCKTYPE  BlockType;
		NIBBLETYPE BlockMeta;
		if (a_Chunk->UnboundedRelGetBlock(a_RelPos + cross, BlockType, BlockMeta))
		{
			if (IsFuel(BlockType, a_FireCount))
			{
				return static_cast<int>(m_BurnStepTimeFuel);
			}
		}
	}  // for i - gCrossCoords[]

	if (!IsBlockBelowSolid)
	{
		// Checked through everything, nothing was flammable
		// If block below isn't solid, we can't have fire, it would be a non-fueled fire
		return 0;
	}
	return static_cast<int>(m_BurnStepTimeNonfuel);
}





void cFireSimulator::TrySpreadFire(cChunk * a_Chunk, Vector3i a_RelPos, const std::vector<Vector3i> & a_Fires)
{
	/*
	if (GetRandomProvider().RandBool(0.99))
	{
		// Make the chance to spread 100x smaller
		return;
	}
	*/

	for (int x = -1; x <= 1; x++)
	{
		for (int z = -1; z <= 1; z++)
		{
			for (int y = 1; y <= 2; y++)
			{
				// No need to check the coords for equality with the parent block,
				// it cannot catch fire anyway (because it's not an air block)

				if (!GetRandomProvider().RandBool(FirePhysics::SpreadChance(m_Flammability)))
				{
					continue;
				}

				// Start the fire in the neighbor a_RelPos + {x, y, z}
				auto dstRelPos = a_RelPos + Vector3i{x, y, z};
				if (CanStartFireInBlock(a_Chunk, dstRelPos, a_Fires))
				{
					auto dstAbsPos = a_Chunk->RelativeToAbsolute(dstRelPos);
					if (cRoot::Get()->GetPluginManager()->CallHookBlockSpread(m_World, dstAbsPos, ssFireSpread))
					{
						continue;
					}

					FIRE_FLOG("FS: Starting new fire at {0}.", dstAbsPos);
					a_Chunk->UnboundedRelSetBlock(dstRelPos, E_BLOCK_FIRE, 0);
				}
			}  // for y
		}  // for z
	}  // for x
}





void cFireSimulator::RemoveFuelNeighbors(cChunk * a_Chunk, Vector3i a_RelPos, const std::vector<Vector3i> & a_Fires)
{
	for (auto & coord : gNeighborCoords)
	{
		auto relPos = a_RelPos + coord;

		if (!cChunkDef::IsValidHeight(relPos))
		{
			continue;
		}

		const auto neighbor = a_Chunk->GetRelNeighborChunkAdjustCoords(relPos);

		if ((neighbor == nullptr) || !neighbor->IsValid())
		{
			continue;
		}

		BLOCKTYPE BlockType = neighbor->GetBlock(relPos);

		if (!IsFuel(BlockType, FirePhysics::CountNearbyFires(a_Fires, a_RelPos + coord)))
		{
			continue;
		}

		auto absPos = neighbor->RelativeToAbsolute(relPos);
		if (cRoot::Get()->GetPluginManager()->CallHookBlockSpread(m_World, absPos, ssFireSpread))
		{
			continue;
		}
		if (BlockType == E_BLOCK_TNT)
		{
			neighbor->SetBlock(relPos, E_BLOCK_AIR, 0);
			m_World.SpawnPrimedTNT(Vector3d(absPos) + Vector3d(0.5, 0.5, 0.5));  // 80 ticks to boom
			continue;
		}

		neighbor->SetBlock(relPos, FirePhysics::BurnResidue(BlockType), 0);
	}  // for i - Coords[]
}





bool cFireSimulator::CanStartFireInBlock(cChunk * a_NearChunk, Vector3i a_RelPos, const std::vector<Vector3i> & a_Fires)
{
	BLOCKTYPE  BlockType;
	NIBBLETYPE BlockMeta;
	if (!a_NearChunk->UnboundedRelGetBlock(a_RelPos, BlockType, BlockMeta))
	{
		// The chunk is not accessible
		return false;
	}

	if (BlockType != E_BLOCK_AIR)
	{
		// Only an air block can be replaced by a fire block
		return false;
	}

	for (const auto & neighbor: gNeighborCoords)
	{
		if (!a_NearChunk->UnboundedRelGetBlock(a_RelPos + neighbor, BlockType, BlockMeta))
		{
			// Neighbor inaccessible, skip it while evaluating
			continue;
		}
		if (IsFuel(BlockType, FirePhysics::CountNearbyFires(a_Fires, a_RelPos + neighbor)))
		{
			return true;
		}
	}  // for i - Coords[]
	return false;
}





bool cFireSimulator::IsFuel(BLOCKTYPE a_BlockType, int a_FireCount)
{
	return IsFuel(a_BlockType) || (FirePhysics::IsMineralFuel(a_BlockType) && FirePhysics::CanBurnMinerals(a_FireCount));
}





std::vector<Vector3i> cFireSimulator::CollectNearbyFires(cChunk & a_Chunk)
{
	std::vector<Vector3i> Fires;
	for (int ChunkOffsetX = -1; ChunkOffsetX <= 1; ++ChunkOffsetX)
	{
		for (int ChunkOffsetZ = -1; ChunkOffsetZ <= 1; ++ChunkOffsetZ)
		{
			Vector3i Position(ChunkOffsetX * cChunkDef::Width, 0, ChunkOffsetZ * cChunkDef::Width);
			const auto Neighbor = a_Chunk.GetRelNeighborChunkAdjustCoords(Position);
			if ((Neighbor == nullptr) || !Neighbor->IsValid())
			{
				continue;
			}
			for (const auto & Fire : Neighbor->GetFireSimulatorData())
			{
				const Vector3i FirePosition(Fire.x, Fire.y, Fire.z);
				if (Neighbor->GetBlock(FirePosition) == E_BLOCK_FIRE)
				{
					Fires.emplace_back(FirePosition + Vector3i(ChunkOffsetX * cChunkDef::Width, 0, ChunkOffsetZ * cChunkDef::Width));
				}
			}
		}
	}
	return Fires;
}





void cFireSimulator::ShatterGlass(cChunk & a_Chunk, Vector3i a_RelPos, int a_FireCount)
{
	if (a_FireCount <= FirePhysics::GlassThreshold)
	{
		return;
	}
	for (int OffsetX = -1; OffsetX <= 1; ++OffsetX)
	{
		for (int OffsetY = -1; OffsetY <= 1; ++OffsetY)
		{
			for (int OffsetZ = -1; OffsetZ <= 1; ++OffsetZ)
			{
				const Vector3i Offset(OffsetX, OffsetY, OffsetZ);
				BLOCKTYPE Block;
				NIBBLETYPE Meta;
				const auto Position = a_RelPos + Offset;
				if (!FirePhysics::CanShatterGlass(a_FireCount, Offset) ||
					!a_Chunk.UnboundedRelGetBlock(Position, Block, Meta) || !FirePhysics::IsGlass(Block))
				{
					continue;
				}
				const auto Absolute = a_Chunk.RelativeToAbsolute(Position);
				if (!cRoot::Get()->GetPluginManager()->CallHookBlockSpread(m_World, Absolute, ssFireSpread))
				{
					a_Chunk.UnboundedRelSetBlock(Position, E_BLOCK_AIR, 0);
					m_World.BroadcastSoundParticleEffect(EffectID::PARTICLE_BLOCK_BREAK, Absolute, Block | (Meta << 12));
				}
			}
		}
	}
}
