
//  WitherSkullEntity.cpp

// Implements the cWitherSkullEntity class representing the entity used by both blue and black wither skulls

#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "WitherSkullEntity.h"
#include "../World.h"
#include "Pawn.h"





cWitherSkullEntity::cWitherSkullEntity(cEntity * a_Creator, Vector3d a_Pos, Vector3d a_Speed):
	Super(pkWitherSkull, a_Creator, a_Pos, 0.3125f, 0.3125f)
{
	SetSpeed(a_Speed);
	SetGravity(0);
	SetAirDrag(0);
}





void cWitherSkullEntity::OnHitSolidBlock(Vector3d a_HitPos, eBlockFace a_HitFace)
{
	Destroy();
	m_World->DoExplosionAt(1, a_HitPos.x, a_HitPos.y, a_HitPos.z, false, esWitherSkull, this);
}





void cWitherSkullEntity::OnHitEntity(cEntity & a_EntityHit, Vector3d a_HitPos)
{
	Super::OnHitEntity(a_EntityHit, a_HitPos);
	if (!m_World->DoWithEntityByID(GetCreatorUniqueID(), [&](cEntity & a_Creator)
		{
			const auto HealthBefore = a_EntityHit.GetHealth();
			a_EntityHit.TakeDamage(dtRangedAttack, &a_Creator, 8, 0);
			if ((HealthBefore > 0) && (a_EntityHit.GetHealth() <= 0))
			{
				a_Creator.Heal(5);
			}
			return true;
		}))
	{
		a_EntityHit.TakeDamage(dtRangedAttack, nullptr, 5, 0);
	}
	if (a_EntityHit.IsPawn() && (a_EntityHit.GetHealth() > 0))
	{
		static_cast<cPawn &>(a_EntityHit).AddEntityEffect(cEntityEffect::effWither, 200, 1);
	}

	Destroy();
	m_World->DoExplosionAt(1, a_HitPos.x, a_HitPos.y, a_HitPos.z, false, esWitherSkull, this);
}




