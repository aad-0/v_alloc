#-*- MakeFile -*-
all = main

CC  = gcc

CFLAGS = --std=c99 -Ofast -Wextra -Wall -Werror -Wpedantic
# -g -Og -ggdb 

main: main.o valloc.o
	@echo "Go get them tiger!"
	$(CC) main.o valloc.o -o main

%.o: %.c
	@echo "getting *.o"
	$(CC) -c $(CFLAGS) $< -o $@

main.o: main.c
	@echo "getting main.o"
	$(CC) $(CFLAGS) -c main.c

test:
	./main
	@echo "ERRCode: $$?" %errorlevel%

gdb:
	@echo "You Lost SOUL !"
	gdb ./main

clean:
	rm -rf ./*.o ./main


