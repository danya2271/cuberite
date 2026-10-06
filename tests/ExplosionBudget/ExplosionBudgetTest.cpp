#include "Physics/ExplosionBudget.h"

#include <stdexcept>

static void Require(bool a_Condition)
{
	if (!a_Condition)
	{
		throw std::runtime_error("Explosion budget assertion failed");
	}
}

int main()
{
	using namespace std::chrono_literals;
	const auto Start = std::chrono::steady_clock::time_point{};
	cExplosionBudget Budget(2, 10ms);
	Require(Budget.TryConsume(Start));
	Require(Budget.TryConsume(Start + 1ms));
	Require(!Budget.TryConsume(Start + 2ms));
	Budget.Reset();
	Require(Budget.TryConsume(Start + 20ms));
	Require(!Budget.TryConsume(Start + 30ms));
	Budget.Reset();
	Require(Budget.TryConsume(Start + 40ms));
	Require(Budget.TryConsume(Start + 49ms));
	cExplosionBudget OtherWorld(1, 10ms);
	Require(OtherWorld.TryConsume(Start));
	Require(!OtherWorld.TryConsume(Start));
	OtherWorld.Reset();
	Require(OtherWorld.TryConsume(Start));
	return 0;
}
