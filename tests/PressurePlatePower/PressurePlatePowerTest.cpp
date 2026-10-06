#include "Simulator/IncrementalRedstoneSimulator/PressurePlatePower.h"

#include <limits>
#include <stdexcept>

static void Require(bool a_Condition)
{
	if (!a_Condition)
	{
		throw std::runtime_error("Weighted pressure plate assertion failed");
	}
}

int main()
{
	using PressurePlatePower::Weighted;
	Require(Weighted(0, 1) == 0);
	Require(Weighted(1, 1) == 1);
	Require(Weighted(14, 1) == 14);
	Require(Weighted(15, 1) == 15);
	Require(Weighted(256, 1) == 15);
	Require(Weighted(std::numeric_limits<std::size_t>::max(), 1) == 15);
	Require(Weighted(0, 10) == 0);
	Require(Weighted(1, 10) == 1);
	Require(Weighted(10, 10) == 1);
	Require(Weighted(11, 10) == 2);
	Require(Weighted(140, 10) == 14);
	Require(Weighted(141, 10) == 15);
	Require(Weighted(150, 10) == 15);
	Require(Weighted(2560, 10) == 15);
	Require(Weighted(std::numeric_limits<std::size_t>::max(), 10) == 15);
	return 0;
}
