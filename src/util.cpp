#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/stat.h>
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
	if (num <= 0) {
		return vector<bool>(length, false);
	}
	if (num >= (int) pow(2, length)-1) {
		return vector<bool>(length, true);
	}
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
		// '*' is interpreted as true, all other characters as false.
		out.at(i++) = c == '*';
		if (i == WORD_SIZE) {
			break;
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

string Util::getFormatedInt(vector<bool> wordIn) {
	char formatedInt [4];
	sprintf(formatedInt, "%3d", Util::getInt(wordIn));
	return formatedInt;
}

string Util::getStringWithFormatedInt(vector<bool> wordIn) {
	return Util::getString(wordIn) + " " + Util::getFormatedInt(wordIn) + "\n";
}

/*
 * GENERAL UTIL
 */

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
	printf("Could not find ram in drawing. Aborting");
	exit(1);
}

/*
 * Returns the code of last pressed key, or 0 if none was pressed.
 */
int Util::getKey() {
	int c = 0;
	struct timeval tv = { 0L, 0L };
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);

	if (FD_ISSET(STDIN_FILENO, &fds)) {
		c = getchar();
	}
	return c;
}

bool Util::fileExists(string filename) {
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1) {
        return true;
    }
    return false;
}

bool Util::inputIsPiped() {
	return !isatty(fileno(stdin));
}

bool Util::outputIsPiped() {
	return !isatty(fileno(stdout));
}

bool Util::startsWithDigit(string line) {
	if (line.empty()) {
		return false;
	}
	return isdigit(line[0]);
}

int Util::extractInteger(string line) {
	int i = 0;
	for (char c : line) {
		if (!isdigit(c)) {
			break;
		}
		i++;
	}
	string stringNumber = line.substr(0, i);
	stringstream ss;
	ss << stringNumber;
	int out;
	ss >> out;
	return out;
}