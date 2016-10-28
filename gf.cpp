/* ==================================================
FileName:	gf.cpp
Author:		SungJoon Lee
StudentID:	12141588
Created:	2016/10/26 Wed
Modified:	2016/10/28 Fri
================================================== */

#include "gf.h"

#define BIT_OF(x)			(sizeof(x)*8)
#define MAX_DEGREE(x, max)	for (int i = 0; i < BIT_OF(x); ++i)\
								if (x & (0x01 << i)) max = i;

bool get_bit(Byte a, int index) {
	if ((a >> index) & 0x01)	return true;
	else						return false;
}
void set_bit(Byte &a, int index, bool value) {
	Byte tmp = 0x01 << index;
	if (value)	a |= tmp;  // set bit
	else		a &= ~tmp; // clr bit
	return;
}

GF::GF(Byte polynominal) {
	set_polynominal(polynominal);
}
Byte GF::get_polynominal() {
	return polynominal_;
}
void GF::set_polynominal(Byte polynominal) {
	polynominal_ = polynominal;
	return;
}
Byte GF::Multiplication(Byte a, Byte b) {
	unsigned int m = 0;
	/* Simply multiplication to large varibale. */
	for (int i = 0; i < BIT_OF(b); ++i)	 // Repeat for each bit of b. 
		if (get_bit(b, i) == true)
			m ^= a << i;				 // Shift a as degree of bit, then add to m. 
	/* If result is too big, then reduce. */
	while (m > 0xFF) {
		int max = 0;
		MAX_DEGREE(m, max);
		m ^= (polynominal_ | 0x100) << (max - 8);  // xor with shifted polynominal. 
	}
	return m;
}
Byte GF::Division(unsigned int m, Byte d, Byte &r) {
	Byte q = 0x00;
	int max1 = 0, max2 = 0;  // maximum degree of m and d
	MAX_DEGREE(m, max1);
	MAX_DEGREE(d, max2);
	do {
		int diff = max1 - max2;
		set_bit(q, diff, true);
		m ^= d << diff;
		MAX_DEGREE(m, max1);;  // update degree_max value of m
	} while (max1 > max2);
	r = m;
	return q;
}
Byte GF::Addition(Byte a, Byte b) {
	return a^b;
}
Byte GF::Inverse(Byte a) {
	if (a == 0) return 0;
	else		return EEA(a);
}
Byte GF::EEA(Byte b3) {
	Byte A1, A2, B1, B2, B3, Q, R;
	unsigned int A3;
	A1 = 1;	A2 = 0;
	A3 = polynominal_ | 0x100;
	B1 = 0;	B2 = 1;
	B3 = b3;
	Q = R = 0;
	while (B3 != 1) {
		Q = Division(A3, B3, R);
		/* Save previous A1, A2 before update. */
		Byte t1, t2;
		t1 = A1; t2 = A2;
		A1 = B1; A2 = B2; A3 = B3;
		B1 = Addition(t1, Multiplication(Q, B1));
		B2 = Addition(t2, Multiplication(Q, B2));
		B3 = R;
	}
	return B2;
}

#undef BIT_OF
#undef MAX_DEGREE