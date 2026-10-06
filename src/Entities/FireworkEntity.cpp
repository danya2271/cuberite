#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "FireworkEntity.h"
#include "../World.h"
#include "../Chunk.h"
#include "Player.h"
#include "../BoundingBox.h"
#include "../LineBlockTracer.h"
#include "../FastRandom.h"





cFireworkEntity::cFireworkEntity(cEntity * a_Creator, Vector3d a_Pos, const cItem & a_Item) :
	Super(pkFirework, a_Creator, a_Pos, 0.25f, 0.25f),
	m_TicksToExplosion(10 + std::max(0, static_cast<int>(a_Item.m_FireworkItem.m_FlightTimeInTicks)) / 2 + GetRandomProvider().RandInt(5) + GetRandomProvider().RandInt(6)),
	m_FireworkItem(a_Item),
	m_BoostedEntityID((a_Creator != nullptr) && a_Creator->IsElytraFlying() ? a_Creator->GetUniqueID() : INVALID_ID)
{
	SetGravity(0);
	SetAirDrag(0);
	SetSpeed(0, 1, 0);
}





void cFireworkEntity::HandlePhysics(std::chrono::milliseconds a_Dt, cChunk & a_Chunk)
{
	if (m_BoostedEntityID != INVALID_ID)
	{
		if (!m_World->DoWithEntityByID(m_BoostedEntityID, [this](cEntity & a_Entity)
			{
				SetPosition(a_Entity.GetPosition());
				return true;
			}))
		{
			Destroy();
		}
		return;
	}

	const auto Above = GetPosition().Floor() + Vector3i(0, 1, 0);
	if (cChunkDef::IsValidHeight(Above) && (m_World->GetBlock(Above) != E_BLOCK_AIR))
	{
		return;
	}

	AddSpeedY(1);
	AddPosition(GetSpeed() * (static_cast<double>(a_Dt.count()) / 1000));
}





void cFireworkEntity::Tick(std::chrono::milliseconds a_Dt, cChunk & a_Chunk)
{
	Super::Tick(a_Dt, a_Chunk);
	if (!IsTicking())
	{
		// The base class tick destroyed us
		return;
	}

	if (m_TicksToExplosion <= 0)
	{
		const auto & Firework = m_FireworkItem.m_FireworkItem;
		const auto ExplosionCount = ((Firework.m_HasExplosion || !Firework.m_Colours.empty()) ? 1 : 0) + Firework.m_AdditionalExplosions.size();
		if (ExplosionCount != 0)
		{
			m_World->ForEachEntityInBox(cBoundingBox(GetPosition() - Vector3d(5, 5, 5), GetPosition() + Vector3d(5, 5, 5)), [this, ExplosionCount](cEntity & a_Entity)
				{
					if (!a_Entity.IsPawn())
					{
						return false;
					}
					const auto Distance = (a_Entity.GetPosition() - GetPosition()).Length();
					if ((Distance >= 5) || !cLineBlockTracer::LineOfSightTrace(*m_World, GetPosition(), a_Entity.GetPosition() + Vector3d(0, a_Entity.GetHeight() / 2, 0), cLineBlockTracer::losAirWaterLava))
					{
						return false;
					}
					const auto Damage = static_cast<int>(std::ceil((5 + 2 * ExplosionCount) * std::sqrt((5 - Distance) / 5)));
					if (!m_World->DoWithEntityByID(GetCreatorUniqueID(), [&a_Entity, Damage](cEntity & a_Creator)
						{
							a_Entity.TakeDamage(dtRangedAttack, &a_Creator, Damage, 0);
							return true;
						}))
					{
						a_Entity.TakeDamage(dtRangedAttack, nullptr, Damage, 0);
					}
					return false;
				});
		}
		// TODO: Notify the plugins
		m_World->BroadcastEntityAnimation(*this, EntityAnimation::FireworkRocketExplodes);
		Destroy();
		return;
	}

	m_TicksToExplosion -= 1;
}
