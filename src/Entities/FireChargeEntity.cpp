#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "FireChargeEntity.h"
#include "../World.h"





cFireChargeEntity::cFireChargeEntity(cEntity * a_Creator, Vector3d a_Pos, Vector3d a_Speed):
	Super(pkFireCharge, a_Creator, a_Pos, 0.3125f, 0.3125f)
{
	SetSpeed(a_Speed);
	SetGravity(0);
	SetAirDrag(0);
}





void cFireChargeEntity::Explode(Vector3i a_Block)
{
	if (cChunkDef::IsValidHeight(a_Block) && (m_World->GetBlock(a_Block) == E_BLOCK_AIR))
	{
		m_World->SetBlock(a_Block, E_BLOCK_FIRE, 1);
	}
}





void cFireChargeEntity::OnHitSolidBlock(Vector3d a_HitPos, eBlockFace a_HitFace)
{
	Destroy();
	const auto Outside = AddFaceDirection(Vector3i(), a_HitFace);
	Explode((a_HitPos + Vector3d(Outside) * 0.001).Floor());
}





void cFireChargeEntity::OnHitEntity(cEntity & a_EntityHit, Vector3d a_HitPos)
{
	Super::OnHitEntity(a_EntityHit, a_HitPos);

	Destroy();

	if (!a_EntityHit.IsFireproof())
	{
		if (!m_World->DoWithEntityByID(GetCreatorUniqueID(), [&a_EntityHit](cEntity & a_Creator)
			{
				a_EntityHit.TakeDamage(dtRangedAttack, &a_Creator, 5, 0);
				return true;
			}))
		{
			a_EntityHit.TakeDamage(dtRangedAttack, nullptr, 5, 0);
		}
		a_EntityHit.StartBurning(5 * 20);  // 5 seconds of burning
	}
}
