/**
 * @file Register.h
 * @author LinhengXilan
 * @version 0.0.0.7
 * @date 2026-10-2
 */

#ifndef _REGISTER_H_
#define _REGISTER_H_

#include <Types.h>

enum class RegisterType : uint8
{
	None,
	General,
	Segment
};

enum class RegisterSize : uint8
{
	None = 0,
	Bit8 = 8,
	Bit16 = 16
};

struct RegisterInfo
{
	RegisterType type = RegisterType::None;
	uint8 code = 0;
	RegisterSize size = RegisterSize::None;
	bool highByte = false;
	bool needRex = false;
};

constexpr RegisterInfo RegisterTable[] = {
	{ RegisterType::None, 0, RegisterSize::None, false, false }, // None
	{ RegisterType::General, 0, RegisterSize::Bit8, false, false }, // AL
	{ RegisterType::General, 1, RegisterSize::Bit8, false, false }, // CL
	{ RegisterType::General, 2, RegisterSize::Bit8, false, false }, // DL
	{ RegisterType::General, 3, RegisterSize::Bit8, false, false }, // BL
	{ RegisterType::General, 4, RegisterSize::Bit8, true, false }, // AH
	{ RegisterType::General, 5, RegisterSize::Bit8, true, false }, // CH
	{ RegisterType::General, 6, RegisterSize::Bit8, true, false }, // DH
	{ RegisterType::General, 7, RegisterSize::Bit8, true, false }, // BH
	{ RegisterType::General, 0, RegisterSize::Bit16, false, false }, // AX
	{ RegisterType::General, 1, RegisterSize::Bit16, false, false }, // CX
	{ RegisterType::General, 2, RegisterSize::Bit16, false, false }, // DX
	{ RegisterType::General, 3, RegisterSize::Bit16, false, false }, // BX
	{ RegisterType::General, 4, RegisterSize::Bit16, false, false }, // SP
	{ RegisterType::General, 5, RegisterSize::Bit16, false, false }, // BP
	{ RegisterType::General, 6, RegisterSize::Bit16, false, false }, // SI
	{ RegisterType::General, 7, RegisterSize::Bit16, false, false }, // DI
	{ RegisterType::Segment, 0, RegisterSize::Bit16, false, false }, // ES
	{ RegisterType::Segment, 1, RegisterSize::Bit16, false, false }, // CS
	{ RegisterType::Segment, 2, RegisterSize::Bit16, false, false }, // SS
	{ RegisterType::Segment, 3, RegisterSize::Bit16, false, false }, // DS
};

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
	DS,
	Count
};

static_assert(std::size(RegisterTable) == static_cast<size_t>(Register::Count), "RegisterTable 与 Register 枚举不一致");

const RegisterInfo& GetRegisterInfo(Register reg);

inline std::map<std::string, Register> RegisterMap = {
	{ "al", Register::AL },
	{ "bl", Register::BL },
	{ "cl", Register::CL },
	{ "dl", Register::DL },
	{ "ah", Register::AH },
	{ "bh", Register::BH },
	{ "ch", Register::CH },
	{ "dh", Register::DH },
	{ "ax", Register::AX },
	{ "bx", Register::BX },
	{ "cx", Register::CX },
	{ "dx", Register::DX },
	{ "bp", Register::BP },
	{ "sp", Register::SP },
	{ "si", Register::SI },
	{ "di", Register::DI },
	{ "cs", Register::CS },
	{ "ds", Register::DS },
	{ "ss", Register::SS },
	{ "es", Register::ES }
};

bool Is8bitRegister(Register reg);
bool Is16bitRegister(Register reg);
bool IsGeneralRegister(Register reg);
bool IsSegmentRegister(Register reg);
std::string GetRegisterName(Register reg);
Byte GetGeneralRegisterCode(Register reg);
Byte GetSegmentRegisterCode(Register reg);
Byte GetAnyRegisterCode(Register reg);

#endif