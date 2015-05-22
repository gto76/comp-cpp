#include <stdio.h>
#include <iostream>
#include <vector>

#include "const.hpp"
#include "util.h"

using namespace std;

// const int WORD_SIZE = 8;
// const int ADDR_SIZE = 4;
// const int ADDR_LEN = ADDR_SIZE*ADDR_SIZE-1;

class Ram {
	public: 
		vector<bool> get(vector<bool> adr) {
			int address = Util::getInt(adr);
			vector<bool> wordOut(WORD_SIZE);
			for (int i = 0; i < WORD_SIZE; i++) {
				wordOut[i] = state[address][i];
			}
			return wordOut;
		}

		void set(vector<bool> adr, vector<bool> wordIn) {
			int address = Util::getInt(adr);
			for (int i = 0; i < WORD_SIZE; i++) {
				state[address][i] = wordIn[i];
			}
		}

	private:
		vector<vector<bool>> state = vector<vector<bool>>(ADDR_LEN, vector<bool>(WORD_SIZE)); 


};


int main(int argc, const char* argv[]) {
	Ram ram;
	ram.set({false, false, false, false}, {true, true, true, true, true, true, true, true});
	vector<bool> word = ram.get({false, false, false, false});
	cout << "Value of first bit of first word is " << word[0] << "\n";
}


