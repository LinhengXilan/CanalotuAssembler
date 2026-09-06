/**
 * @file Main.cpp
 * @author LinhengXilan
 * @version 0.0.0.2
 * @date 2026-9-6
 */

#include <iostream>
#include <string>
#include <Types.h>
#include <fstream>
#include <map>
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
		std::cout << "Version 0.0.0 Build2" << std::endl;
	}
} // namespace

namespace ErrorCode
{
	constexpr uint8 InvalidOption = 1;
	constexpr uint8 NoInput		  = 2;
}

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
	AX, BX, CX, DX, BP, SP, SI, DI,
	CS, DS, SS, ES
};

std::map<Register, std::string> RegisterName = {
	{Register::AL, "AL"},
	{Register::BL, "BL"},
	{Register::CL, "CL"},
	{Register::DL, "DL"},
	{Register::AH, "AH"},
	{Register::BH, "BH"},
	{Register::CH, "CH"},
	{Register::DH, "DH"},
	{Register::AX, "AX"},
	{Register::BX, "BX"},
	{Register::CX, "CX"},
	{Register::DX, "DX"},
	{Register::BP, "BP"},
	{Register::SP, "SP"},
	{Register::SI, "SI"},
	{Register::DI, "DI"},
	{Register::DS, "DS"},
	{Register::CS, "CS"},
	{Register::SS, "SS"},
	{Register::ES, "ES"}
};

enum class OperandType : uint8
{
	Register,
	Immediate,
	Memory
};

//struct Operand
//{
//	OperandType type;
//	union
//	{
//		Word immediate;
//	};
//};

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
		std::string mnemonic = tokens[0];
		tokens.erase(tokens.begin());

		/*std::vector<Operand> operands;*/
	}

	return errorCode;
}