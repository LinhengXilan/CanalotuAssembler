/**
 * @file Main.cpp
 * @author LinhengXilan
 * @version 0.0.0.1
 * @date 2026-9-4
 */

#include <iostream>
#include <string>
#include <Types.h>

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
		std::cout << "Version 0.0.0 Build1" << std::endl;
	}
} // namespace

namespace ErrorCode
{
	constexpr uint8 InvalidOption = 1;
}

int main(int argc, char** argv)
{
	std::string outputFileName;
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
		std::string argument = argv[i];
	}

	std::cout << outputFileName << std::endl;
	return 0;
}