CC=gcc
LIBS=-lGL -lGLEW -lSDL2 -lm
NAME=mandel

make:
	rm -f $(NAME)
	$(CC) mandel.c -o $(NAME) $(LIBS)
	rm -f *.o