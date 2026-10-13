#include "Globals.h"
#include "ClientBlockType.h"
#include "Simulator/FirePhysics.h"
#include "TestHelpers.h"

static void TestFirePhysics()
{
	TEST_FALSE(FirePhysics::CanBurnMinerals(20));
	TEST_TRUE(FirePhysics::CanBurnMinerals(21));
	TEST_FALSE(FirePhysics::CanShatterGlass(5, {1, 0, 0}));
	TEST_TRUE(FirePhysics::CanShatterGlass(6, {1, 1, 0}));
	TEST_FALSE(FirePhysics::CanShatterGlass(6, {2, 0, 0}));
	TEST_EQUAL(FirePhysics::BurnResidue(E_BLOCK_COBBLESTONE), E_BLOCK_OBSIDIAN);
	TEST_EQUAL(FirePhysics::BurnResidue(E_BLOCK_DIRT), E_BLOCK_OBSIDIAN);
	TEST_EQUAL(FirePhysics::BurnResidue(E_BLOCK_PLANKS), E_BLOCK_SOOT);
	TEST_EQUAL(ClientBlockType(E_BLOCK_SOOT), E_BLOCK_SNOW);
	TEST_EQUAL(FirePhysics::BurnStepTime(500), 1500);
	TEST_EQUAL(FirePhysics::SpreadChance(50), 0.001);

	std::vector<Vector3i> Fires;
	for (int Index = 0; Index < 20; ++Index)
	{
		Fires.emplace_back((Index % 5) - 2, 0, (Index / 5) - 2);
	}
	TEST_EQUAL(FirePhysics::CountNearbyFires(Fires, {0, 0, 0}), 20);
	Fires.emplace_back(4, 0, 0);
	TEST_EQUAL(FirePhysics::CountNearbyFires(Fires, {0, 0, 0}), 21);
}

IMPLEMENT_TEST_MAIN("FirePhysics", TestFirePhysics();)
