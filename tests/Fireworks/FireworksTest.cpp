#include "Globals.h"
#include "WorldStorage/FireworksSerializer.h"
#include "WorldStorage/FastNBT.h"
#include "../TestHelpers.h"

static void RoundTrip(const cFireworkItem & a_Item, ENUM_ITEM_TYPE a_Type)
{
	cFastNBTWriter Writer;
	cFireworkItem::WriteToNBTCompound(a_Item, Writer, a_Type);
	Writer.Finish();
	cParsedNBT Parsed(Writer.GetResult());
	TEST_TRUE(Parsed.IsValid());
	const auto Tag = Parsed.FindChildByName(0, a_Type == E_ITEM_FIREWORK_ROCKET ? "Fireworks" : "Explosion");
	cFireworkItem Restored;
	Restored.m_Colours = {123};
	Restored.m_FadeColours = {456};
	cFireworkItem::ParseFromNBT(Restored, Parsed, Tag, a_Type);
	TEST_TRUE(a_Item.IsEqualTo(Restored));
	cFireworkItem::ParseFromNBT(Restored, Parsed, Tag, a_Type);
	TEST_TRUE(a_Item.IsEqualTo(Restored));
}

static void TestFireworks()
{
	cFireworkItem Plain;
	RoundTrip(Plain, E_ITEM_FIREWORK_ROCKET);
	Plain.m_FlightTimeInTicks = 60;
	RoundTrip(Plain, E_ITEM_FIREWORK_ROCKET);
	Plain.m_HasExplosion = true;
	RoundTrip(Plain, E_ITEM_FIREWORK_ROCKET);
	for (int Count = 1; Count <= 9; ++Count)
	{
		cFireworkItem Star;
		Star.m_Type = 4;
		Star.m_HasTrail = true;
		Star.m_HasFlicker = true;
		for (int Index = 0; Index < Count; ++Index)
		{
			Star.m_Colours.push_back(0x123456 + Index);
			Star.m_FadeColours.push_back(0x654321 + Index);
		}
		RoundTrip(Star, E_ITEM_FIREWORK_STAR);
		cFireworkItem Rocket = Star;
		Rocket.m_FlightTimeInTicks = 40;
		for (int Index = 0; Index < 6; ++Index)
		{
			cFireworkExplosion Explosion = Star;
			Explosion.m_Type = static_cast<NIBBLETYPE>(Index % 5);
			Rocket.m_AdditionalExplosions.push_back(Explosion);
		}
		RoundTrip(Rocket, E_ITEM_FIREWORK_ROCKET);
		cFireworkItem Copied;
		Copied.CopyFrom(Rocket);
		TEST_TRUE(Copied.IsEqualTo(Rocket));
		Copied.m_AdditionalExplosions.back().m_Type = 2;
		TEST_FALSE(Copied.IsEqualTo(Rocket));
		Copied.EmptyData();
		TEST_TRUE(Copied.IsEqualTo(cFireworkItem()));
	}
	cFastNBTWriter Writer;
	Writer.BeginCompound("Fireworks");
	Writer.AddByte("Flight", -1);
	Writer.BeginList("Explosions", TAG_Int);
	Writer.AddInt("", 123);
	Writer.EndList();
	Writer.EndCompound();
	Writer.Finish();
	cParsedNBT Parsed(Writer.GetResult());
	cFireworkItem Invalid;
	cFireworkItem::ParseFromNBT(Invalid, Parsed, Parsed.FindChildByName(0, "Fireworks"), E_ITEM_FIREWORK_ROCKET);
	TEST_TRUE(Invalid.IsEqualTo(cFireworkItem()));
}

IMPLEMENT_TEST_MAIN("Fireworks", TestFireworks();)
