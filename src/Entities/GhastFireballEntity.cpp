#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "GhastFireballEntity.h"
#include "../World.h"
#include "Player.h"
#include "../Bindings/PluginManager.h"





cGhastFireballEntity::cGhastFireballEntity(cEntity * a_Creator, Vector3d a_Pos, Vector3d a_Speed):
	Super(pkGhastFireball, a_Creator, a_Pos, 1, 1)
{
	SetSpeed(a_Speed);
	SetGravity(0);
	SetAirDrag(0);
}





void cGhastFireballEntity::Explode(Vector3d a_Position)
{
	m_World->DoExplosionAt(1, a_Position.x, a_Position.y, a_Position.z, true, esGhastFireball, this);
}





void cGhastFireballEntity::OnHitSolidBlock(Vector3d a_HitPos, eBlockFace a_HitFace)
{
	Destroy();
	Explode(a_HitPos);
}





void cGhastFireballEntity::OnHitEntity(cEntity & a_EntityHit, Vector3d a_HitPos)
{
	Super::OnHitEntity(a_EntityHit, a_HitPos);
	if (!m_World->DoWithEntityByID(GetCreatorUniqueID(), [&a_EntityHit](cEntity & a_Creator)
		{
			a_EntityHit.TakeDamage(dtRangedAttack, &a_Creator, 6, 0);
			return true;
		}))
	{
		a_EntityHit.TakeDamage(dtRangedAttack, nullptr, 6, 0);
	}
	Destroy();
	Explode(a_HitPos);
}





void cGhastFireballEntity::Deflect(cEntity & a_Attacker, Vector3d a_Direction)
{
	if (a_Direction.Length() < 0.001)
	{
		return;
	}
	a_Direction.Normalize();
	SetSpeed(a_Direction * std::max(20.0, GetSpeed().Length()));
	SetYawFromSpeed();
	SetPitchFromSpeed();
	m_CreatorData.m_UniqueID = a_Attacker.GetUniqueID();
	m_CreatorData.m_Name = a_Attacker.IsPlayer() ? static_cast<cPlayer &>(a_Attacker).GetName() : "";
	m_CreatorData.m_Enchantments = cEnchantments();
	m_TicksAlive = 0;
	m_World->BroadcastEntityVelocity(*this);
}





bool cGhastFireballEntity::DoTakeDamage(TakeDamageInfo & a_TDI)
{
	if ((a_TDI.Attacker == nullptr) || (a_TDI.DamageType != dtAttack) || cPluginManager::Get()->CallHookTakeDamage(*this, a_TDI))
	{
		return false;
	}
	Deflect(*a_TDI.Attacker, a_TDI.Attacker->GetLookVector());
	return true;
}
