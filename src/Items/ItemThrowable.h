
// Declares the itemhandlers for throwable items: eggs, snowballs, ender pearls, and eyes of ender.

#pragma once

#include "ItemHandler.h"
#include "Entities/ProjectileEntity.h"





class cItemThrowableHandler:
	public cItemHandler
{
	using Super = cItemHandler;

public:

	constexpr cItemThrowableHandler(int a_ItemType, cProjectileEntity::eKind a_ProjectileKind, double a_SpeedCoeff):
		Super(a_ItemType),
		m_ProjectileKind(a_ProjectileKind),
		m_SpeedCoeff(a_SpeedCoeff)
	{
	}





	virtual bool OnItemUse(
		cWorld * a_World,
		cPlayer * a_Player,
		cBlockPluginInterface & a_PluginInterface,
		const cItem & a_HeldItem,
		const Vector3i a_ClickedBlockPos,
		eBlockFace a_ClickedBlockFace
	) const override
	{
		auto Pos = a_Player->GetThrowStartPos();
		auto Speed = a_Player->GetLookVector() * m_SpeedCoeff;

		// Create the projectile:
		if (a_World->CreateProjectile(Pos.x, Pos.y, Pos.z, m_ProjectileKind, a_Player, &a_Player->GetEquippedItem(), &Speed) == cEntity::INVALID_ID)
		{
			return false;
		}

		// Play sound:
		a_World->BroadcastSoundEffect("entity.arrow.shoot", a_Player->GetPosition() - Vector3d(0, a_Player->GetHeight(), 0), 0.5f, 0.4f / GetRandomProvider().RandReal(0.8f, 1.2f));

		// Remove from inventory
		if (!a_Player->IsGameModeCreative())
		{
			a_Player->GetInventory().RemoveOneEquippedItem();
		}

		return true;
	}

protected:

	/** The kind of projectile to create when shooting */
	cProjectileEntity::eKind m_ProjectileKind;

	/** The speed multiplier (to the player's normalized look vector) to set for the new projectile. */
	double m_SpeedCoeff;

	~cItemThrowableHandler() = default;
} ;





class cItemEggHandler final:
	public cItemThrowableHandler
{
	using Super = cItemThrowableHandler;

public:

	constexpr cItemEggHandler(int a_ItemType):
		Super(a_ItemType, cProjectileEntity::pkEgg, 30)
	{
	}
} ;




class cItemSnowballHandler final:
	public cItemThrowableHandler
{
	using Super = cItemThrowableHandler;

public:

	constexpr cItemSnowballHandler(int a_ItemType):
		Super(a_ItemType, cProjectileEntity::pkSnowball, 30)
	{
	}
} ;





class cItemEnderPearlHandler final:
	public cItemThrowableHandler
{
	using Super = cItemThrowableHandler;

public:

	constexpr cItemEnderPearlHandler(int a_ItemType):
		Super(a_ItemType, cProjectileEntity::pkEnderPearl, 30)
	{
	}
} ;





class cItemBottleOEnchantingHandler final :
	public cItemThrowableHandler
{
	using Super = cItemThrowableHandler;

public:

	constexpr cItemBottleOEnchantingHandler(int a_ItemType):
		Super(a_ItemType, cProjectileEntity::pkExpBottle, 14)
	{
	}
};





class cItemFireworkHandler final:
	public cItemThrowableHandler
{
	using Super = cItemThrowableHandler;

public:

	constexpr cItemFireworkHandler(int a_ItemType):
		Super(a_ItemType, cProjectileEntity::pkFirework, 0)
	{
	}





	virtual bool OnItemUse(
		cWorld * a_World,
		cPlayer * a_Player,
		cBlockPluginInterface & a_PluginInterface,
		const cItem & a_HeldItem,
		const Vector3i a_ClickedBlockPos,
		eBlockFace a_ClickedBlockFace
	) const override
	{
		Vector3d Position;
		if (static_cast<cEntity *>(a_Player)->IsElytraFlying())
		{
			Position = a_Player->GetPosition();
		}
		else if ((a_ClickedBlockFace >= 0) && (a_World->GetBlock(a_ClickedBlockPos) != E_BLOCK_AIR))
		{
			const auto LaunchBlock = AddFaceDirection(a_ClickedBlockPos, a_ClickedBlockFace);
			if (!cChunkDef::IsValidHeight(LaunchBlock))
			{
				return false;
			}
			Position = Vector3d(LaunchBlock) + Vector3d(0.5, 0.1, 0.5);
		}
		else
		{
			return false;
		}

		if (a_World->CreateProjectile(Position, m_ProjectileKind, a_Player, &a_HeldItem) == cEntity::INVALID_ID)
		{
			return false;
		}

		if (!a_Player->IsGameModeCreative())
		{
			a_Player->GetInventory().RemoveOneEquippedItem();
		}

		return true;
	}
};
