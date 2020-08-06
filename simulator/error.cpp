#include <iostream>
#include <bitset>
#include <fstream>
#include "error.h"

using namespace std;

fstream error("error_dump.rpt", ios::out);
extern int cycle;
extern bool isHalt;

void WriteToZero_Check(unsigned opcode, unsigned rt, unsigned rd){
	//R-format
	if(opcode == 0x00){
		if(rd == 0){
			error << "In cycle " << dec << cycle << ": Write $0 Error" << endl;
		}
	}
	//I-format
	else{
		if(rt == 0){
			error << "In cycle " << dec << cycle << ": Write $0 Error" << endl;
		}
	}
}

void NumberOverflow_Check(int out, int in1, int in2){
	bitset<32> b0 = out, b1 = in1, b2 = in2;
	if(b1[31] == b2[31] && b1[31] != b0[31])
		error << "In cycle " << dec << cycle << ": Number Overflow" << endl;
}

void OverwriteHI_LO_Check(bool doMult){
	static bool needAccessed = false;
	if(doMult && needAccessed){
		error << "In cycle " << dec << cycle << ": Overwrite HI-LO registers" << endl;
	}
	else if(doMult)
		needAccessed = true;
	else
		needAccessed = false;
}

void AddressOverflow_Check(int location, int byte){
	if((location < 0) || (location > 1024) || (location + byte > 1024)){
		error << "In cycle " << dec << cycle << ": Address Overflow" << endl;
		isHalt = true;
	}
}

void Misaligned_Check(int location, int byte){
	if(location % byte){
		error << "In cycle " << dec << cycle << ": Misalignment Error" << endl;
		isHalt = true;
	}
}
