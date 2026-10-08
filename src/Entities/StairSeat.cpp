#include "Globals.h"

#include "StairSeat.h"
#include "Player.h"
#include "../Blocks/BlockStairs.h"
#include "../ClientHandle.h"
#include "../World.h"



cStairSeat::cStairSeat(const Vector3i a_BlockPos, const NIBBLETYPE a_BlockMeta):
	Super(
		BLOCK_FACE_XP,
		Vector3d(a_BlockPos.x + 0.5, a_BlockPos.y + ((a_BlockMeta & E_BLOCK_STAIRS_UPSIDE_DOWN) ? 1.0 : 0.5), a_BlockPos.z + 0.5)
	),
	m_BlockPos(a_BlockPos)
{
	SetGravity(0);
	SetAirDrag(0);
	SetMaxHealth(1);
	SetHealth(1);
}



bool cStairSeat::Sit(cPlayer & a_Player, const Vector3i a_BlockPos, const NIBBLETYPE a_BlockMeta)
{
	if (a_Player.GetAttached() != nullptr)
	{
		if (a_Player.GetAttached()->IsA(cStairSeat::GetClassStatic()))
		{
			a_Player.Detach();
			return true;
		}
		return false;
	}

	cStairSeat * ExistingSeat = nullptr;
	a_Player.GetWorld()->ForEachEntityInBox(cBoundingBox(a_BlockPos, 1, 1), [&](cEntity & a_Entity)
		{
			if (a_Entity.IsA(cStairSeat::GetClassStatic()) && static_cast<cStairSeat *>(&a_Entity)->IsAt(a_BlockPos))
			{
				ExistingSeat = static_cast<cStairSeat *>(&a_Entity);
				return true;
			}
			return false;
		}
	);

	if (ExistingSeat != nullptr)
	{
		if (!ExistingSeat->HasPassenger())
		{
			a_Player.AttachTo(*ExistingSeat);
		}
		else if (ExistingSeat->HasPassenger(a_Player))
		{
			a_Player.Detach();
		}
		return true;
	}

	auto Seat = std::make_unique<cStairSeat>(a_BlockPos, a_BlockMeta);
	auto * SeatPtr = Seat.get();
	if (!SeatPtr->Initialize(std::move(Seat), *a_Player.GetWorld()))
	{
		return false;
	}

	const auto PlayerID = a_Player.GetUniqueID();
	a_Player.GetWorld()->QueueTask([SeatPtr, PlayerID](cWorld & a_World)
		{
			if (!SeatPtr->IsTicking())
			{
				return;
			}
			a_World.DoWithEntityByID(PlayerID, [SeatPtr](cEntity & a_Entity)
			{
				if (a_Entity.IsPlayer() && (a_Entity.GetAttached() == nullptr))
				{
					a_Entity.AttachTo(*SeatPtr);
				}
				return true;
			});
		}
	);
	return true;
}



void cStairSeat::SpawnOn(cClientHandle & a_ClientHandle)
{
	a_ClientHandle.SendSpawnEntity(*this);
	a_ClientHandle.SendEntityMetadata(*this);
}



void cStairSeat::Tick(std::chrono::milliseconds a_Dt, cChunk & a_Chunk)
{
	if (!HasPassenger() || !cBlockStairsHandler::IsAnyStairType(m_World->GetBlock(m_BlockPos)))
	{
		Destroy();
		return;
	}

	cEntity::Tick(a_Dt, a_Chunk);
}
