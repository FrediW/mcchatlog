# Makefile for compiling on linux using mingw.
# TODO: Make linux configuration

win/mcchatlog.exe: win/main.o win/lex.o win/config.o win/chatline.o win/data.o win/tools.o win/grid.o
	i686-w64-mingw32-g++ win/main.o win/lex.o win/config.o win/chatline.o win/data.o win/tools.o win/grid.o -g -o win/mcchatlog.exe `/usr/local/i686-w64-mingw32/bin/wx-config --libs` -static

bin/mcchatlog: bin/main.o bin/lex.o bin/config.o bin/chatline.o bin/data.o bin/tools.o bin/grid.o
	g++ bin/main.o bin/lex.o bin/config.o bin/chatline.o bin/data.o bin/tools.o bin/grid.o -g -o bin/mcchatlog `wx-config --libs`

bin/lex.o: lex.yy.cc
	g++ lex.yy.cc -c -g -o bin/lex.o -I.

win/lex.o: lex.yy.cc
	i686-w64-mingw32-g++ lex.yy.cc -c -g -o win/lex.o -I.

win/%.o: %.cpp *.h
	i686-w64-mingw32-g++ $< -c -g -o $@ `/usr/local/i686-w64-mingw32/bin/wx-config --cflags` -std=c++0x

bin/%.o: %.cpp *.h
	g++ $< -c -g -o $@ `wx-config --cflags` -std=c++0x

lex.yy.cc: config.flex
	flex++ config.flex

wclean:
	rm -f win/*

clean:
	rm -f bin/*