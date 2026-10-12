
#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "Painting.h"
#include "Player.h"
#include "../ClientHandle.h"
#include "../Chunk.h"





cPainting::cPainting(const AString & a_Name, eBlockFace a_Direction, Vector3d a_Pos):
	Super(etPainting, a_Direction, a_Pos),
	m_Name(a_Name)
{
}




UInt32 cPainting::GetPaintingID(void) const
{
	static constexpr const char * PaintingNames[] =
	{
		"Kebab", "Aztec", "Alban", "Aztec2", "Bomb", "Plant", "Wasteland", "Pool",
		"Courbet", "Sea", "Sunset", "Creebet", "Wanderer", "Graham", "Match", "Bust",
		"Stage", "Void", "SkullAndRoses", "Wither", "Fighters", "Skeleton", "DonkeyKong",
		"Pointer", "Pigscene", "BurningSkull"
	};

	const auto Name = (m_Name.rfind("minecraft:", 0) == 0) ? m_Name.substr(10) : m_Name;
	for (size_t Index = 0; Index < ARRAYCOUNT(PaintingNames); ++Index)
	{
		if (NoCaseCompare(Name, PaintingNames[Index]) == 0)
		{
			return static_cast<UInt32>(Index);
		}
	}

	return 0;
}





void cPainting::SpawnOn(cClientHandle & a_Client)
{
	Super::SpawnOn(a_Client);
	a_Client.SendPaintingSpawn(*this);

	m_World->BroadcastSoundEffect("entity.painting.place", GetPosition(), 1, 1);
}





void cPainting::GetDrops(cItems & a_Items, cEntity * a_Killer)
{
	a_Items.emplace_back(E_ITEM_PAINTING);
}





void cPainting::KilledBy(TakeDamageInfo & a_TDI)
{
	Super::KilledBy(a_TDI);

	m_World->BroadcastSoundEffect("entity.painting.break", GetPosition(), 1, 1);
}
