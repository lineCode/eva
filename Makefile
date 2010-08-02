CC = gcc
CPP = g++
JAVA = java
CFLAGS = -I /usr/local/include/ -I . 
CPPFLAGS = -I . 
# Produced using `wx-config --cxxflags --libs `
WXFLAGS = -I/usr/local/lib/wx/include/msw-ansi-debug-static-2.8 -I/usr/local/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXDEBUG__ -D__WXMSW__ \
-L/usr/local/lib  /usr/local/lib/libwx_mswd_core-2.8.a /usr/local/lib/libwx_based-2.8.a  -loleaut32 -lole32 -luuid -lwinspool -lwinmm -lshell32 -lcomctl32 -lcomdlg32 -lctl3d32 -ladvapi32 -lwsock32 -lgdi32 -lkernel32 -luser32


all: Eva.g EvaTree.g Expr.c Gui.cpp
	$(JAVA) org.antlr.Tool Eva.g EvaTree.g
	$(CPP) $(CFLAGS) -c EvaLexer.c EvaParser.c EvaTree.c Expr.c 
	$(CPP) $(CPPFLAGS) *.o -o Eva Gui.cpp  $(WXFLAGS) -L /usr/local/lib -lantlr3c 
	strip --strip-all ./Eva

clean:
	rm -f *.o *.exe EvaLexer.h EvaLexer.tokens EvaLexer.c EvaParser.h EvaParser.tokens EvaParser.c Eva.h Eva.c Eva.tokens EvaTree.c EvaTree.h EvaTree.tokens
