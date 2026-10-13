/**
 * @file Operand.cpp
 * @author LinhengXilan
 * @version 0.0.0.9
 * @date 2026-10-13
 */

#include <Pch.h>
#include <Operand.h>

std::string GetOperandTypeName(const OperandType& type)
{
	switch (type)
	{
	case OperandType::Register:
		return "Register";
	case OperandType::Immediate:
		return "Immediate";
	case OperandType::Memory:
		return "Memory";
	default:
		return "Unknown OperandType";
	}
}