/**
 * @file Parse.cpp
 * @author LinhengXilan
 * @version 0.0.1.11
 * @date 2026-10-29
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
		throw std::runtime_error("非法内存操作数");
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

bool ParseAddress(const std::string token, Operand& operand, bool sign = true)
{
	Word value;
	if (ParseImmediate(token, value) && !operand.hasDisplacement)
	{

		operand.displacement = sign ? value : -value;
		operand.hasDisplacement = true;
		return true;
	}
	else
	{
		if (!sign)
		{
			//TODO: Error
			throw std::runtime_error("寄存器前不允许负号");
		}
		auto it = RegisterMap.find(token);
		if (it != RegisterMap.end())
		{
			switch (it->second)
			{
			case Register::BX:
				if (operand.memoryBase == Register::None)
				{
					operand.memoryBase = Register::BX;
					return true;
				}
				break;
			case Register::BP:
				if (operand.memoryBase == Register::None)
				{
					operand.memoryBase = Register::BP;
					return true;
				}
				break;
			case Register::SI:
				if (operand.memoryIndex == Register::None)
				{
					operand.memoryIndex = Register::SI;
					return true;
				}
				break;
			case Register::DI:
				if (operand.memoryIndex == Register::None)
				{
					operand.memoryIndex = Register::DI;
					return true;
				}
				break;
			default:
				return false;
			}
		}
	}
	return false;
}

Operand ParseMemoryOperand(const std::string& memoryStr)
{
	Operand operand;
	operand.type = OperandType::Memory;

	std::string addressStr = memoryStr;
	usize seprator = memoryStr.find(':');
	if (seprator != std::string::npos)
	{
		std::string segment = memoryStr.substr(0, seprator);
		auto it = RegisterMap.find(segment);
		if (it != RegisterMap.end() && IsSegmentRegister(it->second))
		{
			operand.segment = it->second;
			addressStr = memoryStr.substr(seprator + 1);
		}
		else
		{
			throw std::runtime_error("段寄存器错误");
		}
	}

	std::erase_if(
		addressStr,
		[](char ch) {
			return std::isspace(static_cast<uint8>(ch));
		}
	);

	while (!addressStr.empty())
	{
		int sign = 0;
		seprator = addressStr.find_last_of("+-");
		if (seprator != std::string::npos)
		{
		
			std::string token = addressStr.substr(seprator + 1);
			usize firstSign = addressStr.find_last_not_of("+-", seprator);
			if (firstSign == std::string::npos)
			{
				throw std::runtime_error("无效的内存格式");
			}
			std::string signStr = addressStr.substr(firstSign + 1, seprator - firstSign);
			addressStr = addressStr.substr(0, firstSign + 1);
			for (const auto& ch : signStr)
			{
				if (ch == '-')
				{
					sign++;
				}
			}
			if (!ParseAddress(token, operand, sign % 2 == 0))
			{
				throw std::runtime_error("无效的内存格式");
			}
		}
		else
		{
			if (!ParseAddress(addressStr, operand))
			{
				throw std::runtime_error("无效的内存格式");
			}
			addressStr.clear();
		}
	}
	return operand;
}

Operand ParseOperand(const std::string& token)
{
	Operand operand;

	if (token.size() > 2 && token.front() == '[' && token.back() == ']')
	{
		return ParseMemoryOperand(token.substr(1, token.size() - 2));
	}

	auto it = RegisterMap.find(token);
	if (it != RegisterMap.end())
	{
		operand.type = OperandType::Register;
		operand.reg = it->second;
		if (Is8bitRegister(operand.reg))
		{
			operand.size = OperandSize::Byte;
		}
		else if (Is16bitRegister(operand.reg) || IsSegmentRegister(operand.reg))
		{
			operand.size = OperandSize::Word;
		}
		return operand;
	}

	if (ParseImmediate(token, operand.immediate))
	{
		operand.type = OperandType::Immediate;
		return operand;
	}

	throw std::runtime_error("未知操作数: " + token);
}