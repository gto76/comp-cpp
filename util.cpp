#include <stdio.h>
#include <iostream>
#include <vector>

#include "const.hpp"
#include "util.h"

using namespace std;

int Util::getInt(vector<bool> bIn) {
	int sum = 0;
	int power = 1;
	for (int i = ADDR_SIZE-1; i >= 0; i--, power *= 2) {
		if (bIn[i] == true) {
			sum += power;
		}
	}
	return sum;
}
