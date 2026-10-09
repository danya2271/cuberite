#pragma once

#include "FastNBT.h"





namespace MinecartSerializer
{
	struct FurnaceState
	{
		int Fuel = 0;
		double PushX = 0;
		double PushZ = 0;
	};

	struct HopperState
	{
		int TransferCooldown = 0;
		bool Enabled = true;
	};

	FurnaceState ReadFurnace(const cParsedNBT & a_NBT, int a_Tag);
	HopperState ReadHopper(const cParsedNBT & a_NBT, int a_Tag);
	int ReadFuse(const cParsedNBT & a_NBT, int a_Tag);
	void WriteFurnace(cFastNBTWriter & a_Writer, FurnaceState a_State);
	void WriteHopper(cFastNBTWriter & a_Writer, HopperState a_State);
	void WriteFuse(cFastNBTWriter & a_Writer, int a_Fuse);
}
