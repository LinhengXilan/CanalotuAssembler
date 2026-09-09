/**
 * @file Main.cpp
 * @author LinhengXilan
 * @version 0.0.0.3
 * @date 2026-9-9
 */

#include <Types.h>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace
{
	void PrintHelp()
	{
		std::cout << "用法: clasm 输入文件名 [选项]\n"
				  << "示例: clasm Example.asm -o Example.bin\n"
				  << "-h 显示此帮助信息\n"
				  << "-o <文件名> 指定输出文件名\n"
				  << "-v 显示版本信息\n";
	}

	void PrintVersion()
	{
		std::cout << "Version 0.0.0 Build3" << std::endl;
	}
} // namespace

namespace ErrorCode
{
	constexpr uint8 InvalidOption = 1;
	constexpr uint8 NoInput = 2;
} // namespace ErrorCode

std::string inputFileName;
std::string outputFileName;

static uint8 ParseCommand(int argc, char** argv)
{
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			case 'h':
				if (argv[i][2] != '\0')
				{
					std::cerr << "未知选项: " << argv[i] << std::endl;
					return ErrorCode::InvalidOption;
				}
				PrintHelp();
				break;
			case 'o':
				outputFileName = argv[++i];
				break;
			case 'v':
				if (argv[i][2] != '\0')
				{
					std::cerr << "未知选项: " << argv[i] << std::endl;
					return ErrorCode::InvalidOption;
				}
				PrintVersion();
				break;
			default:
				std::cerr << "未知选项: " << argv[i] << std::endl;
				return ErrorCode::InvalidOption;
			}
		}
		else
		{
			inputFileName = argv[i];
		}
	}
	return 0;
}

enum class Register : uint8
{
	None,
	AL, BL, CL, DL, AH, BH, CH, DH,
	AX,	BX,	CX,	DX,	BP,	SP,	SI,	DI,
	CS, DS, SS, ES
};

std::map<std::string, Register> RegisterMap = {{"al", Register::AL}, {"bl", Register::BL}, {"cl", Register::CL}, {"dl", Register::DL}, {"ah", Register::AH}, {"bh", Register::BH}, {"ch", Register::CH}, {"dh", Register::DH}, {"ax", Register::AX}, {"bx", Register::BX},
												{"cx", Register::CX}, {"dx", Register::DX}, {"bp", Register::BP}, {"sp", Register::SP}, {"si", Register::SI}, {"di", Register::DI}, {"cs", Register::CS}, {"ds", Register::DS}, {"ss", Register::SS}, {"es", Register::ES}};

std::string GetRegisterName(Register reg)
{
	switch (reg)
	{
	case Register::AL:
		return "al";
	case Register::BL:
		return "bl";
	case Register::CL:
		return "cl";
	case Register::DL:
		return "dl";
	case Register::AH:
		return "ah";
	case Register::BH:
		return "bh";
	case Register::CH:
		return "ch";
	case Register::DH:
		return "dh";
	case Register::AX:
		return "ax";
	case Register::BX:
		return "bx";
	case Register::CX:
		return "cx";
	case Register::DX:
		return "dx";
	case Register::BP:
		return "bp";
	case Register::SP:
		return "sp";
	case Register::SI:
		return "si";
	case Register::DI:
		return "di";
	case Register::CS:
		return "cs";
	case Register::DS:
		return "ds";
	case Register::SS:
		return "ss";
	case Register::ES:
		return "es";
	default:
		return "Unknown Register";
	}
}

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
		return false;
	}
	if (str.size() == 3 && str[0] == '\'' && str[3] == '\'')
	{
		value = static_cast<Byte>(str[1]);
		return true;
	}
	if (str.size() > 2 || str[0] == '0')
	{
		if (str[1] == 'x')
		{
			std::string substr = str.substr(2);
			try
			{
				value = static_cast<Word>(std::stoi(substr, nullptr, 16));
			}
			catch (...)
			{
				return false;
			}
		}
		else if (str[1] >= '1' && str[1] < '7')
		{
			std::string substr = str.substr(1);
			try
			{
				value = static_cast<Word>(std::stoi(substr, nullptr, 8));
			}
			catch (...)
			{
				return false;
			}
		}
		else if (str[1] == 'b')
		{
			std::string substr = str.substr(2);
			try
			{
				value = static_cast<Word>(std::stoi(substr, nullptr, 2));
			}
			catch (...)
			{
				return false;
			}
		}
		else
		{
			try
			{
				value = static_cast<Word>(std::stoi(str));
			}
			catch (...)
			{
				return false;
			}
		}
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

int main(int argc, char** argv)
{
	uint8 errorCode = ParseCommand(argc, argv);
	if (inputFileName.empty())
	{
		std::cerr << "无输入文件" << std::endl;
		return ErrorCode::NoInput;
	}
	std::ifstream ifs{inputFileName};
	if (!ifs.is_open())
	{
		std::cerr << "错误: 无法打开文件" << inputFileName << std::endl;
		return ErrorCode::NoInput;
	}

	std::vector<Operand> operands;
	std::string line;
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

		for (const auto& token : tokens)
		{
			operands.push_back(ParseOperand(token));
		}
	}

	for (const auto& operand : operands)
	{
		std::cout << GetOperandTypeName(operand.type) << ' ' << (operand.type == OperandType::Register ? GetRegisterName(operand.reg) : std::to_string(operand.immediate));
		std::cout << std::endl;
	}

	return errorCode;
}