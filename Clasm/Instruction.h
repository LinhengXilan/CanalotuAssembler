/**
 * @file Instruction.h
 * @author LinhengXilan
 * @version 0.0.0.8
 * @date 2026-10-3
 */

#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include <Types.h>

namespace OperandCategory
{
	constexpr uint32 None = 0;
	constexpr uint32 Reg8 = BIT(0); // 8 位通用寄存器
	constexpr uint32 Reg16 = BIT(1); // 16 位通用寄存器
	constexpr uint32 Sreg = BIT(2); // 段寄存器
	constexpr uint32 Memory = BIT(3); // 内存
	constexpr uint32 Immediate = BIT(4); // 立即数
	constexpr uint32 Relative = BIT(5); // 标签
	constexpr uint32 DirectMemory = BIT(6); // 形如[0x1234]的直接地址
	constexpr uint32 Reg8OrMemory = Reg8 | Memory;
	constexpr uint32 Reg16OrMemory = Reg16 | Memory;
} // namespace OperandCategory

namespace EncodingFlag
{
	constexpr uint32 None = 0;
	constexpr uint32 ModRM = BIT(0);
	constexpr uint32 RegFromOperand0 = BIT(1);
	constexpr uint32 RegFromOperand1 = BIT(2);
	constexpr uint32 FixedReg = BIT(3);
	constexpr uint32 OpcodePlusReg = BIT(4);
	constexpr uint32 SregShift = BIT(5);
	constexpr uint32 Immediate8 = BIT(6);
	constexpr uint32 Immediate16 = BIT(7);
	constexpr uint32 Relative8 = BIT(8);
	constexpr uint32 Relative16 = BIT(9);
} // namespace EncodingFlag

struct Instruction
{
	const char* mnemonic;
	uint32 operand0 = OperandCategory::None;
	uint32 operand1 = OperandCategory::None;
	uint8 opcode;
	uint32 flags = EncodingFlag::None;
	uint8 fixedReg = 0;
};

constexpr Instruction InstructionTable[] = {
	{ "mov", OperandCategory::Reg8, OperandCategory::Reg8OrMemory, 0x8A, EncodingFlag::ModRM | EncodingFlag::RegFromOperand0 },
	{ "mov", OperandCategory::Reg8, OperandCategory::Immediate, 0xB0, EncodingFlag::OpcodePlusReg | EncodingFlag::Immediate8 },
	{ "mov", OperandCategory::Reg16, OperandCategory::Reg16OrMemory, 0x8B, EncodingFlag::ModRM | EncodingFlag::RegFromOperand0 },
	{ "mov", OperandCategory::Reg16, OperandCategory::Immediate, 0xB8, EncodingFlag::OpcodePlusReg | EncodingFlag::Immediate16 },
	{ "mov", OperandCategory::Reg16OrMemory, OperandCategory::Sreg, 0x8C, EncodingFlag::ModRM | EncodingFlag::RegFromOperand1 },
	{ "mov", OperandCategory::Sreg, OperandCategory::Reg16OrMemory, 0x8E, EncodingFlag::ModRM | EncodingFlag::RegFromOperand0 },
	{ "mov", OperandCategory::Reg8OrMemory, OperandCategory::Reg8, 0x88, EncodingFlag::ModRM | EncodingFlag::RegFromOperand1 },
	{ "mov", OperandCategory::Memory, OperandCategory::Reg16, 0x89, EncodingFlag::ModRM | EncodingFlag::RegFromOperand1 },
	{ "mov", OperandCategory::Memory, OperandCategory::Immediate, 0xC6, EncodingFlag::ModRM | EncodingFlag::FixedReg | EncodingFlag::Immediate8 },
	{ "mov", OperandCategory::Memory, OperandCategory::Immediate, 0xC7, EncodingFlag::ModRM | EncodingFlag::FixedReg | EncodingFlag::Immediate16 },

};

#endif