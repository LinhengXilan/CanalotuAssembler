/**
 * @file Encode.cpp
 * @author LinhengXilan
 * @version 0.0.0.10
 * @date 2026-10-15
 */

#include <Pch.h>
#include <Encode.h>

bool MatchOperandCategory(uint32 category, const Operand& operand)
{
	switch (operand.type)
	{
	case OperandType::Register:
		if (IsSegmentRegister(operand.reg))
		{
			return (category & OperandCategory::Sreg) != 0;
		}
		else if (Is8bitRegister(operand.reg))
		{
			return (category & OperandCategory::Reg8) != 0;
		}
		else if (Is16bitRegister(operand.reg))
		{
			return (category & OperandCategory::Reg16) != 0;
		}
		return false;
	case OperandType::Immediate:
		return (category & OperandCategory::Immediate) != 0;
	case OperandType::Memory:
		return (category & OperandCategory::Memory) != 0;
	default:
		return false;
	}
}

const Instruction* MatchInstruction(const std::string& mnemonic, const std::vector<Operand>& operands)
{
	for (const auto& instruction : InstructionTable)
	{
		if (mnemonic != instruction.mnemonic)
		{
			continue;
		}
		usize nrWantedOperand = 0;
		if (instruction.operand1 != OperandCategory::None)
		{
			nrWantedOperand = 2;
		}
		else if (instruction.operand0 != OperandCategory::None)
		{
			nrWantedOperand = 1;
		}
		if (nrWantedOperand != operands.size())
		{
			continue;
		}
		if (nrWantedOperand > 0 && !MatchOperandCategory(instruction.operand0, operands[0]))
		{
			continue;
		}
		if (nrWantedOperand > 1 && !MatchOperandCategory(instruction.operand1, operands[1]))
		{
			continue;
		}
		return &instruction;
	}
	return nullptr;
}

void EmitByte(Bytes& out, Byte data)
{
	out.push_back(data);
}

void EmitWord(Bytes& out, Word data)
{
	out.push_back(static_cast<Byte>(data & 0x00FF));
	out.push_back(static_cast<Byte>(data >> 8 & 0xFF));
}

Byte EncodeModRM(Mod mod, uint8 reg, uint8 rm)
{
	return static_cast<Byte>(static_cast<uint8>(mod) << 6) | ((reg & 0b111) << 3) | (rm & 0b111);
}

Bytes EncodeInstruction(const std::string& mnemonic, const std::vector<Operand>& operands)
{
	Bytes binary;

	/* vvv opcode vvv */
	const Instruction* instruction = MatchInstruction(mnemonic, operands);
	Byte opcode = instruction->opcode;
	if (instruction->flags & EncodingFlag::OpcodePlusReg)
	{
		opcode |= GetGeneralRegisterCode(operands[0].reg);
	}
	else if (instruction->flags & EncodingFlag::SregShift)
	{
		opcode |= static_cast<Byte>(GetSegmentRegisterCode(operands[0].reg) << 3);
	}

	EmitByte(binary, opcode);

	/* vvv ModRM vvv */
	if (instruction->flags & EncodingFlag::ModRM)
	{
		uint8 regField = 0;
		const Operand* rmOperand = nullptr;

		if (instruction->flags & EncodingFlag::RegFromOperand0)
		{
			regField = GetAnyRegisterCode(operands[0].reg);
			rmOperand = &operands[1];
		}
		else if (instruction->flags & EncodingFlag::RegFromOperand1)
		{
			regField = GetAnyRegisterCode(operands[1].reg);
			rmOperand = &operands[0];
		}
		else if (instruction->flags & EncodingFlag::FixedReg)
		{
			regField = instruction->fixedReg;
			for (const auto& operand : operands)
			{
				if (operand.type != OperandType::Immediate)
				{
					rmOperand = &operand;
					break;
				}
			}
		}

		if (rmOperand->type == OperandType::Register)
		{
			EmitByte(binary, EncodeModRM(Mod::Register, regField, GetGeneralRegisterCode(rmOperand->reg)));
		}
		else if (rmOperand->type == OperandType::Memory)
		{
			EmitByte(binary, EncodeModRM(Mod::Memory, regField, 0b110));
			EmitWord(binary, rmOperand->memory);
		}
		else
		{
			// TODO: 报错
			throw std::runtime_error("不支持的操作数类型");
		}
		return binary;
	}

	/* vvv 立即数 vvv */
	if (instruction->flags & (EncodingFlag::Immediate8 | EncodingFlag::Immediate16))
	{
		const Operand* immOperand = nullptr;
		for (const auto& operand : operands)
		{
			if (operand.type == OperandType::Immediate)
			{
				immOperand = &operand;
				break;
			}
		}
		if (immOperand == nullptr)
		{
			throw std::runtime_error("缺少立即数操作数");
		}

		if (instruction->flags & EncodingFlag::Immediate8)
		{
			EmitByte(binary, static_cast<Byte>(immOperand->immediate));
		}
		else
		{
			EmitWord(binary, immOperand->immediate);
		}
	}

	return binary;
}