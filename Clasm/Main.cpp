/**
 * @file Main.cpp
 * @author LinhengXilan
 * @version 0.0.0.6
 * @date 2026-9-20
 */

#include <Pch.h>
#include <Types.h>
#include <Register.h>
#include <CommandLine.h>

enum class OperandType : uint8
{
	Register,
	Immediate,
	Memory
};

struct Operand
{
	OperandType type;
	union {
		Register reg;
		Word immediate = 0;
	};
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
	for (auto& ch : line)
	{
		if (ch == ';')
		{
			break;
		}
		if (ch == ' ' || ch == ',' || ch == '\t')
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
	auto it = RegisterMap.find(token);
	if (it != RegisterMap.end())
	{
		operand.type = OperandType::Register;
		operand.reg = it->second;
		return operand;
	}
	Word immediate;
	if (ParseImmediate(token, immediate))
	{
		operand.type = OperandType::Immediate;
		operand.immediate = immediate;
		std::cout << "immediate: " << immediate << std::endl;
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
	Memory = 0b00000000,
	MemoryWith8bitDisplacement = 0b01000000,
	MemoryWith16bitDisplacement = 0b10000000,
	Register = 0b11000000,
};

Byte EncodeModRM(Mod mod, uint8 reg, uint8 rm)
{
	return static_cast<Byte>(mod) | ((reg & 0b111) << 3) | (rm & 0b111);
}

Bytes EncodeInstruction(const std::string& mnemonic, const std::vector<Operand>& operands)
{
	Bytes binary;
	if (mnemonic == "mov")
	{
		if (operands.size() != 2)
		{
			throw std::runtime_error("mov指令需要两个操作数");
		}
		const Operand& destination = operands[0];
		const Operand& source = operands[1];
		if (destination.type == OperandType::Register && source.type == OperandType::Register)
		{
			if (Is8bitRegister(destination.reg) && Is8bitRegister(source.reg))
			{
				EmitByte(binary, 0x8A);
				EmitByte(binary, EncodeModRM(Mod::Register, GetRegisterCode(destination.reg), GetRegisterCode(source.reg)));
			}
			else if (Is16bitRegister(destination.reg) && Is16bitRegister(source.reg))
			{
				EmitByte(binary, 0x8B);
				EmitByte(binary, EncodeModRM(Mod::Register, GetRegisterCode(destination.reg), GetRegisterCode(source.reg)));
			}
			else if (destination.reg != Register::CS && IsSegmentRegister(destination.reg) && Is16bitRegister(source.reg))
			{
				EmitByte(binary, 0x8E);
				EmitByte(binary, EncodeModRM(Mod::Register, GetSegmentRegisterCode(destination.reg), GetRegisterCode(source.reg)));
			}
			else if (Is16bitRegister(destination.reg) && IsSegmentRegister(source.reg))
			{
				EmitByte(binary, 0x8C);
				EmitByte(binary, EncodeModRM(Mod::Register, GetSegmentRegisterCode(source.reg), GetRegisterCode(destination.reg)));
			}
			else
			{
				throw std::runtime_error("mov指令操作数非法");
			}
		}
		else if (destination.type == OperandType::Register && source.type == OperandType::Immediate)
		{
			if (Is8bitRegister(destination.reg))
			{
				EmitByte(binary, 0xB0 | GetRegisterCode(destination.reg));
				EmitByte(binary, static_cast<Byte>(source.immediate));
			}
			else if (Is16bitRegister(destination.reg))
			{
				EmitByte(binary, 0xB8 | GetRegisterCode(destination.reg));
				EmitWord(binary, source.immediate);
			}
			else
			{
				throw std::runtime_error("mov指令操作数非法");
			}
		}
		else
		{
			throw std::runtime_error("mov指令操作数非法");
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
	std::ifstream ifs{option.inputFileName};
	if (!ifs.is_open())
	{
		std::cerr << "错误: 无法打开文件" << option.inputFileName << std::endl;
		return ErrorCode::NoInput;
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

		for (auto& token : tokens)
		{
			std::cout << token << ' ';
		}
		std::cout << std::endl;
		std::string mnemonic = tokens[0];
		tokens.erase(tokens.begin());

		std::vector<Operand> operands;
		for (const auto& token : tokens)
		{
			operands.push_back(ParseOperand(token));
		}

		for (const auto& operand : operands)
		{
			std::cout << GetOperandTypeName(operand.type) << ' ' << (operand.type == OperandType::Register ? GetRegisterName(operand.reg) : std::to_string(operand.immediate));
			std::cout << std::endl;
		}
		binary.push_back(EncodeInstruction(mnemonic, operands));
	}

	std::ofstream ofs{option.outputFileName, std::ios::binary};
	if (ofs.is_open())
	{
		for (const auto& bytes : binary)
		{
			for (const auto& byte : bytes)
			{
				ofs << byte;
			}
		}
	}
	return errorCode;
}