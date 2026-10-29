/**
 * @file Encode.cpp
 * @author LinhengXilan
 * @version 0.0.1.11
 * @date 2026-10-29
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
		else if (IsGeneralRegister(operand.reg))
		{
			return (category & OperandCategory::Reg) != 0;
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
		if (nrWantedOperand > 0 && operands[0].size != instruction.size)
		{
			if (operands[0].size != OperandSize::None)
			{
				continue;
			}
		}
		if (nrWantedOperand > 1 && operands[1].size != instruction.size)
		{
			if (operands[1].size != OperandSize::None)
			{
				continue;
			}
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

Byte EncodeModRM(uint8 mod, uint8 reg, uint8 rm)
{
	return (mod) << 6 | ((reg & 0b111) << 3) | (rm & 0b111);
}

Byte EncodeMemoryModRM(uint8 reg, const Operand& operand, OperandSize& size)
{
	if (operand.type != OperandType::Memory)
	{
		throw std::runtime_error("汇编器内部错误");
	}
	uint8 mod = 0;
	int16 disp = static_cast<int16>(operand.displacement);
	bool isDisp8 = (disp >= -128 && disp <= 127);
	if (operand.hasDisplacement && (operand.memoryBase != Register::None || operand.memoryIndex != Register::None))
	{
		
		if (isDisp8)
		{
			mod = 0b01;
			size = OperandSize::Byte;
		}
		else
		{
			mod = 0b10;
			size = OperandSize::Word;
		}
	}
	else
	{
		mod = 0b00;
	}

	uint8 rm = 0;
	if (operand.memoryBase == Register::BX)
	{
		if (operand.memoryIndex == Register::SI)
		{
			rm = 0b000;
		}
		else if (operand.memoryIndex == Register::DI)
		{
			rm = 0b001;
		}
		else
		{
			rm = 0b111;
		}
	}
	else if (operand.memoryBase == Register::BP)
	{
		if (operand.memoryIndex == Register::SI)
		{
			rm = 0b010;
		}
		else if (operand.memoryIndex == Register::DI)
		{
			rm = 0b011;
		}
		else
		{
			if (!operand.hasDisplacement)
			{
				mod = 0b01;
				size = OperandSize::Byte;
			}
			rm = 0b110;
		}
	}
	else
	{
		if (operand.memoryIndex == Register::SI)
		{
			rm = 0b100;
		}
		else if (operand.memoryIndex == Register::DI)
		{
			rm = 0b101;
		}
		else
		{
			rm = 0b110;

			if (isDisp8)
			{
				size = OperandSize::Byte;
			}
			else
			{
				size = OperandSize::Word;
			}
		}
	}
	if (disp == 0 && rm != 6) // TODO: rm 得先算，或者先算 rm 再定 mod
	{
		mod = 0b00;
		size = OperandSize::None;
	}
	return EncodeModRM(mod, reg, rm);
}

Bytes EncodeInstruction(const std::string& mnemonic, const std::vector<Operand>& operands)
{
	Bytes binary;

	/* vvv opcode vvv */
	const Instruction* instruction = MatchInstruction(mnemonic, operands);
	if (instruction == nullptr)
	{
		throw std::runtime_error("无匹配指令");
	}
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

		if (rmOperand == nullptr)
		{
			throw std::runtime_error("内部错误：ModRM 缺少 r/m 操作数");
		}
		if (rmOperand->type == OperandType::Register)
		{
			EmitByte(binary, EncodeModRM(Mod::Register, regField, GetGeneralRegisterCode(rmOperand->reg)));
		}
		else if (rmOperand->type == OperandType::Memory)
		{
			OperandSize size = OperandSize::None;
			EmitByte(binary, EncodeMemoryModRM(regField, *rmOperand, size));
			if (size == OperandSize::Byte)
			{
				EmitByte(binary, static_cast<Byte>(rmOperand->displacement));
			}
			else if (size == OperandSize::Word)
			{
				EmitWord(binary, rmOperand->displacement);
			}
		}
		else
		{
			// TODO: 报错
			throw std::runtime_error("不支持的操作数类型");
		}
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