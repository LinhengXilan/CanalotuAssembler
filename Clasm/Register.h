/**
 * @file Register.h
 * @author LinhengXilan
 * @version 0.0.0.5
 * @date 2026-9-18
 */

enum class Register : uint8
{
	None,
	AL,
	CL,
	DL,
	BL,
	AH,
	CH,
	DH,
	BH,
	AX,
	CX,
	DX,
	BX,
	SP,
	BP,
	SI,
	DI,
	ES,
	CS,
	SS,
	DS
};

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

std::map<std::string, Register> RegisterMap = {{"al", Register::AL}, {"bl", Register::BL}, {"cl", Register::CL}, {"dl", Register::DL}, {"ah", Register::AH}, {"bh", Register::BH}, {"ch", Register::CH}, {"dh", Register::DH}, {"ax", Register::AX}, {"bx", Register::BX},
											   {"cx", Register::CX}, {"dx", Register::DX}, {"bp", Register::BP}, {"sp", Register::SP}, {"si", Register::SI}, {"di", Register::DI}, {"cs", Register::CS}, {"ds", Register::DS}, {"ss", Register::SS}, {"es", Register::ES}};

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