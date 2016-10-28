/* ==================================================
FileName:	gf.h
Author:		SungJoon Lee
StudentID:	12141588
Created:	2016/10/26 Wed
Modified:	2016/10/28 Fri
================================================== */

typedef unsigned char	Byte;

bool get_bit(Byte a, int index);
void set_bit(Byte &a, int index, bool value);

/* ==================================================
Galois Field Operator for AES. 

Provide: 
	getter/setter for polynominal
	Multiplication in galois field
	Addition in galois field
	Inverse in galois field
================================================== */
class GF{
 public:
	GF(Byte polynominal);
	Byte get_polynominal();
	void set_polynominal(Byte polynominal);
	Byte Multiplication(Byte a, Byte b);
	Byte Addition(Byte a, Byte b);
	Byte Inverse(Byte a);
 private:
	Byte Division(unsigned int m, Byte d, Byte &r);
	Byte EEA(Byte b3);
	Byte polynominal_;  /*	In AES, polynominal ALWAYS has x^8.
							So I decided 8 bit var for saving 
							coefficients from x^7 to x^0.		*/
};