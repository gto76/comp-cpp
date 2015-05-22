#include <stdio.h>
#include <iostream>
#include <vector>

using namespace std;

const int WORD_SIZE = 8;
const int ADDR_SIZE = 4;
//const int ADDR_LEN = ADDR_SIZE*ADDR_SIZE-1;
const int ADDR_LEN = 15;


class Ram {
	public: 
		vector<vector<bool>> state = vector<vector<bool>>(ADDR_LEN, vector<bool>(WORD_SIZE)); 

		vector<bool> get(vector<bool> adr) {
			int address = getInt(adr);
			//bool wordOut[WORD_SIZE];
			vector<bool> wordOut(WORD_SIZE);
			for (int i = 0; i < WORD_SIZE; i++) {
				wordOut[i] = state[address][i];
			}
			return wordOut;
		}

		void set(vector<bool> adr, vector<bool> wordIn) {
			int address = getInt(adr);
			for (int i = 0; i < WORD_SIZE; i++) {
				state[address][i] = wordIn[i];
			}
		}


	private:

		int getInt(vector<bool> bIn) {
			int sum = 0;
			int power = 1;
			for (int i = ADDR_SIZE-1; i >= 0; i--, power *= 2) {
				if (bIn[i] == true) {
					sum += power;
				}
			}
			return sum;
		}
};


int main(int argc, const char* argv[]) {
	Ram ram;
	ram.set({false, false, false, false}, {true, true, true, true, true, true, true, true});
	vector<bool> word = ram.get({false, false, false, false});
	cout << "Value of first bit of first word is " << word[0] << "\n";
}

