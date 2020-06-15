CXX=g++
CXXFLAGS=-c -Wall -g -D_GLIBCXX_USE_NANOSLEEP -std=c++11 -O3
LDLIBS=-lpthread -lSDL -lSDL_image -lSDL_mixer -lSDL_net -largon2 -lSDLmain -framework OpenGL -framework Cocoa
SOURCES:=$(wildcard *.cpp)
OBJECTS:=$(patsubst %.cpp,%.o,$(SOURCES))
EXECUTABLE=Stick-Knights-Online

all : $(EXECUTABLE)

$(EXECUTABLE) : $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDLIBS)
	
$(OBJECTS) : %.o : %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean : 
	$(RM) $(OBJECTS) $(EXECUTABLE)
