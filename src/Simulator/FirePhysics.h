#pragma once

#include "../BlockType.h"
#include "../FastRandom.h"
#include "../Vector3.h"
#include <algorithm>
#include <limits>

namespace FirePhysics
{
	constexpr int DensityRadius = 4;
	constexpr int GlassThreshold = 5;
	constexpr int MineralThreshold = 20;
	constexpr double ObsidianChance = 0.05;

	inline bool IsMineralFuel(BLOCKTYPE a_Block)
	{
		return (a_Block == E_BLOCK_COBBLESTONE) || (a_Block == E_BLOCK_DIRT) || (a_Block == E_BLOCK_GRASS);
	}

	inline bool IsGlass(BLOCKTYPE a_Block)
	{
		return (a_Block == E_BLOCK_GLASS) || (a_Block == E_BLOCK_GLASS_PANE) ||
			(a_Block == E_BLOCK_STAINED_GLASS) || (a_Block == E_BLOCK_STAINED_GLASS_PANE);
	}

	inline bool CanBurnMinerals(int a_FireCount)
	{
		return a_FireCount > MineralThreshold;
	}

	inline bool CanShatterGlass(int a_FireCount, Vector3i a_Offset)
	{
		return (a_FireCount > GlassThreshold) && (a_Offset.SqrLength() < 4);
	}

	template <typename Positions>
	int CountNearbyFires(const Positions & a_Fires, Vector3i a_Position)
	{
		int Count = 0;
		for (const auto & Fire : a_Fires)
		{
			if (((Fire - a_Position).SqrLength() <= DensityRadius * DensityRadius) && (++Count > MineralThreshold))
			{
				break;
			}
		}
		return Count;
	}

	inline int BurnStepTime(int a_Time)
	{
		return std::clamp(a_Time, 1, std::numeric_limits<int>::max() / 3) * 3;
	}

	inline double SpreadChance(int a_Flammability)
	{
		return std::clamp(a_Flammability / 50000.0, 0.0, 1.0);
	}

	inline BLOCKTYPE BurnResidue(BLOCKTYPE a_Block)
	{
		if (!IsMineralFuel(a_Block))
		{
			return E_BLOCK_SOOT;
		}
		return GetRandomProvider().RandBool(ObsidianChance) ? E_BLOCK_OBSIDIAN : E_BLOCK_BLOCK_OF_COAL;
	}
}
