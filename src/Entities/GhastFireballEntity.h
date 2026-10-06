
// GhastFireballEntity.h

// Declares the cGhastFireballEntity class representing the ghast fireball in flight.





#pragma once

#include "ProjectileEntity.h"





// tolua_begin

class cGhastFireballEntity :
	public cProjectileEntity
{
	// tolua_end

	using Super = cProjectileEntity;

public:  // tolua_export

	CLASS_PROTODEF(cGhastFireballEntity)

	cGhastFireballEntity(cEntity * a_Creator, Vector3d a_Pos, Vector3d a_Speed);

	void Deflect(cEntity & a_Attacker, Vector3d a_Direction);

protected:

	void Explode(Vector3d a_Position);

	// cProjectileEntity overrides:
	virtual void OnHitSolidBlock(Vector3d a_HitPos, eBlockFace a_HitFace) override;
	virtual void OnHitEntity(cEntity & a_EntityHit, Vector3d a_HitPos) override;

	virtual bool DoTakeDamage(TakeDamageInfo & a_TDI) override;

} ;  // tolua_export



