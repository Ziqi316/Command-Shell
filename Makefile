CFLAGS= -Wall -Werror -std=gnu++03 -pedantic -ggdb3

myShell: myShell.cpp parse.cpp Shell.cpp buildInFunc.cpp
	g++ $(CFLAGS) -o myShell myShell.cpp parse.cpp Shell.cpp buildInFunc.cpp

clean:
	rm -f test *~

