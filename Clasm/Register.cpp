/**
 * @file Register.cpp
 * @author LinhengXilan
 * @version 0.0.0.6
 * @date 2026-9-20
 */

#include <Pch.h>
#include <Register.h>

bool Is8bitRegister(Register reg)
{
	return reg >= Register::AL && reg <= Register::BH;
}

bool Is16bitRegister(Register reg)
{
	return reg >= Register::AX && reg <= Register::DI;
}

bool IsSegmentRegister(Register reg)
{
	return reg >= Register::ES && reg <= Register::DS;
}

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

Byte GetRegisterCode(Register reg)
{
	switch (reg)
	{
	case Register::AL:
	case Register::AX:
		return 0;
	case Register::CL:
	case Register::CX:
		return 1;
	case Register::DL:
	case Register::DX:
		return 2;
	case Register::BL:
	case Register::BX:
		return 3;
	case Register::AH:
	case Register::SP:
		return 4;
	case Register::CH:
	case Register::BP:
		return 5;
	case Register::DH:
	case Register::SI:
		return 6;
	case Register::BH:
	case Register::DI:
		return 7;
	default:
		return 255;
	}
}

Byte GetSegmentRegisterCode(Register reg)
{
	switch (reg)
	{
	case Register::ES:
		return 0;
	case Register::CS:
		return 1;
	case Register::SS:
		return 2;
	case Register::DS:
		return 3;
	default:
		return 255;
	}
}