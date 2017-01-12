CC = clang
CXX = clang++

CFLAGS = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall -std=c++11 $(INCLUDES)

LDFLAGS = -g 
LDLIBS = -lsqlite3

executables = classMap  
objects = classMap.o  pullcourses.o 

.PHONY: default
default: clean classMap

classMap: classMap.o pullcourses.o
	$(CXX) $(CXXFLAGS) -Wl,classMap.o,pullcourses.o -o classMap $(LDLIBS)

classMap.o:
	$(CXX) $(CXXFLAGS) -c classMap.cpp

pullcourses.o:
	$(CC) $(CLFAGS) -c pullcourses.c

.PHONY: clean
clean:
	rm -f a.out $(objects) $(executables)
