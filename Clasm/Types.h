/**
 * @file Types.h
 * @author LinhengXilan
 * @version 0.0.0.7
 * @date 2026-10-2
 */

#ifndef _TYPES_H_
#define _TYPES_H_

#include <cstdint>
#include <vector>

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
using usize = size_t;

using Byte = uint8;
using Word = uint16;

using Bytes = std::vector<Byte>;

constexpr uint32 BIT(uint8 i)
{
	return 1u << i;
}

#endif