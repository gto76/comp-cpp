#ifndef UTIL_H
#define UTIL_H

#include <string>

using namespace std;

class Util {
	public:
		static int getInt(vector<bool> bbb);
		static vector<bool> getBoolByte(int num);
		static vector<bool> getBoolNibb(int num);
		static vector<bool> getFirstNibble(vector<bool> bbb);
		static vector<bool> getSecondNibble(vector<bool> bbb);
		static string getString(vector<bool> bbb);
		static vector<string> splitString(string stringIn);
		static string makeString(vector<string> lines);
		static int hexToInt(char cIn);

	private:
		static vector<bool> getBool(int num, int length);
		static char getChar(bool b);
};

#endif