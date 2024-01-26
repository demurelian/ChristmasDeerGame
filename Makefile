all:
	g++ -o game Application.cpp -lSDL2 -lSDL2_mixer -lSDL2_ttf -lSDL2_image && ./game
