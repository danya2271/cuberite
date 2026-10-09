
#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "Wither.h"

#include "../World.h"
#include "../Entities/Player.h"
#include "../Entities/WitherSkullEntity.h"
#include "../ClientHandle.h"
#include "../CompositeChat.h"





cWither::cWither(void) :
	Super("Wither", mtWither, "entity.wither.hurt", "entity.wither.death", "entity.wither.ambient", 0.9f, 3.5f),
	m_WitherInvulnerableTicks(220)
{
	SetMaxHealth(300);
	SetHealth(GetMaxHealth() / 3);
}





bool cWither::IsArmored(void) const
{
	return GetHealth() <= (GetMaxHealth() / 2);
}





bool cWither::DoTakeDamage(TakeDamageInfo & a_TDI)
{
	if (a_TDI.DamageType == dtDrowning)
	{
		return false;
	}

	if (m_WitherInvulnerableTicks > 0)
	{
		return false;
	}

	if (IsArmored() && (a_TDI.DamageType == dtRangedAttack))
	{
		return false;
	}

	if (!Super::DoTakeDamage(a_TDI))
	{
		return false;
	}

	m_World->BroadcastBossBarUpdateHealth(*this, GetUniqueID(), GetHealth() / GetMaxHealth());
	return true;
}





void cWither::GetDrops(cItems & a_Drops, cEntity * a_Killer)
{
	AddRandomDropItem(a_Drops, 1, 1, E_ITEM_NETHER_STAR);
}





void cWither::KilledBy(TakeDamageInfo & a_TDI)
{
	Super::KilledBy(a_TDI);

	Vector3d Pos = GetPosition();
	m_World->ForEachPlayer([=](cPlayer & a_Player)
		{
			// TODO 2014-05-21 xdot: Vanilla minecraft uses an AABB check instead of a radius one
			double Dist = (a_Player.GetPosition() - Pos).Length();
			if (Dist < 50.0)
			{
				// If player is close, award achievement
				a_Player.AwardAchievement(CustomStatistic::AchKillWither);
			}
			return false;
		}
	);
}





void cWither::SpawnOn(cClientHandle & a_Client)
{
	Super::SpawnOn(a_Client);

	// Purple boss bar with no divisions that darkens the sky:
	a_Client.SendBossBarAdd(GetUniqueID(), cCompositeChat("Wither"), GetHealth() / GetMaxHealth(), BossBarColor::Purple, BossBarDivisionType::None, true, false, false);
}





void cWither::Tick(std::chrono::milliseconds a_Dt, cChunk & a_Chunk)
{
	Super::Tick(a_Dt, a_Chunk);
	if (!IsTicking())
	{
		// The base class tick destroyed us
		return;
	}
	if (IsPlayerTamed())
	{
		return;
	}

	if (m_WitherInvulnerableTicks > 0)
	{
		unsigned int NewTicks = m_WitherInvulnerableTicks - 1;

		if (NewTicks == 0)
		{
			m_World->DoExplosionAt(7.0, GetPosX(), GetPosY(), GetPosZ(), false, esWitherBirth, this);
		}

		m_WitherInvulnerableTicks = NewTicks;

		if ((NewTicks % 10) == 0)
		{
			Heal(10);
		}
	}
	else if ((GetTicksAlive() % 20) == 0)
	{
		Heal(1);
		m_World->BroadcastBossBarUpdateHealth(*this, GetUniqueID(), GetHealth() / GetMaxHealth());
	}

	m_World->BroadcastEntityMetadata(*this);
}





bool cWither::Attack(std::chrono::milliseconds a_Dt)
{
	if ((m_WitherInvulnerableTicks > 0) || (GetTarget() == nullptr) || (m_AttackCoolDownTicksLeft != 0))
	{
		return false;
	}
	const auto Origin = GetPosition().addedY(2.5);
	auto Direction = GetTarget()->GetPosition().addedY(GetTarget()->GetHeight() / 2) - Origin;
	if (Direction.SqrLength() < 0.0001)
	{
		return false;
	}
	Direction.Normalize();
	auto Skull = std::make_unique<cWitherSkullEntity>(this, Origin + Direction, Direction * 20);
	const auto SkullPtr = Skull.get();
	if (!SkullPtr->Initialize(std::move(Skull), *m_World))
	{
		return false;
	}
	ResetAttackCooldown();
	m_World->BroadcastSoundEffect("entity.wither.shoot", GetPosition(), 1, 1);
	return true;
}
