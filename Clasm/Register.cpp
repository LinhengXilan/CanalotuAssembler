/**
 * @file Register.cpp
 * @author LinhengXilan
 * @version 0.0.0.7
 * @date 2026-10-2
 */

#include <Pch.h>
#include <Register.h>

const RegisterInfo& GetRegisterInfo(Register reg)
{
	return RegisterTable[static_cast<uint8>(reg)];
}

bool Is8bitRegister(Register reg)
{
	return GetRegisterInfo(reg).size == RegisterSize::Bit8;
}

bool Is16bitRegister(Register reg)
{
	return GetRegisterInfo(reg).size == RegisterSize::Bit16;
}

bool IsGeneralRegister(Register reg)
{
	return GetRegisterInfo(reg).type == RegisterType::General;
}

bool IsSegmentRegister(Register reg)
{
	return GetRegisterInfo(reg).type == RegisterType::Segment;
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

Byte GetGeneralRegisterCode(Register reg)
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
		//TODO: 报错信息
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
		//TODO: 报错信息
		return 255;
	}
}

Byte GetAnyRegisterCode(Register reg)
{
	if (IsGeneralRegister(reg))
	{
		return GetGeneralRegisterCode(reg);
	}
	else if (IsSegmentRegister(reg))
	{
		return GetSegmentRegisterCode(reg);
	}
	else
	{
		//TODO: 报错信息
		return 255;
	}
}