#ifndef CONST_H
#define CONST_H

#include <string>

using namespace std;

const int WORD_SIZE = 8;
const int ADDR_SIZE = 4;
const int RAM_SIZE = ADDR_SIZE*ADDR_SIZE-1;

// Does it wait for pressed key after every cycle or is it automatic
const bool AUTOMATIC = true;
// Miliseconds between cycles (if automatic)
const int FQ = 300;

#endif