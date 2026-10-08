#include "Globals.h"

#include "FallingBlock.h"
#include "../BlockInfo.h"
#include "../Blocks/BlockHandler.h"
#include "../World.h"
#include "../ClientHandle.h"
#include "../Simulator/SandSimulator.h"
#include "../Chunk.h"





cFallingBlock::cFallingBlock(Vector3d a_Position, BLOCKTYPE a_BlockType, NIBBLETYPE a_BlockMeta, bool a_IsStatic):
	Super(etFallingBlock, a_Position, 0.98f, 0.98f),
	m_BlockType(a_BlockType),
	m_BlockMeta(a_BlockMeta),
	m_IsStatic(a_IsStatic)
{
	SetGravity(a_IsStatic ? 0.0f : -16.0f);
	SetAirDrag(a_IsStatic ? 0.0f : 0.02f);
}



bool cFallingBlock::HasStaticAt(cWorld & a_World, const Vector3i a_BlockPos)
{
	bool Found = false;
	a_World.ForEachPendingEntity([&Found, a_BlockPos](cEntity & a_Entity)
		{
			if (!a_Entity.IsFallingBlock())
			{
				return false;
			}

			const auto & FallingBlock = static_cast<const cFallingBlock &>(a_Entity);
			if (FallingBlock.IsStatic() && (Vector3i(FloorC(FallingBlock.GetPosX()), FloorC(FallingBlock.GetPosY()), FloorC(FallingBlock.GetPosZ())) == a_BlockPos))
			{
				Found = true;
				return true;
			}
			return false;
		}
	);
	if (Found)
	{
		return true;
	}

	a_World.ForEachEntityInBox(cBoundingBox(a_BlockPos, 1, 1), [&Found, a_BlockPos](cEntity & a_Entity)
		{
			if (!a_Entity.IsFallingBlock())
			{
				return false;
			}

			const auto & FallingBlock = static_cast<const cFallingBlock &>(a_Entity);
			if (FallingBlock.IsStatic() && (Vector3i(FloorC(FallingBlock.GetPosX()), FloorC(FallingBlock.GetPosY()), FloorC(FallingBlock.GetPosZ())) == a_BlockPos))
			{
				Found = true;
				return true;
			}
			return false;
		}
	);
	return Found;
}



bool cFallingBlock::DestroyStaticAt(cWorld & a_World, const Vector3i a_BlockPos)
{
	cFallingBlock * Found = nullptr;
	a_World.ForEachPendingEntity([&Found, a_BlockPos](cEntity & a_Entity)
		{
			if (!a_Entity.IsFallingBlock())
			{
				return false;
			}

			auto & FallingBlock = static_cast<cFallingBlock &>(a_Entity);
			if (FallingBlock.IsStatic() && (Vector3i(FloorC(FallingBlock.GetPosX()), FloorC(FallingBlock.GetPosY()), FloorC(FallingBlock.GetPosZ())) == a_BlockPos))
			{
				Found = &FallingBlock;
				return true;
			}
			return false;
		}
	);
	if (Found != nullptr)
	{
		return (a_World.RemoveEntity(*Found) != nullptr);
	}

	a_World.ForEachEntityInBox(cBoundingBox(a_BlockPos, 1, 1), [&Found, a_BlockPos](cEntity & a_Entity)
		{
			if (!a_Entity.IsFallingBlock())
			{
				return false;
			}

			auto & FallingBlock = static_cast<cFallingBlock &>(a_Entity);
			if (FallingBlock.IsStatic() && (Vector3i(FloorC(FallingBlock.GetPosX()), FloorC(FallingBlock.GetPosY()), FloorC(FallingBlock.GetPosZ())) == a_BlockPos))
			{
				Found = &FallingBlock;
				return true;
			}
			return false;
		}
	);

	if (Found == nullptr)
	{
		return false;
	}

	Found->Destroy();
	return true;
}



bool cFallingBlock::PlaceStaticBlocks(cWorld & a_World, const std::initializer_list<sStaticBlock> a_Blocks)
{
	for (const auto & Block: a_Blocks)
	{
		if (!cChunkDef::IsValidHeight(Block.Position) || HasStaticAt(a_World, Block.Position))
		{
			return false;
		}

		for (const auto & OtherBlock: a_Blocks)
		{
			if ((&Block != &OtherBlock) && (Block.Position == OtherBlock.Position))
			{
				return false;
			}
		}
	}

	for (const auto & Block: a_Blocks)
	{
		a_World.SpawnStaticFallingBlock(Block.Position, Block.BlockType, Block.BlockMeta);
	}
	return true;
}





void cFallingBlock::SpawnOn(cClientHandle & a_ClientHandle)
{
	a_ClientHandle.SendSpawnEntity(*this);
}



void cFallingBlock::GetDrops(cItems & a_Drops, cEntity * a_Killer)
{
	UNUSED(a_Killer);
	if (m_IsStatic)
	{
		a_Drops = cBlockHandler::For(m_BlockType).ConvertToPickups(m_BlockMeta);
	}
}





void cFallingBlock::Tick(std::chrono::milliseconds a_Dt, cChunk & a_Chunk)
{
	if (m_IsStatic)
	{
		if (GetHealth() <= 0)
		{
			Destroy();
			return;
		}

		const auto BlockPos = Vector3i(FloorC(GetPosX()), FloorC(GetPosY()), FloorC(GetPosZ()));
		if (!cChunkDef::IsValidHeight(BlockPos))
		{
			Destroy();
			return;
		}

		if (m_World->GetBlock(BlockPos) == E_BLOCK_AIR)
		{
			m_World->SetBlock(BlockPos, m_BlockType, m_BlockMeta);
			Destroy();
		}
		return;
	}

	// GetWorld()->BroadcastTeleportEntity(*this);  // Test position

	int BlockX = POSX_TOINT;
	int BlockY = static_cast<int>(GetPosY() - 0.5);
	int BlockZ = POSZ_TOINT;

	if (BlockY < 0)
	{
		// Fallen out of this world, just continue falling until out of sight, then destroy:
		if (BlockY < VOID_BOUNDARY)
		{
			Destroy();
		}
		return;
	}

	if (BlockY >= cChunkDef::Height)
	{
		// Above the world, just wait for it to fall back down
		return;
	}

	BLOCKTYPE BlockBelow = a_Chunk.GetBlock(BlockX - a_Chunk.GetPosX() * cChunkDef::Width, BlockY, BlockZ - a_Chunk.GetPosZ() * cChunkDef::Width);
	NIBBLETYPE BelowMeta = a_Chunk.GetMeta(BlockX - a_Chunk.GetPosX() * cChunkDef::Width, BlockY, BlockZ - a_Chunk.GetPosZ() * cChunkDef::Width);
	if (cSandSimulator::DoesBreakFallingThrough(BlockBelow, BelowMeta))
	{
		// Fallen onto a block that breaks this into pickups (e. g. half-slab)
		// Must finish the fall with coords one below the block:
		cSandSimulator::FinishFalling(m_World, BlockX, BlockY, BlockZ, m_BlockType, m_BlockMeta);
		Destroy();
		return;
	}
	else if (!cSandSimulator::CanContinueFallThrough(BlockBelow))
	{
		// Fallen onto a solid block
		/*
		FLOGD(
			"Sand: Checked below at {0} (rel {1}), it's {2}, finishing the fall.",
			Vector3i{BlockX, BlockY, BlockZ},
			cChunkDef::AbsoluteToRelative({BlockX, BlockY, BlockZ}, {a_Chunk.GetPosX(), a_Chunk.GetPosZ()}),
			ItemTypeToString(BlockBelow)
		);
		*/

		if (BlockY < cChunkDef::Height - 1)
		{
			cSandSimulator::FinishFalling(m_World, BlockX, BlockY + 1, BlockZ, m_BlockType, m_BlockMeta);
		}
		Destroy();
		return;
	}
	else if ((m_BlockType == E_BLOCK_CONCRETE_POWDER) && IsBlockWater(BlockBelow))
	{
		// Concrete powder falling into water solidifies on the first water it touches
		cSandSimulator::FinishFalling(m_World, BlockX, BlockY, BlockZ, E_BLOCK_CONCRETE, m_BlockMeta);
		Destroy();
		return;
	}

	float MilliDt = a_Dt.count() * 0.001f;
	AddSpeedY(MilliDt * -9.8f);
	AddPosition(GetSpeed() * MilliDt);

	// If not static (one billionth precision) broadcast movement
	if ((fabs(GetSpeedX()) > std::numeric_limits<double>::epsilon()) || (fabs(GetSpeedZ()) > std::numeric_limits<double>::epsilon()))
	{
		BroadcastMovementUpdate();
	}
}
