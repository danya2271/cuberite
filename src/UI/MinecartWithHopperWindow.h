#pragma once

#include "Window.h"
#include "SlotArea.h"
#include "../Entities/Minecart.h"





class cMinecartWithHopperWindow final:
	public cWindow
{
	using Super = cWindow;

public:
	cMinecartWithHopperWindow(cMinecartWithHopper & a_Minecart):
		Super(wtHopper, "Minecart with Hopper")
	{
		m_SlotAreas.push_back(new cSlotAreaItemGrid(a_Minecart.GetContents(), *this));
		m_SlotAreas.push_back(new cSlotAreaInventory(*this));
		m_SlotAreas.push_back(new cSlotAreaHotBar(*this));
	}

	virtual void DistributeStack(cItem & a_ItemStack, int a_Slot, cPlayer & a_Player, cSlotArea * a_ClickedArea, bool a_ShouldApply) override
	{
		cSlotAreas Areas;
		if (a_ClickedArea == m_SlotAreas[0])
		{
			Areas.push_back(m_SlotAreas[2]);
			Areas.push_back(m_SlotAreas[1]);
		}
		else
		{
			Areas.push_back(m_SlotAreas[0]);
		}
		Super::DistributeStackToAreas(a_ItemStack, a_Player, Areas, a_ShouldApply, a_ClickedArea == m_SlotAreas[0]);
	}
};
