/**
 * @file Encode.h
 * @author LinhengXilan
 * @version 0.0.1.11
 * @date 2026-10-29
 */

#ifndef _ENCODE_H_
#define _ENCODE_H_

#include <Operand.h>
#include <Instruction.h>

namespace Mod
{
	constexpr uint8 Memory = 0b00;
	constexpr uint8 MemoryWith8bitDisplacement = 0b01;
	constexpr uint8 MemoryWith16bitDisplacement = 0b10;
	constexpr uint8 Register = 0b11;
};

bool MatchOperandCategory(uint32 category, const Operand& operand);
const Instruction* MatchInstruction(const std::string& mnemonic, const std::vector<Operand>& operands);
void EmitByte(Bytes& out, Byte data);
void EmitWord(Bytes& out, Word data);
Bytes EncodeInstruction(const std::string& mnemonic, const std::vector<Operand>& operands);
Byte EncodeModRM(uint8 mod, uint8 reg, uint8 rm);

#endif