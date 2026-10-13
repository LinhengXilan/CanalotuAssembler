/**
 * @file Operand.h
 * @author LinhengXilan
 * @version 0.0.0.9
 * @date 2026-10-13
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

struct Operand
{
	OperandType type = OperandType::None;
	Register reg = Register::None;
	Word immediate = 0;
	Word memory = 0;
	Register memoryBase = Register::None;
	Register memoryIndex = Register::None;
	Word displacement = 0;
	bool hasDisplacement = false;
};

std::string GetOperandTypeName(const OperandType& type);

#endif