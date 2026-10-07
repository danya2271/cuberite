#pragma once

#include "Window.h"
#include "SlotArea.h"
#include "../Mobs/Monster.h"



class cTamedMobWindow final:
	public cWindow
{
	using Super = cWindow;

public:

	cTamedMobWindow(cMonster * a_Mob) :
		cWindow(wtChest, "Tamed mob inventory"),
		m_Mob(a_Mob)
	{
		m_SlotAreas.push_back(new cSlotAreaItemGrid(a_Mob->GetPlayerOwnerContents(), *this, 18));
		m_SlotAreas.push_back(new cSlotAreaInventory(*this));
		m_SlotAreas.push_back(new cSlotAreaHotBar(*this));
	}

	virtual void DistributeStack(cItem & a_ItemStack, int a_Slot, cPlayer & a_Player, cSlotArea * a_ClickedArea, bool a_ShouldApply) override
	{
		cSlotAreas AreasInOrder;

		if (a_ClickedArea == m_SlotAreas[0])
		{
			AreasInOrder.push_back(m_SlotAreas[2]);
			AreasInOrder.push_back(m_SlotAreas[1]);
			Super::DistributeStackToAreas(a_ItemStack, a_Player, AreasInOrder, a_ShouldApply, true);
		}
		else
		{
			AreasInOrder.push_back(m_SlotAreas[0]);
			Super::DistributeStackToAreas(a_ItemStack, a_Player, AreasInOrder, a_ShouldApply, false);
		}
	}

private:
	cMonster * m_Mob;
};
