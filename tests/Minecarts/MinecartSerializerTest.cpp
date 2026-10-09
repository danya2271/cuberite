#include "Globals.h"
#include "WorldStorage/MinecartSerializer.h"
#include "../TestHelpers.h"





static void TestDefaults()
{
	cFastNBTWriter Writer;
	Writer.Finish();
	cParsedNBT Parsed(Writer.GetResult());
	TEST_TRUE(Parsed.IsValid());
	const auto Furnace = MinecartSerializer::ReadFurnace(Parsed, 0);
	TEST_EQUAL(Furnace.Fuel, 0);
	TEST_EQUAL(Furnace.PushX, 0);
	TEST_EQUAL(Furnace.PushZ, 0);
	const auto Hopper = MinecartSerializer::ReadHopper(Parsed, 0);
	TEST_TRUE(Hopper.Enabled);
	TEST_EQUAL(Hopper.TransferCooldown, 0);
	TEST_EQUAL(MinecartSerializer::ReadFuse(Parsed, 0), -1);
}





static void TestRoundTrip()
{
	for (const auto Fuel : { 0, 3600, 32000 })
	{
		for (const auto Fuse : { -1, 0, 1, 80 })
		{
			for (const auto Enabled : { false, true })
			{
				cFastNBTWriter Writer;
				MinecartSerializer::WriteFurnace(Writer, { Fuel, -1.25, 0.75 });
				MinecartSerializer::WriteHopper(Writer, { 4, Enabled });
				MinecartSerializer::WriteFuse(Writer, Fuse);
				Writer.Finish();
				cParsedNBT Parsed(Writer.GetResult());
				TEST_TRUE(Parsed.IsValid());
				const auto Furnace = MinecartSerializer::ReadFurnace(Parsed, 0);
				TEST_EQUAL(Furnace.Fuel, Fuel);
				TEST_EQUAL(Furnace.PushX, -1.25);
				TEST_EQUAL(Furnace.PushZ, 0.75);
				const auto Hopper = MinecartSerializer::ReadHopper(Parsed, 0);
				TEST_EQUAL(Hopper.Enabled, Enabled);
				TEST_EQUAL(Hopper.TransferCooldown, 4);
				TEST_EQUAL(MinecartSerializer::ReadFuse(Parsed, 0), Fuse);
			}
		}
	}
}





static void TestInvalidTags()
{
	cFastNBTWriter Writer;
	Writer.AddInt("Fuel", 100);
	Writer.AddString("PushX", "invalid");
	Writer.AddDouble("PushZ", std::numeric_limits<double>::infinity());
	Writer.AddString("Enabled", "invalid");
	Writer.AddInt("TransferCooldown", -100);
	Writer.AddInt("TNTFuse", -100);
	Writer.Finish();
	cParsedNBT Parsed(Writer.GetResult());
	TEST_TRUE(Parsed.IsValid());
	const auto Furnace = MinecartSerializer::ReadFurnace(Parsed, 0);
	TEST_EQUAL(Furnace.Fuel, 0);
	TEST_EQUAL(Furnace.PushX, 0);
	TEST_EQUAL(Furnace.PushZ, 0);
	const auto Hopper = MinecartSerializer::ReadHopper(Parsed, 0);
	TEST_TRUE(Hopper.Enabled);
	TEST_EQUAL(Hopper.TransferCooldown, 0);
	TEST_EQUAL(MinecartSerializer::ReadFuse(Parsed, 0), -1);

	cFastNBTWriter NegativeFuel;
	NegativeFuel.AddShort("Fuel", -1);
	NegativeFuel.Finish();
	cParsedNBT NegativeParsed(NegativeFuel.GetResult());
	TEST_EQUAL(MinecartSerializer::ReadFurnace(NegativeParsed, 0).Fuel, 0);

	cFastNBTWriter Bounds;
	MinecartSerializer::WriteFurnace(Bounds, { 99999, std::numeric_limits<double>::quiet_NaN(), -2 });
	MinecartSerializer::WriteHopper(Bounds, { -1, false });
	MinecartSerializer::WriteFuse(Bounds, -999);
	Bounds.Finish();
	cParsedNBT BoundsParsed(Bounds.GetResult());
	TEST_EQUAL(MinecartSerializer::ReadFurnace(BoundsParsed, 0).Fuel, 32000);
	TEST_EQUAL(MinecartSerializer::ReadFurnace(BoundsParsed, 0).PushX, 0);
	TEST_EQUAL(MinecartSerializer::ReadHopper(BoundsParsed, 0).TransferCooldown, 0);
	TEST_EQUAL(MinecartSerializer::ReadFuse(BoundsParsed, 0), -1);
}





IMPLEMENT_TEST_MAIN("MinecartSerializer", TestDefaults(); TestRoundTrip(); TestInvalidTags();)
