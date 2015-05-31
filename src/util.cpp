#include <stdio.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>

#include "const.hpp"
#include "util.hpp"
#include "drawing.hpp"

using namespace std;

int Util::getInt(vector<bool> bbb) {
	int sum = 0;
	int power = 1;
	for (int i = bbb.size()-1; i >= 0; i--, power *= 2) {
		if (bbb[i] == true) {
			sum += power;
		}
	}
	return sum;
}

vector<bool> Util::getBoolByte(int num) {
	return getBool(num, 8);
}

vector<bool> Util::getBoolNibb(int num) {
	return getBool(num, 4);
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

vector<bool> Util::getBoolByte(string sIn) {
	vector<bool> out(8);
	int i = 0;
	for (char c : sIn) {
		if (c == '-') {
			out.at(i++) = false;
		} else if (c == '*') {
			out.at(i++) = true;
		} else {
			cout << "Input Error 02 - Unrecognized char";
			exit(2);
		}
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

vector<string> Util::splitString(string stringIn) {
	vector<string> out;
	stringstream stream (stringIn);
	string line;
	while (getline(stream, line)) {
  		out.push_back(line);
	}
	return out;
}

string Util::makeString(vector<string> lines) {
	string out;
    for (string line : lines) {
        out += line;
    }
    return out;
}

int Util::hexToInt(char cIn) {
	unsigned int out;
	stringstream ss;
	ss << cIn;
	ss >> std::hex >> out;
	return out;
}

string Util::getString(char cIn) {
	stringstream ss;
	string s;
	ss << cIn;
	ss >> s;
	return s;
}

vector<vector<bool>> Util::getRamFromString(string ramString) {
    vector<vector<bool>> data = vector<vector<bool>>(RAM_SIZE, vector<bool>(WORD_SIZE));
    int i = 0;
    for (string line : splitString(ramString)) {
    	data[i++] = getBoolByte(line);
    }
    return data;
}

vector<bool> Util::getRandomWord() {
	vector<bool> wordOut(WORD_SIZE);
	for (int i = 0; i < WORD_SIZE; i++) {
		wordOut[i] = 0 == (rand() % 2);
	}
	return wordOut;
}

tuple<int,int> Util::getLocationOfFirstRamLightbulb() {
	int i = 0;
	int j = 0;
	for (char c : drawing) {
		if (c == '0') {
			return tuple<int,int>(i, j); 
		}
		if (c == '\n') {
			i = 0;
			j++;
		} else {
			i++;
		}
	}
}