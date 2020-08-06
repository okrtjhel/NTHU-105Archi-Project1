#include <iostream>
#include <cstdlib>
#include <set>
#include <bitset>
#include <iomanip>
#include "instruction.h"
#include "regfile.h"
#include "memory.h"
#include "error.h"

using namespace std;

extern fstream snapshot;
extern set<int> valueSet;

int signExtend8(int val){
	return (val << 24) >> 24;
}

int signExtend16(int val){
	return (val << 16) >> 16;
}

void decode(unsigned instr){
	unsigned opcode = instr >> 26;
	unsigned rs = (instr << 6) >> 27;
	unsigned rt = (instr << 11) >> 27;
	unsigned rd = (instr << 16) >> 27;
	unsigned shamt = (instr << 21) >> 27;
	unsigned func = (instr << 26) >> 26;
	int  imme = (instr << 16) >> 16;
	unsigned addr = (instr << 6) >> 6;
	int temp = 0, tempHI = 0, tempLO = 0;

	switch(opcode){
		//R-format
		case 0x00:
			switch(func){
				case 0x20:	//add
					snapshot << "add " << dec << rd << " " << dec << rs << " " << dec << rt << endl;
					WriteToZero_Check(opcode, rt, rd);
					NumberOverflow_Check(reg[rd], reg[rs], reg[rt]);
					temp = reg[rd];
					reg[rd] = reg[rs] + reg[rt];						
					if(temp != reg[rd])
						valueSet.insert(rd);
					break;
				case 0x21:	//addu
					snapshot << "addu " << dec << rd << " " << dec << rs << " " << dec << rt << endl;
					WriteToZero_Check(opcode, rt, rd);
					temp = reg[rd];
					reg[rd] = reg[rs] + reg[rt];			
					if(temp != reg[rd])
						valueSet.insert(rd);
					break;
				case 0x22:	//sub
					snapshot << "sub " << dec << rd << " " << dec << rs << " " << dec << rt << endl;
					WriteToZero_Check(opcode, rt, rd);				
					NumberOverflow_Check(reg[rd], reg[rs], 0 - reg[rt]);
					temp = reg[rd];
					reg[rd] = reg[rs] - reg[rt];
					if(temp != reg[rd])
						valueSet.insert(rd);
					break;
				case 0x24:	//and	
					snapshot << "and " << dec << rd << " " << dec << rs << " " << dec << rt << endl;			
					WriteToZero_Check(opcode, rt, rd);
					temp = reg[rd];
					reg[rd] = reg[rs] & reg[rt];
					if(temp != reg[rd])
						valueSet.insert(rd);
					break;
				case 0x25:	//or
					snapshot << "or " << dec << rd << " " << dec << rs << " " << dec << rt << endl;
					WriteToZero_Check(opcode, rt, rd);
					temp = reg[rd];
					reg[rd] = reg[rs] | reg[rt];
					if(temp != reg[rd])
						valueSet.insert(rd);
					break;
				case 0x26:	//xor
					snapshot << "xor " << dec << rd << " " << dec << rs << " " << dec << rt << endl;
					WriteToZero_Check(opcode, rt, rd);
					temp = reg[rd];
					reg[rd] = reg[rs] ^ reg[rt];
					if(temp != reg[rd])
						valueSet.insert(rd);
					break;
				case 0x27:	//nor
					snapshot << "nor " << dec << rd << " " << dec << rs << " " << dec << rt << endl;
					WriteToZero_Check(opcode, rt, rd);
					temp = reg[rd];
					reg[rd] = !(reg[rs] | reg[rt]);						
					if(temp != reg[rd])
						valueSet.insert(rd);
					break;
				case 0x28:	//nand
					snapshot << "nand " << dec << rd << " " << dec << rs << " " << dec << rt << endl;				
					WriteToZero_Check(opcode, rt, rd);
					temp = reg[rd];
					reg[rd] = !(reg[rs] & reg[rt]);
					if(temp != reg[rd])
						valueSet.insert(rd);
					break;
				case 0x2A:	//slt
					snapshot << "slt " << dec << rd << " " << dec << rs << " " << dec << rt << endl;
					WriteToZero_Check(opcode, rt, rd);
					temp = reg[rd];
					reg[rd] = (reg[rs] < reg[rt]) ? 1 : 0;
					if(temp != reg[rd])
						valueSet.insert(rd);
					break;
				case 0x00:	//sll
					snapshot << "sll " << dec << rd << " " << dec << rs << " " << dec << shamt << endl;
					if(!(rd == 0 && rs == 0 && shamt == 0)){
						WriteToZero_Check(opcode, rt, rd);						
						temp = reg[rd];
						reg[rd] = (unsigned)reg[rs] << shamt;
						if(temp != reg[rd])
							valueSet.insert(rd);
					}
					break;
				case 0x02:	//srl
					snapshot << "srl " << dec << rd << " " << dec << rs << " " << dec << shamt << endl;
					WriteToZero_Check(opcode, rt, rd);
					temp = reg[rd];
					reg[rd] = (unsigned)reg[rs] >> shamt;
					if(temp != reg[rd])
						valueSet.insert(rd);
					break;
				case 0x03:	//sra
					snapshot << "sra " << dec << rd << " " << dec << rs << " " << dec << shamt << endl;
					WriteToZero_Check(opcode, rt, rd);
					temp = reg[rd];
					reg[rd] = reg[rs] << shamt;
					if(temp != reg[rd])
						valueSet.insert(rd);
					break;
				case 0x08:	//jr
					snapshot << "jr " << dec << rs << endl;
					reg[PC] = reg[rs];
					break;
				case 0x18:{	//mult
					snapshot << "mult " << dec << rs << " " << dec << rt << endl;
					OverwriteHI_LO_Check(true);
					long long ll = (long long)reg[rs] * (long long)reg[rt];
					tempHI = reg[HI];
					tempLO = reg[LO];
					reg[HI] = ll >> 32;
					reg[LO] = ll;
					if(tempHI != reg[HI])
						valueSet.insert(HI);
					if(tempLO != reg[LO])
						valueSet.insert(LO);
					break;
				}					
				case 0x19:{	//multu
					snapshot << "multu " << dec << rs << " " << dec << rt << endl;
					OverwriteHI_LO_Check(true);
					unsigned long long ull = (unsigned long long)(unsigned)reg[rs] * (unsigned long long)(unsigned)reg[rt];
					tempHI = reg[HI];
					tempLO = reg[LO];
					reg[HI] = ull >> 32;
					reg[LO] = ull;
					if(tempHI != reg[HI])
						valueSet.insert(HI);
					if(tempLO != reg[LO])
						valueSet.insert(LO);
					break;
				}
				case 0x10:	//mfhi
					snapshot << "mfhi " << dec << rd << endl;
					WriteToZero_Check(opcode, rd, rt);
					OverwriteHI_LO_Check(false);
					temp = reg[rd];
					reg[rd] = reg[HI];
					if(temp != reg[rd])
						valueSet.insert(rd);
					break;
				case 0x12:	//mflo
					snapshot << "mflo " << dec << rd << endl;
					WriteToZero_Check(opcode, rd, rt);
					OverwriteHI_LO_Check(false);
					temp = reg[rd];
					reg[rd] = reg[LO];
					if(temp != reg[rd])
						valueSet.insert(rd);
					break;
				default:
					cout << "illegal instruction found at 0x" << setfill('0') << setw(8) << hex << uppercase << reg[PC]-4 << endl;
					exit(0);
					break;
			}
			break;
		//I-format
		case 0x08:	//addi
			snapshot << "addi " << dec << rt << " " << dec << rs << " " << dec << imme << endl;
			imme = signExtend16(imme);		
			WriteToZero_Check(opcode, rt, rd);
			NumberOverflow_Check(reg[rt] , reg[rs], imme);
			temp = reg[rt];
			reg[rt] = reg[rs] + imme;	
			if(temp != reg[rt])
				valueSet.insert(rt);
			break;
		case 0x09:	//addiu
			snapshot << "addiu " << dec << rt << " " << dec << rs << " " << dec << imme << endl;
			imme = signExtend16(imme);
			WriteToZero_Check(opcode, rt, rd);
			temp = reg[rt];
			reg[rt] = reg[rs] + imme;
			if(temp != reg[rt])
				valueSet.insert(rt);
			break;
		case 0x23:	//lw
			snapshot << "lw " << dec << rt << " " << dec << imme << "(" << dec << rs << ")" << endl;
			imme = signExtend16(imme);
			WriteToZero_Check(opcode, rt, rd);
			NumberOverflow_Check(reg[rs] + imme, reg[rs], imme);
			temp = reg[rt];
			reg[rt] = loadMemory(reg[rs] + imme, 4);
			if(temp != reg[rt])
				valueSet.insert(rt);
			break;
		case 0x21:	//lh
			snapshot << "lh " << dec << rt << " " << dec << imme << "(" << dec << rs << ")" << endl;
			imme = signExtend16(imme);
			WriteToZero_Check(opcode, rt, rd);
			NumberOverflow_Check(reg[rs] + imme, reg[rs], imme);
			temp = reg[rt];
			reg[rt] = loadMemory(reg[rs] + imme, 2);
			reg[rt] = signExtend16(reg[rt]);
			if(temp != reg[rt])
				valueSet.insert(rt);					
			break;
		case 0x25:	//lhu
			snapshot << "lhu " << dec << rt << " " << dec << imme << "(" << dec << rs << ")" << endl;
			imme = signExtend16(imme);
			WriteToZero_Check(opcode, rt, rd);
			NumberOverflow_Check(reg[rs] + imme, reg[rs], imme);
			temp = reg[rt];
			reg[rt] = loadMemory(reg[rs] + imme, 2);
			if(temp != reg[rt])
				valueSet.insert(rt);
			break;
		case 0x20:	//lb
			snapshot << "lb " << dec << rt << " " << dec << imme << "(" << dec << rs << ")" << endl;
			imme = signExtend16(imme);
			WriteToZero_Check(opcode, rt, rd);
			NumberOverflow_Check(reg[rs] + imme, reg[rs], imme);
			temp = reg[rt];
			reg[rt] = loadMemory(reg[rs] + imme, 1);
			reg[rt] = signExtend8(reg[rt]);
			if(temp != reg[rt])
				valueSet.insert(rt);
			break;
		case 0x24:	//lbu
			snapshot << "lbu " << dec << rt << " " << dec << imme << "(" << dec << rs << ")" << endl;
			imme = signExtend16(imme);			
			WriteToZero_Check(opcode, rt, rd);
			NumberOverflow_Check(reg[rs] + imme, reg[rs], imme);
			temp = reg[rt];
			reg[rt] = loadMemory(reg[rs] + imme, 1);
			if(temp != reg[rt])
				valueSet.insert(rt);
			break;
		case 0x2B:	//sw
			snapshot << "sw " << dec << rt << " " << dec << imme << "(" << dec << rs << ")" << endl;
			imme = signExtend16(imme);
			NumberOverflow_Check(reg[rs] + imme, reg[rs], imme);
			saveMemory(reg[rs] + imme, 4, reg[rt]);
			break;
		case 0x29:	//sh
			snapshot << "sh " << dec << rt << " " << dec << imme << "(" << dec << rs << ")" << endl;
			imme = signExtend16(imme);
			NumberOverflow_Check(reg[rs] + imme, reg[rs], imme);
			saveMemory(reg[rs] + imme, 2, reg[rt]);
			break;
		case 0x28:	//sb
			snapshot << "sb " << dec << rt << " " << dec << imme << "(" << dec << rs << ")" << endl;
			imme = signExtend16(imme);
			NumberOverflow_Check(reg[rs] + imme, reg[rs], imme);
			saveMemory(reg[rs] + imme, 1, reg[rt]);
			break;
		case 0x0F:	//lui
			snapshot << "lui " << dec << rt << " " << dec << imme << endl;
			WriteToZero_Check(opcode, rt, rd);
			temp = reg[rt];
			reg[rt] = imme << 16;
			if(temp != reg[rt])
				valueSet.insert(rt);
			break;
		case 0x0C:	//andi
			snapshot << "andi " << dec << rt << " " << dec << rs << " " << dec << imme << endl;
			WriteToZero_Check(opcode, rt, rd);
			temp = reg[rt];
			reg[rt] = reg[rs] & (unsigned)imme;
			if(temp != reg[rt])
				valueSet.insert(rt);
			break;
		case 0x0D:	//ori
			snapshot << "ori " << dec << rt << " " << dec << rs << " " << dec << imme << endl;	
			WriteToZero_Check(opcode, rt, rd);
			temp = reg[rt];
			reg[rt] = reg[rs] | (unsigned)imme;
			if(temp != reg[rt])
				valueSet.insert(rt);
			break;
		case 0x0E:	//nori
			snapshot << "nori " << dec << rt << " " << dec << rs << " " << dec << imme << endl;
			WriteToZero_Check(opcode, rt, rd);
			temp = reg[rt];
			reg[rt] = !(reg[rs] | (unsigned)imme);
			if(temp != reg[rt])
				valueSet.insert(rt);
			break;
		case 0x0A:	//slti
			snapshot << "slti " << dec << rt << " " << dec << rs << " " << dec << imme << endl;
			WriteToZero_Check(opcode, rt, rd);
			temp = reg[rt];
			reg[rt] = (reg[rs] < (unsigned)imme) ? 1 : 0;
			if(temp != reg[rt])
				valueSet.insert(rt);
			break;
		case 0x04:	//beq
			snapshot << "beq " << dec << rs << " " << dec << rt << " " << dec << imme << endl;
			NumberOverflow_Check((reg[PC] + 4 * imme), reg[PC], 4 * imme);
			if(reg[rs] == reg[rt])
				reg[PC] = reg[PC] + 4 * imme;
			break;
		case 0x05:	//bne
			snapshot << "bne " << dec << rs << " " << dec << rt << " " << dec << imme << endl;
			NumberOverflow_Check((reg[PC] + 4 * imme), reg[PC], 4 * imme);
			if(reg[rs] != reg[rt])
				reg[PC] = reg[PC] + 4 * imme;
			break;
		case 0x07:	//bgtz
			snapshot << "beq " << dec << rs << " " << dec << imme << endl;
			NumberOverflow_Check((reg[PC] + 4 * imme), reg[PC], 4 * imme);
			if(reg[rs] > 0)
				reg[PC] = reg[PC] + 4 * imme;
			break;
		//J-format
		case 0x02:	//j
			snapshot << "j " << dec << addr << endl;
			reg[PC] = ((unsigned)reg[PC] >> 28) | (4 * (unsigned)addr);
			break;
		case 0x03:	//jal
			snapshot << "jal " << dec << addr << endl;
			temp = reg[31];
			reg[31] = reg[PC];
			if(temp != reg[31])
				valueSet.insert(31);
			reg[PC] = ((unsigned)reg[PC] >> 28) | (4 * (unsigned)addr);
			break;
		//S-format
		case 0x3F:	//halt
			cout << "halt" << endl;
			exit(0);
			break;
		//illegal
		default:
			cout << "illegal instruction found at 0x" << setfill('0') << setw(8) << hex << uppercase << (reg[PC] - 4) << endl;
			exit(0);
			break;
	}
	reg[0] = 0;
	valueSet.erase(0);
}
