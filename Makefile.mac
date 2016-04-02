CXXFLAGS=-c -Wall -g
LDLIBS=-framework cocoa -framework OpenGL SDLMain.m -framework SDL -framework SDL_image -framework SDL_mixer -framework SDL_net
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
