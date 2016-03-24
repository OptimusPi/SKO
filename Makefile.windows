CXXFLAGS=-c -Wall -g
LDLIBS=-lpthreadGC2 -lopengl32 -lmingw32 -lSDLmain -lSDL -lSDL_image -lSDL_mixer -lSDL_net
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
