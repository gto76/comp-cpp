CFLAGS=-std=gnu11 -g -O3
CPPFLAGS=-std=c++11

SOURCES_CPP=$(wildcard src/*.cpp) 
SOURCES_C=$(wildcard src/*.c)

OBJDIR=obj
OBJECTS=$(addprefix $(OBJDIR)/,$(notdir $(SOURCES_CPP:.cpp=.o))) $(addprefix $(OBJDIR)/,$(notdir $(SOURCES_C:.c=.o)))

EXECUTABLE=comp

all: $(OBJDIR) $(SOURCES_CPP) $(SOURCES_C) $(EXECUTABLE) 
    
$(EXECUTABLE): $(OBJECTS) 
	g++ -o $@ $^

# Including all .d files, that contain the make statements
# describing depencencies of a files based on #include
# statements. This is needed, so that changes to header files
# get noticed, and apropriate files that depend on them get
# recompiled. The dependencies get generated with compilers -MM
# option. Is it just me or is this way too complicated. I just
# want to compile a simple project for god sake, been
# copypasting bits and pieces for whole day now. Don't you event
# think about RTFMing me, I'll find where you live and burn your
# house down :)
-include $(OBJDIR)/*.d

$(OBJDIR)/%.o: src/%.cpp
	g++ -c $(CPPFLAGS) -o $@ $< 
	g++ -MM $(CPPFLAGS) -MT '$@' src/$*.cpp > $(OBJDIR)/$*.d

$(OBJDIR)/%.o: src/%.c
	gcc -c $(CFLAGS) -o $@ $<
	gcc -MM $(CFLAGS) -MT '$@' src/$*.c > $(OBJDIR)/$*.d

# Creates 'obj' directory if it doesent exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -f $(OBJDIR)/* $(EXECUTABLE)

# Convert a drawing textfile to a drawing.hpp, containing
# that textfile in a string constant.
src/drawing.hpp: src/resources/drawing
	sed '/Do not edit/q' src/drawing.hpp > /tmp/drawing.hpp
	printf "\nconst string drawing = {" >> /tmp/drawing.hpp 
	xxd -i src/resources/drawing | tail -n+2 | head -n-1 >> /tmp/drawing.hpp
	printf "\n#endif" >> /tmp/drawing.hpp
	mv -f /tmp/drawing.hpp src/drawing.hpp

