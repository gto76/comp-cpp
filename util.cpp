#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>

#include "const.hpp"
#include "util.hpp"

using namespace std;

int Util::getInt(vector<bool> bbb) {
	int sum = 0;
	int power = 1;
	for (int i = ADDR_SIZE-1; i >= 0; i--, power *= 2) {
		if (bbb[i] == true) {
			sum += power;
		}
	}
	return sum;
}

vector<bool> Util::getBoolByte(int num) {
	return getBool(num, 4);
}

vector<bool> Util::getBoolNibb(int num) {
	return getBool(num, 8);
}

vector<bool> Util::getBool(int num, int length) {
	vector<bool> out(length);
	int j = 0;
	for (int i = length-1; i >= 0; i--) {
		int divider = (int) pow(2, i);
		int res = num / divider;
		if (res > 0) {
			out[j++] = true;
		} else {
			out[j++] = false;
		}
		num = num % divider;
	}
	return out;	
}

vector<bool> Util::getFirstNibble(vector<bool> bbb) {
	return {bbb[0], bbb[1], bbb[2], bbb[3]};
}

vector<bool> Util::getSecondNibble(vector<bool> bbb) {
	return {bbb[4], bbb[5], bbb[6], bbb[7]};
}

string Util::getString(vector<bool> bbb) {
	string out = "";
	for (bool b : bbb) {
		out += Util::getChar(b);
	}
	return out;
}

char Util::getChar(bool b) {
	if (b) {
		return '*';
	} else { 
		return '-';
	}
}
