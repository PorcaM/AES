/* ==================================================
FileName:	aes.cpp
Author:		SungJoon Lee
StudentID:	12141588
Created:	2016/10/25 Tue
Modified:	2016/10/28 Fri
================================================== */

#pragma warning(disable:4996)

#include "aes.h"

#include <string.h>

#define AES_STD_POLY 		0x1B
#define AES_LOG(fmt, ...) 	if (print_log_) printf(fmt, ##__VA_ARGS__)

char aes_buff[256];  /* This var MUST not be changed by user. (READ ONLY)
						This array is pointer for string in some cases. 
						See ToString(ByteList) function. */

const char *ToString(ByteList list) {
	strcpy(aes_buff, "");
	for (int i = 0; i < list.size(); ++i) {
		char buf[4];
		snprintf(buf, 4, "%02X ", list[i]);
		strncat(aes_buff, buf, 3);
	}
	strcat(aes_buff, "\0");
	return aes_buff;
}

AES::AES() : gf_(AES_STD_POLY) {
	print_log_ = false;
	set_polynominal(AES_STD_POLY);
}
void AES::set_polynominal(Byte polynominal) {
	polynominal_ = polynominal;
	gf_.set_polynominal(polynominal);
	set_rc(polynominal_);
	return;
}
ByteList AES::get_rc() {
	return rc_;
}
void AES::PrintLog(bool value) {
	print_log_ = value;
	return;
}
ByteList AES::Encrypt(ByteList plain, ByteList key) {
	AES_LOG("\n<----- ENCRYPTION ----->\n\n");
	ByteList cipher = plain;
	KeyExpansion(key);
	AES_LOG("Round 0\n");
	cipher = InitialRound(cipher, key);
	for (int i = 0; i < 9; ++i) {
		AES_LOG("Round %d\n", i+1);
		cipher = RegularRound(cipher, i + 1);
	}
	AES_LOG("Round 10\n");
	cipher = FinalRound(cipher);
	return cipher;
}
ByteList AES::Decrypt(ByteList cipher, ByteList key) {
	AES_LOG("\n<----- DECRYPTION ----->\n\n");
	ByteList decripted = cipher;
	KeyExpansion(key);
	AES_LOG("Round 0\n");
	decripted = InitialRound(decripted, round_key_[10]);
	for (int i = 0; i < 9; ++i) {
		AES_LOG("Round %d\n", i + 1);
		decripted = InverseRegularRound(decripted, i + 1);
	}
	AES_LOG("Round 10\n");
	decripted = InverseFinalRound(decripted);
	return decripted;
}
void AES::set_rc(Byte polynominal) {
	Byte tmp = 0x01;
	rc_.clear();
	rc_.reserve(10);
	for (int i = 0; i < 8; ++i) {
		rc_.push_back(tmp);
		tmp = tmp << 1;
	}
	rc_.push_back(gf_.Multiplication(0x80, 0x02));
	rc_.push_back(gf_.Multiplication(0x80, 0x04));
	return;
}
/* Tokenize only 4 elements. (4개의 원소만을 추출하는 함수) */
ByteList AES::ListTok4(Byte *input) {
	ByteList result;
	result.push_back(input[0]);
	result.push_back(input[1]);
	result.push_back(input[2]);
	result.push_back(input[3]);
	return result;
}
void AES::KeyExpansion(ByteList key) {
	AES_LOG("KEY EXPANSION\n");
	for (int i = 0; i < 11; ++i) round_key_[i].clear();
	round_key_[0] = key;
	for (int i = 1; i < 11; ++i) {
		/* In case round_key_index%4 == 0, with R function */
		ByteList rk_4 = ListTok4(&round_key_[i - 1][0]);  // w[i-4]
		ByteList rk_1 = Rfunction(ListTok4(&round_key_[i - 1][12]), i - 1);  // R(w[i-1])
		for (int k = 0; k < 4; ++k) {
			Byte tmp = rk_4[k] ^ rk_1[k];
			round_key_[i].push_back(tmp);
		}
		/* Else */
		for (int j = 1; j < 4; ++j) {
			rk_4 = ListTok4(&round_key_[i - 1][j * 4]);  // w[i-4]
			rk_1 = ListTok4(&round_key_[i][(j - 1) * 4]);  // w[i-1]
			for (int k = 0; k < 4; ++k) {
				Byte tmp = rk_4[k] ^ rk_1[k];
				round_key_[i].push_back(tmp);
			}
		}
	}
	for (int i = 0; i < 11; ++i)
		AES_LOG("ROUND %d: \t%s\n", i,
			ToString(round_key_[i]));
	AES_LOG("\n");
	return;
}
ByteList AES::Rfunction(ByteList input, int round) {
	/* Shift */
	Byte tmp = input[0];
	input.erase(input.begin());
	input.push_back(tmp);
	/* SubByte for each byte */
	for (int i = 0; i < 4; ++i)
		input[i] = SBox(input[i]);
	/* xor rc */
	input[0] ^= rc_[round];
	return input;  // input is 4Bytes
}
ByteList AES::InitialRound(ByteList input, ByteList key) {
	input = AddRoundKey(input, key);
	return input;
}
ByteList AES::RegularRound(ByteList input, int round) {
	input = SubBytes(input);
	input = ShiftRows(input);
	input = MixColumns(input);
	input = AddRoundKey(input, round_key_[round]);
	return input;
}
ByteList AES::InverseRegularRound(ByteList input, int round) {
	input = InverseShiftRows(input);
	input = InverseSubBytes(input);
	input = AddRoundKey(input, round_key_[10 - round]);
	input = InverseMixColumns(input);
	return input;
}
ByteList AES::FinalRound(ByteList input) {
	input = SubBytes(input);
	input = ShiftRows(input);
	// No mix columns
	input = AddRoundKey(input, round_key_[10]);
	return input;
}
ByteList AES::InverseFinalRound(ByteList input) {
	input = InverseShiftRows(input);
	input = InverseSubBytes(input);
	input = AddRoundKey(input, round_key_[0]);
	// No mix columns
	return input;
}
ByteList AES::SubBytes(ByteList input) {
	for (int i = 0; i < input.size(); ++i)
		input[i] = SBox(input[i]);
	AES_LOG("SB: %s\n", ToString(input));
	return input;
}
ByteList AES::InverseSubBytes(ByteList input) {
	for (int i = 0; i < input.size(); ++i)
		input[i] = InverseSBox(input[i]);
	AES_LOG("SB: %s\n", ToString(input));
	return input;
}
Byte AES::SBox(Byte input) {
	Byte matrix[8];
	Byte inverse = gf_.Inverse(input);
	Byte result = 0x00;
	/* Get matrix */
	for (int i = 0; i < 8; ++i)
		matrix[i] = (0x8F >> i) | (0x8F << 8 - i);
	/* For each bit */
	for (int j = 0; j < 8; ++j) {
		/* Matrix Multiplication */
		bool bit = false;
		for (int k = 0; k < 8; ++k) {
			bool a = get_bit(matrix[j], 7 - k);
			bool b = get_bit(inverse, k);
			if (a && b) bit = !bit;
		}
		/* xor(GF addition) with bit of 0x63 */
		bool c = get_bit(0x63, j);
		if (c) bit = !bit;
		/* Save result */
		set_bit(result, j, bit);
	}
	return result;
}
Byte AES::InverseSBox(Byte input) {
	Byte matrix[8];
	Byte result = 0x00;
	/* Get matrix */
	for (int i = 0; i < 8; ++i)
		matrix[i] = (0x25 >> i) | (0x25 << 8 - i);
	/* For each bit */
	for (int j = 0; j < 8; ++j) {
		/* Matrix Multiplication */
		bool bit = false;
		for (int k = 0; k < 8; ++k) {
			bool a = get_bit(matrix[j], 7 - k);
			bool b = get_bit(input, k);  // [In inverse] Not inverse
			if (a && b) bit = !bit;
		}
		/* xor(GF addition) with bit of 0x05 */
		bool c = get_bit(0x05, j);
		if (c) bit = !bit;
		/* Save result */
		set_bit(result, j, bit);
	}
	/* [In inverse] Return inverse */
	return gf_.Inverse(result);
}
ByteList AES::ShiftRows(ByteList input) {
	ByteList row;
	for (int i = 1; i < 4; ++i) {
		row.clear();
		row.push_back(input[i ]);
		row.push_back(input[i + 4]);
		row.push_back(input[i + 8]);
		row.push_back(input[i + 12]);
		for (int j = 0; j < i; ++j) {
			/* Pop front and push back as tail */
			Byte tmp = row[0];
			row.erase(row.begin());
			row.push_back(tmp);
		}
		input[i] = row[0];
		input[i + 4] = row[1];
		input[i + 8] = row[2];
		input[i + 12] = row[3];
	}
	AES_LOG("SR: %s\n", ToString(input));
	return input;
}
ByteList AES::InverseShiftRows(ByteList input) {
	ByteList row;
	for (int i = 1; i < 4; ++i) {
		row.clear();
		row.push_back(input[i]);
		row.push_back(input[i + 4]);
		row.push_back(input[i + 8]);
		row.push_back(input[i + 12]);
		for (int j = 0; j < 4 - i; ++j) {  // [In inverse] different condition of 'for statement'
			/* Pop front and push back as tail */
			Byte tmp = row[0];
			row.erase(row.begin());
			row.push_back(tmp);
		}
		input[i] = row[0];
		input[i + 4] = row[1];
		input[i + 8] = row[2];
		input[i + 12] = row[3];
	}
	AES_LOG("SR: %s\n", ToString(input));
	return input;
}
ByteList AES::MixColumns(ByteList input) {
	ByteList matrix = { 2, 3, 1, 1,
						1, 2, 3, 1,
						1, 1, 2, 3,
						3, 1, 1, 2 };
	ByteList result(16, 0x00);
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 4; ++k) {
				Byte a = matrix[i * 4 + k];
				Byte b = input[j * 4 + k];
				Byte tmp = gf_.Multiplication(a, b);
				result[i + j * 4] ^= tmp;
			}
		}
	}
	AES_LOG("MC: %s\n", ToString(result));
	return result;
}
ByteList AES::InverseMixColumns(ByteList input) {
	const Byte kA = 0x0E, kB = 0x0B, kC = 0x0D, kD = 0x09;
	// [In inverse] different matrix
	ByteList matrix = { kA, kB, kC, kD,
						kD, kA, kB, kC,
						kC, kD, kA, kB,
						kB, kC, kD, kA };
	ByteList result(16, 0x00);
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 4; ++k) {
				Byte a = matrix[i * 4 + k];
				Byte b = input[j * 4 + k];
				Byte tmp = gf_.Multiplication(a, b);
				result[i + j * 4] ^= tmp;
			}
		}
	}
	AES_LOG("MC: %s\n", ToString(result));
	return result;
}
ByteList AES::AddRoundKey(ByteList input, ByteList key) {
	for (int i = 0; i < 16; ++i) {
		input[i] ^= key[i];
	}
	AES_LOG("AR: %s\n", ToString(input));
	return input;
}

#undef AES_STD_POLY
#undef AES_LOG