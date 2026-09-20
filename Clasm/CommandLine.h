/**
 * @file CommandLine.h
 * @author LinhengXilan
 * @version 0.0.0.6
 * @date 2026-9-20
 */

#ifndef _COMMANDLINE_H_
#define _COMMANDLINE_H_

#include <Types.h>

namespace ErrorCode
{
	constexpr uint8 InvalidOption = 1;
	constexpr uint8 NoInput = 2;
} // namespace ErrorCode

struct CommandOption
{
	std::string inputFileName;
	std::string outputFileName;
};


void PrintHelp();
void PrintVersion();
uint8 ParseCommand(int argc, char** argv, CommandOption& option);

#endif