#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "ThrownSnowballEntity.h"
#include "../World.h"
#include "Pawn.h"





cThrownSnowballEntity::cThrownSnowballEntity(cEntity * a_Creator, Vector3d a_Pos, Vector3d a_Speed, bool a_IsSoot):
	Super(pkSnowball, a_Creator, a_Pos, a_Speed, 0.25f, 0.25f),
	m_IsSoot(a_IsSoot)
{
}





void cThrownSnowballEntity::OnHitEntity(cEntity & a_EntityHit, Vector3d a_HitPos)
{
	Super::OnHitEntity(a_EntityHit, a_HitPos);

	int Damage = 0;
	if (a_EntityHit.IsMob() && (static_cast<cMonster &>(a_EntityHit).GetMobType() == mtBlaze))
	{
		// Blazes take 3 damage:
		Damage = 3;
	}
	else if (a_EntityHit.IsEnderCrystal())
	{
		// Endercrystals are destroyed:
		Damage = CeilC(a_EntityHit.GetHealth());
	}

	a_EntityHit.TakeDamage(dtRangedAttack, GetCreatorUniqueID(), Damage, 1);
	if (m_IsSoot && a_EntityHit.IsPlayer())
	{
		auto & Player = static_cast<cPawn &>(a_EntityHit);
		Player.AddEntityEffect(cEntityEffect::effSlowness, 100, 1);
		Player.AddEntityEffect(cEntityEffect::effBlindness, 100, 1);
	}
	m_World->BroadcastEntityAnimation(*this, EntityAnimation::SnowballPoofs);
	Destroy();
}





void cThrownSnowballEntity::OnHitSolidBlock(Vector3d a_HitPos, eBlockFace a_HitFace)
{
	Super::OnHitSolidBlock(a_HitPos, a_HitFace);

	m_World->BroadcastEntityAnimation(*this, EntityAnimation::SnowballPoofs);
	Destroy();
}
