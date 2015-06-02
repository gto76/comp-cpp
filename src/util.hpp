#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <tuple>

using namespace std;

class Util {
	public:
		static int getInt(vector<bool> bbb);
		static vector<bool> getBoolByte(int num);
		static vector<bool> getBoolNibb(int num);
		static vector<bool> getBoolByte(string sIn);
		static vector<bool> getFirstNibble(vector<bool> bbb);
		static vector<bool> getSecondNibble(vector<bool> bbb);
		static string getString(vector<bool> bbb);
		static vector<string> splitString(string stringIn);
		static string makeString(vector<string> lines);
		static int hexToInt(char cIn);
		static string getString(char cIn);
		static char getChar(bool b);
		static vector<vector<bool>> getRamFromString(string ramString);
		static vector<bool> getRandomWord();
		static tuple<int,int> getLocationOfFirstRamLightbulb();
		static int getKey();

	private:
		static vector<bool> getBool(int num, int length);
};

#endif