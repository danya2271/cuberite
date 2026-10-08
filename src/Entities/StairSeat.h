#pragma once

#include "LeashKnot.h"



class cStairSeat final:
	public cLeashKnot
{
	using Super = cLeashKnot;

public:

	CLASS_PROTODEF(cStairSeat)

	cStairSeat(Vector3i a_BlockPos, NIBBLETYPE a_BlockMeta);

	static bool Sit(cPlayer & a_Player, Vector3i a_BlockPos, NIBBLETYPE a_BlockMeta);

	bool IsAt(Vector3i a_BlockPos) const { return m_BlockPos == a_BlockPos; }
	bool HasPassenger(void) const { return m_Attachee != nullptr; }
	bool HasPassenger(const cEntity & a_Entity) const { return m_Attachee == &a_Entity; }

	virtual bool IsInvisible(void) const override { return true; }
	virtual bool DoesPreventBlockPlacement(void) const override { return false; }

private:

	Vector3i m_BlockPos;

	virtual void SpawnOn(cClientHandle & a_ClientHandle) override;
	virtual void Tick(std::chrono::milliseconds a_Dt, cChunk & a_Chunk) override;
};
