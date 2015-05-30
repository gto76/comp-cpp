SOURCES_CPP=comp.cpp renderer.cpp util.cpp
SOURCES_C=environment.c output.c
OBJECTS=$(SOURCES_CPP:.cpp=.o) $(SOURCES_C:.c=.o)

EXECUTABLE=a.out

all: $(SOURCES_CPP) $(SOURCES_C) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	g++ -o a.out *.o

%.o: %.cpp
	g++ -c -std=c++11 $< 

%.o: %.c
	gcc -c -std=gnu11 -g -Wall -O3 $<

clean:
	rm *.o a.out

