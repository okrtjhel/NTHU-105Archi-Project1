#include <iostream>
#include <fstream>
#include <cstdlib>
#include "memory.h"
#include "regfile.h"
#include "instruction.h"

using namespace std;

extern int reg[35];

fstream snapshot("_snapshot.rpt", fstream::out);
int cycle = 0;
bool isHalt = false;

int main(){
    ifstream iimg("./iimage.bin", ios::in | ios::binary);
    ifstream dimg("./dimage.bin", ios::in | ios::binary);
    initReg();
    initMemory();
    loadIimage(&iimg);
    loadDimage(&dimg);
    for(int i = 0; i < 34; i++)
        valueSet.insert(i);

    while(!isHalt){
    	showRegValue();
	    cycle++;
    	reg[PC] = reg[PC] + 4;
    	if(reg[PC] < 0 || reg[PC] > 1024){
    		cout << "Address overflow in I memory" << endl;
			exit(0);
    	}
    	decode(insMemory[(reg[PC] / 4) - 1]);
	    //snapshot << "cycle " << dec << cycle << " success" << endl;
    }
    return 0;
}
