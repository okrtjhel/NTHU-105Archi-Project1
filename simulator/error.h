#ifndef _ERROR_H_
#define _ERROR_H_

extern void WriteToZero_Check(unsigned, unsigned, unsigned);
extern void NumberOverflow_Check(int, int, int);
extern void OverwriteHI_LO_Check(bool);
extern void AddressOverflow_Check(int, int);
extern void Misaligned_Check(int, int);

#endif