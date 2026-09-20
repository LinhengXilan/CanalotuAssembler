/**
 * @file Register.h
 * @author LinhengXilan
 * @version 0.0.0.6
 * @date 2026-9-20
 */

#ifndef _REGISTER_H_
#define _REGISTER_H_

#include <Types.h>

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

inline std::map<std::string, Register> RegisterMap = {{"al", Register::AL}, {"bl", Register::BL}, {"cl", Register::CL}, {"dl", Register::DL}, {"ah", Register::AH}, {"bh", Register::BH}, {"ch", Register::CH}, {"dh", Register::DH}, {"ax", Register::AX}, {"bx", Register::BX},
											   {"cx", Register::CX}, {"dx", Register::DX}, {"bp", Register::BP}, {"sp", Register::SP}, {"si", Register::SI}, {"di", Register::DI}, {"cs", Register::CS}, {"ds", Register::DS}, {"ss", Register::SS}, {"es", Register::ES}};

bool Is8bitRegister(Register reg);
bool Is16bitRegister(Register reg);
bool IsSegmentRegister(Register reg);
std::string GetRegisterName(Register reg);
Byte GetRegisterCode(Register reg);
Byte GetSegmentRegisterCode(Register reg);

#endif