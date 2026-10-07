#pragma once

#include "Monster.h"
#include "../ItemGrid.h"
#include "../UI/WindowOwner.h"



class cPlayerChild final:
	public cMonster,
	public cItemGrid::cListener,
	public cEntityWindowOwner
{
	using Super = cMonster;

public:

	CLASS_PROTODEF(cPlayerChild)

	cPlayerChild(Vector3d a_Pos, const cUUID & a_Parent1, const cUUID & a_Parent2, const AString & a_ParentNames);

	const cItemGrid & GetContents(void) const { return m_Contents; }
	cItemGrid & GetContents(void) { return m_Contents; }

	virtual void Tick(std::chrono::milliseconds a_Dt, cChunk & a_Chunk) override;
	virtual void OnRightClicked(cPlayer & a_Player) override;
	virtual void OnRemoveFromWorld(cWorld & a_World) override;
	virtual void GetDrops(cItems & a_Drops, cEntity * a_Killer = nullptr) override;

protected:
	virtual void OnSlotChanged(cItemGrid * a_Grid, int a_SlotNum) override;

private:

	void OpenNewWindow(void);

	cUUID m_Parent1;
	cUUID m_Parent2;
	cItemGrid m_Contents;
	int m_FollowTimer;
};
