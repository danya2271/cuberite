#pragma once

#include "Window.h"
#include "SlotArea.h"
#include "../Mobs/PlayerChild.h"



class cPlayerChildWindow final:
	public cWindow
{
	using Super = cWindow;

public:

	cPlayerChildWindow(cPlayerChild * a_Child) :
		cWindow(wtChest, "Player child inventory"),
		m_Child(a_Child)
	{
		m_SlotAreas.push_back(new cSlotAreaItemGrid(a_Child->GetContents(), *this));
		m_SlotAreas.push_back(new cSlotAreaInventory(*this));
		m_SlotAreas.push_back(new cSlotAreaHotBar(*this));

		a_Child->GetWorld()->BroadcastSoundEffect("block.chest.open", a_Child->GetPosition(), 1, 1);
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

	virtual ~cPlayerChildWindow() override
	{
		m_Child->GetWorld()->BroadcastSoundEffect("block.chest.close", m_Child->GetPosition(), 1, 1);
	}

private:
	cPlayerChild * m_Child;
};
