snake: src/snake.c src/main.c
	gcc -g -o bin/$@ $^ -lncurses

run: bin/snake
	./$^