/**
 * @file Main.cpp
 * @author LinhengXilan
 * @version 0.0.0.8
 * @date 2026-10-3
 */

#include <Pch.h>
#include <CommandLine.h>
#include <Instruction.h>
#include <Register.h>
#include <Types.h>

enum class OperandType : uint8
{
	None,
	Register,
	Immediate,
	Memory,

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

/**
 * @brief 将一行汇编拆分成token
 * @param line 一行汇编代码
 * @return tokens
 */
std::vector<std::string> Tokenize(const std::string& line)
{
	std::vector<std::string> tokens;
	std::string token;
	uint8 depth = 0;
	for (auto& ch : line)
	{
		if (ch == ';')
		{
			break;
		}
		if (ch == '[')
		{
			depth++;
		}
		if (ch == ']')
		{
			depth--;
		}
		if (depth == 0 && (ch == ' ' || ch == ',' || ch == '\t'))
		{
			if (!token.empty())
			{
				tokens.push_back(token);
				token.clear();
			}
		}
		else
		{
			token.push_back(ch);
		}
	}
	if (depth != 0)
	{
		std::runtime_error("非法内存操作数 ");
	}
	if (!token.empty())
	{
		tokens.push_back(token);
	}
	return tokens;
}

/**
 * @brief 将字符串转换为数值
 *
 * @param[in] str 字符串
 * @param[out] value 数值
 * @return 是否成功转换
 */
bool ParseImmediate(const std::string& str, Word& value)
{
	if (str.empty())
	{
		value = 0;
		return false;
	}
	if (str.size() == 3 && str[0] == '\'' && str[2] == '\'')
	{
		value = static_cast<Byte>(str[1]);
		return true;
	}
	if (str.size() > 2 && str[0] == '0')
	{
		if (str[1] == 'x')
		{
			std::string substr = str.substr(2);
			try
			{
				value = static_cast<Word>(std::stoi(substr, nullptr, 16));
				return true;
			}
			catch (...)
			{
				value = 0;
				return false;
			}
		}
		if (str[1] >= '1' && str[1] <= '7')
		{
			std::string substr = str.substr(1);
			try
			{
				value = static_cast<Word>(std::stoi(substr, nullptr, 8));
				return true;
			}
			catch (...)
			{
				value = 0;
				return false;
			}
		}
		if (str[1] == 'b')
		{
			std::string substr = str.substr(2);
			try
			{
				value = static_cast<Word>(std::stoi(substr, nullptr, 2));
				return true;
			}
			catch (...)
			{
				value = 0;
				return false;
			}
		}
	}
	try
	{
		value = static_cast<Word>(std::stoi(str));
		return true;
	}
	catch (...)
	{
		value = 0;
		return false;
	}
}

Operand ParseOperand(const std::string& token)
{
	Operand operand;

	if (token.size() > 2 && token.front() == '[' && token.back() == ']')
	{
		std::string addressStr = token.substr(1, token.size() - 2);
		Word address = 0;
		if (ParseImmediate(addressStr, address))
		{
			operand.type = OperandType::Memory;
			operand.memory = address;
			return operand;
		}
		else
		{
			throw std::runtime_error("未知内存操作数: " + token);
		}
	}

	auto it = RegisterMap.find(token);
	if (it != RegisterMap.end())
	{
		operand.type = OperandType::Register;
		operand.reg = it->second;
		return operand;
	}

	if (ParseImmediate(token, operand.immediate))
	{
		operand.type = OperandType::Immediate;
		return operand;
	}

	throw std::runtime_error("未知操作数: " + token);
}

void EmitByte(Bytes& out, Byte value)
{
	out.push_back(value);
}

void EmitWord(Bytes& out, Word value)
{
	out.push_back(static_cast<Byte>(value & 0x00FF));
	out.push_back(static_cast<Byte>(value >> 8 & 0xFF));
}

enum class Mod
{
	Memory = 0b00,
	MemoryWith8bitDisplacement = 0b01,
	MemoryWith16bitDisplacement = 0b10,
	Register = 0b11,
};

Byte EncodeModRM(Mod mod, uint8 reg, uint8 rm)
{
	return static_cast<Byte>(static_cast<uint8>(mod) << 6) | ((reg & 0b111) << 3) | (rm & 0b111);
}

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
		else if(rmOperand->type == OperandType::Memory)
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

int main(int argc, char** argv)
{
	CommandOption option;
	uint8 errorCode = ParseCommand(argc, argv, option);
	if (errorCode != 0)
	{
		return errorCode;
	}
	if (option.inputFileName.empty())
	{
		std::cerr << "无输入文件" << std::endl;
		return ErrorCode::NoInput;
	}
	std::ifstream ifs{ option.inputFileName, std::ios::binary };
	if (!ifs.is_open())
	{
		std::cerr << "错误: 无法打开文件" << option.inputFileName << std::endl;
		return ErrorCode::NoInput;
	}
	// 跳过 UTF-8 BOM
	if (ifs.peek() == 0xEF)
	{
		ifs.get();
		if (ifs.peek() == 0xBB) ifs.get();
		if (ifs.peek() == 0xBF) ifs.get();
	}

	std::string line;
	std::vector<Bytes> binary;
	while (std::getline(ifs, line))
	{
		/* vvv 去除注释 vvv */
		usize comment = line.find(';');
		if (comment != std::string::npos)
		{
			line = line.substr(0, comment);
		}
		/* ^^^ 去除注释 ^^^ */

		/* vvv 去除多余转义字符 vvv */
		usize start = line.find_first_not_of(" \t\r\n");
		if (start == std::string::npos) // 跳过空行
		{
			continue;
		}
		usize end = line.find_last_not_of(" \t\r\n");
		line = line.substr(start, end - start + 1);
		if (line.empty()) // 去除只有空格的行
		{
			continue;
		}
		/* ^^^ 去除多余转义字符 ^^^ */

		std::vector<std::string> tokens = Tokenize(line);
		if (tokens.empty())
		{
			continue;
		}

		std::string mnemonic = tokens[0];
		tokens.erase(tokens.begin());

		std::vector<Operand> operands;
		for (const auto& token : tokens)
		{
			operands.push_back(ParseOperand(token));
		}

		binary.push_back(EncodeInstruction(mnemonic, operands));
	}

	Bytes out;
	for (const auto& bytes : binary)
	{
		for (const auto& byte : bytes)
		{
			out.push_back(byte);
		}
	}

	out.resize(510, 0);
	out.push_back(0x55);
	out.push_back(0xAA);

	std::ofstream ofs{ option.outputFileName, std::ios::binary };
	if (ofs.is_open())
	{
		for (const auto& byte : out)
		{
			ofs << byte;
		}
	}

	return errorCode;
}