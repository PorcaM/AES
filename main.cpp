/* ==================================================
FileName:	main.cpp
Author:		SungJoon Lee
StudentID:	12141588
Created:	2016/10/25 Tue
Modified:	2016/10/28 Fri
================================================== */

#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>

#include "aes.h"

int main(int argc, char *argv[]) {
	FILE *fp;
	ByteList plain, key;
	Byte buf[1];

	if (argc != 3) {
		perror("usage: aes <plain> <key>");
		exit(1);
	}
	
	/* Read plain.bin */
	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		perror("open plain file error");
		exit(1);
	}
	while (fread(buf, sizeof(Byte), 1, fp))
		plain.push_back(buf[0]);
	fclose(fp);

	/* Read key.bin */
	fp = fopen(argv[2], "r");
	if (fp == NULL) {
		perror("open key file error");
		exit(1);
	}
	while (fread(buf, sizeof(Byte), 1, fp))
		key.push_back(buf[0]);
	fclose(fp);

	AES aes;
	aes.set_polynominal(0x65);  /* x^8 + x^6 + x^5 + x^2 + 1
									If not set this explicitly, 
									polynominal will be set with standard one. */
	aes.PrintLog(true);  // Print logs step by step. 
	printf("RC:"			"\t%s\n", ToString(aes.get_rc()));
	printf("PLAIN:"			"\t%s\n", ToString(plain));
	printf("KEY:"			"\t%s\n", ToString(key));
	ByteList cipher = aes.Encrypt(plain, key);
	printf("CIPHER:"		"\t%s\n", ToString(cipher));
	ByteList decripted = aes.Decrypt(cipher, key);
	printf("DECRIPTED:"		" %s\n", ToString(decripted));

	return 0;
}
