#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "ZombiePigman.h"
#include "../World.h"
#include "../ClientHandle.h"
#include "../Entities/Player.h"





cZombiePigman::cZombiePigman(void) :
	Super("ZombiePigman", mtZombiePigman, "entity.zombie_pig.hurt", "entity.zombie_pig.death", "entity.zombie_pig.ambient", 0.6f, 1.95f)
{
}





void cZombiePigman::GetDrops(cItems & a_Drops, cEntity * a_Killer)
{
	unsigned int LootingLevel = 0;
	if (a_Killer != nullptr)
	{
		LootingLevel = a_Killer->GetEquippedWeapon().m_Enchantments.GetLevel(cEnchantments::enchLooting);
	}
	AddRandomDropItem(a_Drops, 0, 1 + LootingLevel, E_ITEM_ROTTEN_FLESH);
	AddRandomDropItem(a_Drops, 0, 1 + LootingLevel, E_ITEM_GOLD_NUGGET);

	cItems RareDrops;
	RareDrops.Add(cItem(E_ITEM_GOLD));
	AddRandomRareDropItem(a_Drops, RareDrops, LootingLevel);
	AddRandomArmorDropItem(a_Drops, LootingLevel);
	AddRandomWeaponDropItem(a_Drops, LootingLevel);
}





bool cZombiePigman::DoTakeDamage(TakeDamageInfo & a_TDI)
{
	if (!Super::DoTakeDamage(a_TDI))
	{
		return false;
	}
	if (
		IsPlayerTamed() || (a_TDI.Attacker == nullptr) || !a_TDI.Attacker->IsPlayer() ||
		!static_cast<cPlayer *>(a_TDI.Attacker)->CanMobsTarget()
	)
	{
		return true;
	}
	auto & Attacker = static_cast<cPlayer &>(*a_TDI.Attacker);
	m_World->ForEachEntityInBox(cBoundingBox(GetPosition(), 32, 20, -10), [&](cEntity & a_Entity)
	{
		if (!a_Entity.IsMob() || (a_Entity.GetHealth() <= 0))
		{
			return false;
		}
		auto & Monster = static_cast<cMonster &>(a_Entity);
		if ((Monster.GetMobType() == mtZombiePigman) && !Monster.IsPlayerTamed())
		{
			Monster.SetTarget(&Attacker);
			Monster.m_EMState = CHASING;
		}
		return false;
	});
	return true;
}





void cZombiePigman::SpawnOn(cClientHandle & a_ClientHandle)
{
	Super::SpawnOn(a_ClientHandle);
	a_ClientHandle.SendEntityEquipment(*this, 0, cItem(E_ITEM_GOLD_SWORD));
}
