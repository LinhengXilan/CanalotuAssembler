/**
 * @file Operand.h
 * @author LinhengXilan
 * @version 0.0.1.11
 * @date 2026-10-29
 */

#ifndef _OPERAND_H_
#define _OPERAND_H_

#include <string>
#include <Types.h>
#include <Register.h>

enum class OperandType : uint8
{
	None,
	Register,
	Immediate,
	Memory
};

enum class OperandSize
{
	None,
	Byte,
	Word
};

struct Operand
{
	OperandType type = OperandType::None;
	OperandSize size = OperandSize::None;
	Register reg = Register::None;
	Word immediate = 0;
	Register segment = Register::None;
	Register memoryBase = Register::None;
	Register memoryIndex = Register::None;
	Word displacement = 0;
	bool hasDisplacement = false;
};

std::string GetOperandTypeName(const OperandType& type);

#endif