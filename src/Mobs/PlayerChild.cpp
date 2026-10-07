#include "Globals.h"

#include "PlayerChild.h"
#include "../World.h"
#include "../Entities/Player.h"
#include "../UI/PlayerChildWindow.h"
#include "../FastRandom.h"



cPlayerChild::cPlayerChild(Vector3d a_Pos, const cUUID & a_Parent1, const cUUID & a_Parent2, const AString & a_ParentNames) :
	Super("", mtVillager, "entity.villager.hurt", "entity.villager.death", "entity.villager.ambient", 0.6f, 1.0f),
	cEntityWindowOwner(this),
	m_Parent1(a_Parent1),
	m_Parent2(a_Parent2),
	m_Contents(10, 1),
	m_FollowTimer(0)
{
	SetPosition(a_Pos);
	SetMaxHealth(10);
	SetHealth(10);
	SetAge(-1);
	SetRelativeWalkSpeed(0.8);
	m_EMPersonality = PASSIVE;
	SetCanPickUpLoot(false);
	SetCustomName("Child of " + a_ParentNames);
	m_Contents.AddListener(*this);
}





void cPlayerChild::Tick(std::chrono::milliseconds a_Dt, cChunk & a_Chunk)
{
	if (m_FollowTimer <= 0)
	{
		m_FollowTimer = 20;

		cUUID FollowedUUID = GetRandomProvider().RandBool() ? m_Parent1 : m_Parent2;
		cPlayer * FollowedPlayer = nullptr;
		m_World->DoWithPlayerByUUID(FollowedUUID, [&](cPlayer & a_Player)
		{
			FollowedPlayer = &a_Player;
			return true;
		});

		if (FollowedPlayer == nullptr)
		{
			const auto OtherUUID = (FollowedUUID == m_Parent1) ? m_Parent2 : m_Parent1;
			m_World->DoWithPlayerByUUID(OtherUUID, [&](cPlayer & a_Player)
			{
				FollowedPlayer = &a_Player;
				return true;
			});
		}

		if (FollowedPlayer != nullptr)
		{
			MoveToPosition(FollowedPlayer->GetPosition());
		}
		else
		{
			StopMovingToPosition();
		}
	}
	else
	{
		--m_FollowTimer;
	}

	Super::Tick(a_Dt, a_Chunk);
}





void cPlayerChild::OnRightClicked(cPlayer & a_Player)
{
	Super::OnRightClicked(a_Player);

	if (GetWindow() == nullptr)
	{
		OpenNewWindow();
	}

	if ((GetWindow() != nullptr) && (a_Player.GetWindow() != GetWindow()))
	{
		a_Player.OpenWindow(*GetWindow());
	}
}





void cPlayerChild::OnRemoveFromWorld(cWorld & a_World)
{
	if (GetWindow() != nullptr)
	{
		GetWindow()->OwnerDestroyed();
	}

	Super::OnRemoveFromWorld(a_World);
}





void cPlayerChild::GetDrops(cItems & a_Drops, cEntity * a_Killer)
{
	UNUSED(a_Killer);
	m_Contents.CopyToItems(a_Drops);
}





void cPlayerChild::OnSlotChanged(cItemGrid * a_Grid, int a_SlotNum)
{
	UNUSED(a_SlotNum);
	ASSERT(a_Grid == &m_Contents);

	if (m_World == nullptr)
	{
		return;
	}

	if (GetWindow() != nullptr)
	{
		GetWindow()->BroadcastWholeWindow();
	}
	m_World->MarkChunkDirty(GetChunkX(), GetChunkZ());
}





void cPlayerChild::OpenNewWindow(void)
{
	OpenWindow(new cPlayerChildWindow(this));
}
