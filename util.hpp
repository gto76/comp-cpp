#ifndef UTIL_H
#define UTIL_H

class Util {
	public:
		static int getInt(std::vector<bool> bbb);
		static std::vector<bool> getBoolByte(int num);
		static std::vector<bool> getBoolNibb(int num);
		static std::vector<bool> getFirstNibble(std::vector<bool> bbb);
		static std::vector<bool> getSecondNibble(std::vector<bool> bbb);
		static std::string getString(std::vector<bool> bbb);

	private:
		static std::vector<bool> getBool(int num, int length);
		static char getChar(bool b);
};

#endif