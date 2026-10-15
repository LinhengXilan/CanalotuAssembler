/**
 * @file Encode.h
 * @author LinhengXilan
 * @version 0.0.0.10
 * @date 2026-10-15
 */

#ifndef _ENCODE_H_
#define _ENCODE_H_

#include <Operand.h>
#include <Instruction.h>

bool MatchOperandCategory(uint32 category, const Operand& operand);
const Instruction* MatchInstruction(const std::string& mnemonic, const std::vector<Operand>& operands);
void EmitByte(Bytes& out, Byte data);
void EmitWord(Bytes& out, Word data);
Bytes EncodeInstruction(const std::string& mnemonic, const std::vector<Operand>& operands);
Byte EncodeModRM(Mod mod, uint8 reg, uint8 rm);

#endif