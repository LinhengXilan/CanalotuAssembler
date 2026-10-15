/**
 * @file Parse.h
 * @author LinhengXilan
 * @version 0.0.0.10
 * @date 2026-10-15
 */

#ifndef _PARSE_H_
#define _PARSE_H_

#include <Operand.h>

std::vector<std::string> Tokenize(const std::string& line);
bool ParseImmediate(const std::string& str, Word& value);
Operand ParseOperand(const std::string& token);

#endif