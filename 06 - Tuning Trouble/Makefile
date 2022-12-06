SRC=tune.cc

CPPFLAGS=-I../common
CXXFLAGS=-std=c++20 -O3 -flto -Wall -Wextra -Wpedantic -Wconversion -Wshadow  -g -ggdb
LDLIBS=-lfmt

OBJ=$(SRC:.cc=.o)
LINK.o=$(LINK.cc)
TARGET=$(SRC:.cc=)

all: $(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

Makefile.deps: $(SRC) Makefile
	$(CXX) $(CPPFLAGS) -MM $(SRC) >$@

include Makefile.deps
