# Makefile for compiling on linux using mingw.
# TODO: Make linux configuration

win/mcchatlog.exe: win/main.o win/lex.o win/config.o win/chatline.o win/data.o win/tools.o win/grid.o
	i686-w64-mingw32-g++ win/main.o win/lex.o win/config.o win/chatline.o win/data.o win/tools.o win/grid.o -g -o win/mcchatlog.exe `/usr/local/i686-w64-mingw32/bin/wx-config --libs` -static

win/config.o: config.cpp *.h
	i686-w64-mingw32-g++ config.cpp -c -g -o win/config.o `/usr/local/i686-w64-mingw32/bin/wx-config --cflags` -std=c++0x

win/main.o: main.cpp *.h 
	i686-w64-mingw32-g++ main.cpp -c -g -o win/main.o `/usr/local/i686-w64-mingw32/bin/wx-config --cflags` -std=c++0x

win/tools.o: tools.cpp *.h 
	i686-w64-mingw32-g++ tools.cpp -c -g -o win/tools.o `/usr/local/i686-w64-mingw32/bin/wx-config --cflags` -std=c++0x

win/grid.o: grid.cpp *.h 
	i686-w64-mingw32-g++ grid.cpp -c -g -o win/grid.o `/usr/local/i686-w64-mingw32/bin/wx-config --cflags` -std=c++0x

win/chatline.o: chatline.cpp *.h
	i686-w64-mingw32-g++ chatline.cpp -c -g -o win/chatline.o `/usr/local/i686-w64-mingw32/bin/wx-config --cflags`

win/data.o: data.cpp *.h
	i686-w64-mingw32-g++ data.cpp -c -g -o win/data.o `/usr/local/i686-w64-mingw32/bin/wx-config --cflags` -std=c++0x

win/lex.o: lex.yy.cc
	i686-w64-mingw32-g++ lex.yy.cc -c -g -o win/lex.o -I.

lex.yy.cc: config.flex
	flex++ config.flex

wclean:
	rm -Rf win/*
