/* ==================================================
FileName:	aes.h
Author:		SungJoon Lee
StudentID:	12141588
Created:	2016/10/25 Tue
Modified:	2016/10/28 Fri
================================================== */

#include <vector>
using namespace std;

#include "gf.h"

typedef vector<Byte>	ByteList;

const char *ToString(ByteList);

/* ==================================================
Polynominal customable AES Ecryptor/Decryptor

Provide:
	Encryt plain to cipher with key. 
	Decryt cipher to decryted with key. 
	On/off console log. (Expanded key and each steps in round)
================================================== */
class AES{
 public:
	AES();
	void set_polynominal(Byte polynominal);
	ByteList get_rc();
	void PrintLog(bool value);
	ByteList Encrypt(ByteList plain, ByteList key);
	ByteList Decrypt(ByteList chiper, ByteList key);
 private:
	void set_rc(Byte polynominal);
	/* Tokenize only 4 elements. (4개의 원소만을 추출하는 함수) */
	ByteList ListTok4(Byte * input);
	void KeyExpansion(ByteList key);
	ByteList Rfunction(ByteList input, int round);
	ByteList InitialRound(ByteList input, ByteList key);
	ByteList RegularRound(ByteList input, int round);
	ByteList InverseRegularRound(ByteList input, int round);
	ByteList FinalRound(ByteList input);
	ByteList InverseFinalRound(ByteList input);
	ByteList SubBytes(ByteList input);
	ByteList InverseSubBytes(ByteList input);
	Byte SBox(Byte input);
	Byte InverseSBox(Byte input);
	ByteList ShiftRows(ByteList input);
	ByteList InverseShiftRows(ByteList input);
	ByteList MixColumns(ByteList input);
	ByteList InverseMixColumns(ByteList input);
	ByteList AddRoundKey(ByteList input, ByteList key);

	bool print_log_;
	GF gf_;
	Byte polynominal_;
	ByteList round_key_[11];  // for round 0~10
	ByteList rc_;
};
