CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -O2

SRC = main.cpp glad.c
OBJ = $(SRC:.cpp=.o)
OBJ := $(OBJ:.c=.o)

LIBS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl

main: $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LIBS)

exe:
	$(CXX) $(OBJ) -o $@.exe $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f *.o main
