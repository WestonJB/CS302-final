CXX=		g++
CXXFLAGS=	-g -Wall -std=gnu++11
SHELL=		bash
SOURCES=	main.cpp Game.cpp Player.cpp
OBJECTS=	$(SOURCES:.cpp=.o)

all: bin/main

bin/main: obj/$(OBJECTS) 
	$(CXX) -o $@ $(OBJECTS)

obj/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $^

clean:
	rm -f bin/$(PROGRAMS) obj/$(OBJECTS)