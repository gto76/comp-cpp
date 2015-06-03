#ifndef RAM_H
#define RAM_H

#include <vector>

#include "const.hpp"

using namespace std;

class Ram {
	public:
		vector<vector<bool>> state = vector<vector<bool>>(RAM_SIZE, vector<bool>(WORD_SIZE));
		vector<bool> get(vector<bool> adr);
		void set(vector<bool> adr, vector<bool> wordIn);
};

#endif