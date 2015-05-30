SOURCES_CPP=$(wildcard src/*.cpp)
SOURCES_C=$(wildcard src/*.c)

OBJECTS=$(addprefix obj/,$(notdir $(SOURCES_CPP:.cpp=.o))) $(addprefix obj/,$(notdir $(SOURCES_C:.c=.o)))

EXECUTABLE=comp

all: $(SOURCES_CPP) $(SOURCES_C) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	g++ -o $@ $^

obj/%.o: src/%.cpp
	g++ -c -std=c++11 -o $@ $< 

obj/%.o: src/%.c
	gcc -c -std=gnu11 -g -Wall -O3 -o $@ $<

clean:
	rm obj/*.o $(EXECUTABLE)

