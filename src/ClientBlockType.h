#pragma once

#include "BlockType.h"

constexpr BLOCKTYPE ClientBlockType(BLOCKTYPE a_BlockType)
{
	return (a_BlockType == E_BLOCK_SOOT) ? static_cast<BLOCKTYPE>(E_BLOCK_SNOW) : a_BlockType;
}
