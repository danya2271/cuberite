#pragma once

#include <algorithm>
#include <cstddef>

namespace PressurePlatePower
{
	constexpr unsigned char Weighted(const std::size_t a_NumberOfEntities, const std::size_t a_EntitiesPerLevel)
	{
		const auto Count = std::min(a_NumberOfEntities, 15 * a_EntitiesPerLevel);
		return static_cast<unsigned char>((Count + a_EntitiesPerLevel - 1) / a_EntitiesPerLevel);
	}
}
