#pragma once

#include "LeashKnot.h"



// tolua_begin
class cStairSeat final:
	public cLeashKnot
{
	// tolua_end
	using Super = cLeashKnot;

public:

	CLASS_PROTODEF(cStairSeat)

	cStairSeat(Vector3i a_BlockPos, NIBBLETYPE a_BlockMeta);
	cStairSeat(Vector3i a_BlockPos, double a_SeatY, bool a_IsEndRod);

	static bool Sit(cPlayer & a_Player, Vector3i a_BlockPos, NIBBLETYPE a_BlockMeta);
	static bool SitOnEndRod(cPlayer & a_Player, Vector3i a_BlockPos);  // tolua_export

	bool IsAt(Vector3i a_BlockPos) const { return m_BlockPos == a_BlockPos; }
	bool IsEndRod(void) const { return m_IsEndRod; }
	bool HasPassenger(void) const { return m_Attachee != nullptr; }
	bool HasPassenger(const cEntity & a_Entity) const { return m_Attachee == &a_Entity; }

	virtual bool IsInvisible(void) const override { return true; }
	virtual bool DoesPreventBlockPlacement(void) const override { return false; }

private:
	Vector3i m_BlockPos;
	bool m_IsEndRod;

	static bool SitAt(cPlayer & a_Player, Vector3i a_BlockPos, double a_SeatY, bool a_IsEndRod);

	virtual void SpawnOn(cClientHandle & a_ClientHandle) override;
	virtual void Tick(std::chrono::milliseconds a_Dt, cChunk & a_Chunk) override;
};  // tolua_export
