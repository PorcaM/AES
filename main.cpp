#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <vector>
using namespace std;

#include "aes.h"

int main(int argc, char *argv[]) {
	FILE *fp;

	if (argc != 3) {
		perror("usage: aes <plain> <key>");
		exit(1);
	}

	vector<Byte> plain, key;
	Byte buf[1];

	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		perror("open plain file error");
		exit(1);
	}
	while (fread(buf, sizeof(Byte), 1, fp)) {
		plain.push_back(buf[0]);
	}

	fp = fopen(argv[2], "r");
	if (fp == NULL) {
		perror("open key file error");
		exit(1);
	}
	while (fread(buf, sizeof(Byte), 1, fp)) {
		key.push_back(buf[0]);
	}

	AES aes;
	vector<Byte> chiper = aes.Encrypt(plain, key);
	vector<Byte> decripted = aes.Decrypt(chiper, key);

	return 0;
}