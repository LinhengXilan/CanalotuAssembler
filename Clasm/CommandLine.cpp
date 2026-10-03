/**
 * @file CommandLine.cpp
 * @author LinhengXilan
 * @version 0.0.0.8
 * @date 2026-10-3
 */

#include <Pch.h>
#include <CommandLine.h>

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
	std::cout << "Version 0.0.0 Build8" << std::endl;
}

uint8 ParseCommand(int argc, char** argv, CommandOption& option)
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
				if (i + 1 < argc)
				{
					option.outputFileName = argv[++i];
				}
				else
				{
					std::cerr << "-o 没有参数: " << argv[i] << std::endl;
					return ErrorCode::InvalidOption;
				}
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
			option.inputFileName = argv[i];
		}
	}
	return 0;
}