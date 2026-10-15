/**
 * @file Main.cpp
 * @author LinhengXilan
 * @version 0.0.0.10
 * @date 2026-10-15
 */

#include <Pch.h>
#include <CommandLine.h>
#include <Instruction.h>
#include <Register.h>
#include <Parse.h>
#include <Encode.h>

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

	/* vvv 调试用 vvv */
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