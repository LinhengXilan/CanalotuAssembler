/**
 * @file Parse.cpp
 * @author LinhengXilan
 * @version 0.0.0.10
 * @date 2026-10-15
 */

#include <Pch.h>
#include <Parse.h>

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
		std::runtime_error("非法内存操作数");
	}
	if (!token.empty())
	{
		tokens.push_back(token);
	}
	return tokens;
}

/**
 *@brief 将字符串转换为数值
 * @param[in] str 字符串
 * @param[out] value 数值
 * @ return 是否成功转换
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