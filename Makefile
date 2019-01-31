CC=g++
LIBS=-lGL -lGLEW -lSDL2 -lm
NAME=mandel
SOURCES=mandel.cpp resources.cpp lodepng.cpp imgui.cpp imgui_draw.cpp imgui_impl_sdl.cpp imgui_impl_opengl3.cpp
FLAGS=-I./imgui -I./imgui/examples
OBJS= $(addsuffix .o, $(basename $(notdir $(SOURCES))))

%.o:%.cpp
	$(CC) $(FLAGS) -c -o $@ $< $(LIBS)


%.o:imgui/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:imgui/examples/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -o $@ $^ $(FLAGS) $(LIBS)

clean:
	rm -f $(NAME) $(OBJS)