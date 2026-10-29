/**
 * @file Instruction.h
 * @author LinhengXilan
 * @version 0.0.1.11
 * @date 2026-10-29
 */

#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include <Types.h>
#include <Operand.h>
#include <Utils.h>

namespace OperandCategory
{
	constexpr uint32 None = 0;
	constexpr uint32 Reg = BIT(0); // 通用寄存器
	constexpr uint32 Sreg = BIT(1); // 段寄存器
	constexpr uint32 Memory = BIT(2); // 内存
	constexpr uint32 Immediate = BIT(3); // 立即数
	constexpr uint32 Relative = BIT(4); // 标签
	constexpr uint32 RegOrMemory = Reg | Memory;
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
	OperandSize size = OperandSize::None;
	uint32 operand0 = OperandCategory::None;
	uint32 operand1 = OperandCategory::None;
	uint8 opcode = 0;
	uint32 flags = EncodingFlag::None;
	uint8 fixedReg = 0;
};

constexpr Instruction InstructionTable[] = {
	{ "mov", OperandSize::Byte, OperandCategory::Reg, OperandCategory::RegOrMemory, 0x8A, EncodingFlag::ModRM | EncodingFlag::RegFromOperand0 },
	{ "mov", OperandSize::Word, OperandCategory::Reg, OperandCategory::RegOrMemory, 0x8B, EncodingFlag::ModRM | EncodingFlag::RegFromOperand0 },
	{ "mov", OperandSize::Byte, OperandCategory::Memory, OperandCategory::Reg, 0x88, EncodingFlag::ModRM | EncodingFlag::RegFromOperand1 },
	{ "mov", OperandSize::Word, OperandCategory::Memory, OperandCategory::Reg, 0x89, EncodingFlag::ModRM | EncodingFlag::RegFromOperand1 },
	{ "mov", OperandSize::Byte, OperandCategory::Reg, OperandCategory::Immediate, 0xB0, EncodingFlag::OpcodePlusReg | EncodingFlag::Immediate8 },
	{ "mov", OperandSize::Word, OperandCategory::Reg, OperandCategory::Immediate, 0xB8, EncodingFlag::OpcodePlusReg | EncodingFlag::Immediate16 },
	{ "mov", OperandSize::Word, OperandCategory::RegOrMemory, OperandCategory::Sreg, 0x8C, EncodingFlag::ModRM | EncodingFlag::RegFromOperand1 },
	{ "mov", OperandSize::Word, OperandCategory::Sreg, OperandCategory::RegOrMemory, 0x8E, EncodingFlag::ModRM | EncodingFlag::RegFromOperand0 },
	{ "mov", OperandSize::Byte, OperandCategory::Memory, OperandCategory::Immediate, 0xC6, EncodingFlag::ModRM | EncodingFlag::FixedReg | EncodingFlag::Immediate8 },
	{ "mov", OperandSize::Word, OperandCategory::Memory, OperandCategory::Immediate, 0xC7, EncodingFlag::ModRM | EncodingFlag::FixedReg | EncodingFlag::Immediate16 }
};

#endif