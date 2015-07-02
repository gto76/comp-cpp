Comp Mark I
===========

Simple 4-bit/1 Hz virtual computer for learning purposes

![screenshot](doc/screenshot.png)

Memory
------
```
instructions - 4 bits
  |  +-- addresses - 4 bits
  v  v
----***-  <- 0  ----
--*-**-*  <- 1  ---*
---***--  <- 2  --*-
---*****  <- 3  --**
----***-  <- 4  -*--
---***-*  <- 5  -*-*
----**--  <- 6  -**-
---****-  <- 7  -***
-***---*  <- 8  *---
-*--****  <- 9  *--*
--------  <- 10 *-*-
--------  <- 11 *-**
--------  <- 12 **--
-------*  <- 13 **-*
-------*  <- 14 ***-
<OUTPUT>  <- 15 ****
```

* Execution starts at the first address (0). 
* Execution stops when program reaches last address (15).
* Whatever gets written to the last address is sent to the printer.
* Computer has one 8 bit register.
* When reading from the last address (15), we get a random byte value.

Instruction set
---------------

* `READ     ----` - Copies the value at the specified address into register.  
* `WRITE    ---*` - Copies value of the register to the specified address.  
* `ADD      --*-` - Adds value at the specified address to the value of the register, and writes result to the register. If the result is bigger than the maximum possible value (255 = ********) then 255 gets written.  
* `SUBTRACT --**` - Subtracts value at the speicfied address from the value of the register, and writes result to the register. If the result is smaller than 0 then 0 gets written.  
* `JUMP     -*--`  - Changes the value of the program counter to the specified address, meaning that in the next cycle execution will continue at that address.  
* `IF MAX   -*-*` - Jumps to the specified address if register has value 255 = `********`. 
* `IF MIN   -**-` - Jumps to the specified address if register has value 0 = `--------`. 
* `SHIFT R -***` - Moves every bit of the register one spot to the right. This way rightmost bit gets lost, and a leftmost becomes '*-*'. This is the only instruction that doesn't use the address part, making the last four bits irrelevant.  
* Any instruction that is not defined above gets interpreted as `READ ----` instruction.

How to run on…
--------------

### Windows

* Install *Tiny Core Linux* on *VirtualBox* using this [**instructions**](https://github.com/gto76/my-linux-setup/tree/gh-pages/conf-files/tiny-core-linux).
* Run the *UNIX* commands.

### UNIX
```bash
git clone https://github.com/gto76/comp-cpp.git
cd comp-cpp
./run
```

Other versions
--------------
* [**Mark II**](https://github.com/gto76/comp-m2), a model with separate address space for code and data and with more instructions. Programs can be saved and loaded and it can run without the interface (instead of a printer it uses stdout) and input can be piped in.  
 <a href="https://github.com/gto76/comp-m2/"> <img src="https://github.com/gto76/comp-m2/raw/master/doc/screenshot.png" alt="screenshot" width=400"/></a>




