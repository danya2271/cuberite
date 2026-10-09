#include "Globals.h"
#include "MinecartSerializer.h"





MinecartSerializer::FurnaceState MinecartSerializer::ReadFurnace(const cParsedNBT & a_NBT, int a_Tag)
{
	FurnaceState State;
	const auto Fuel = a_NBT.FindChildByName(a_Tag, "Fuel");
	if ((Fuel >= 0) && (a_NBT.GetType(Fuel) == TAG_Short))
	{
		State.Fuel = Clamp<int>(a_NBT.GetShort(Fuel), 0, 32000);
	}
	const auto PushX = a_NBT.FindChildByName(a_Tag, "PushX");
	if ((PushX >= 0) && (a_NBT.GetType(PushX) == TAG_Double))
	{
		const auto Push = a_NBT.GetDouble(PushX);
		State.PushX = std::isfinite(Push) ? Push : 0;
	}
	const auto PushZ = a_NBT.FindChildByName(a_Tag, "PushZ");
	if ((PushZ >= 0) && (a_NBT.GetType(PushZ) == TAG_Double))
	{
		const auto Push = a_NBT.GetDouble(PushZ);
		State.PushZ = std::isfinite(Push) ? Push : 0;
	}
	return State;
}





MinecartSerializer::HopperState MinecartSerializer::ReadHopper(const cParsedNBT & a_NBT, int a_Tag)
{
	HopperState State;
	const auto Cooldown = a_NBT.FindChildByName(a_Tag, "TransferCooldown");
	if ((Cooldown >= 0) && (a_NBT.GetType(Cooldown) == TAG_Int))
	{
		State.TransferCooldown = std::max(0, a_NBT.GetInt(Cooldown));
	}
	const auto Enabled = a_NBT.FindChildByName(a_Tag, "Enabled");
	if ((Enabled >= 0) && (a_NBT.GetType(Enabled) == TAG_Byte))
	{
		State.Enabled = (a_NBT.GetByte(Enabled) != 0);
	}
	return State;
}





int MinecartSerializer::ReadFuse(const cParsedNBT & a_NBT, int a_Tag)
{
	const auto Fuse = a_NBT.FindChildByName(a_Tag, "TNTFuse");
	if ((Fuse < 0) || (a_NBT.GetType(Fuse) != TAG_Int))
	{
		return -1;
	}
	return std::max(-1, a_NBT.GetInt(Fuse));
}





void MinecartSerializer::WriteFurnace(cFastNBTWriter & a_Writer, FurnaceState a_State)
{
	a_Writer.AddShort("Fuel", static_cast<Int16>(Clamp(a_State.Fuel, 0, 32000)));
	a_Writer.AddDouble("PushX", std::isfinite(a_State.PushX) ? a_State.PushX : 0);
	a_Writer.AddDouble("PushZ", std::isfinite(a_State.PushZ) ? a_State.PushZ : 0);
}





void MinecartSerializer::WriteHopper(cFastNBTWriter & a_Writer, HopperState a_State)
{
	a_Writer.AddInt("TransferCooldown", std::max(0, a_State.TransferCooldown));
	a_Writer.AddByte("Enabled", a_State.Enabled ? 1 : 0);
}





void MinecartSerializer::WriteFuse(cFastNBTWriter & a_Writer, int a_Fuse)
{
	a_Writer.AddInt("TNTFuse", std::max(-1, a_Fuse));
}
