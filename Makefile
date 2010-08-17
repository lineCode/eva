CC = gcc
CPP = g++
JAVA = java
C_FLAGS = -I . -mno-cygwin  -Os -fdata-sections -ffunction-sections  -Wl,--gc-sections -g0
CFLAGS = -I /usr/local/include/ $(C_FLAGS)
CPPFLAGS = $(C_FLAGS)

WX_STC_INCLUDE_DIR = -I /usr/local/include/wx-2.8/ -I /cygdrive/c/wxWidgets-2.8.11/contrib/include -I /cygdrive/c/wxWidgets-2.8.11/contrib/src/stc/scintilla/include -I /cygdrive/c/wxWidgets-2.8.11/contrib/src/stc/scintilla/src
WX_STC_LIB_DIR = -L /cygdrive/c/wxWidgets-2.8.11/lib/
# Produced using `wx-config --shared --cxxflags --libs `
#WXFLAGS = -I/usr/local/lib/wx/include/msw-ansi-debug-static-2.8 -I/usr/local/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXDEBUG__ -D__WXMSW__ \
#-L/usr/local/lib  /usr/local/lib/libwx_mswd_core-2.8.a /usr/local/lib/libwx_based-2.8.a  -loleaut32 -lole32 -luuid -lwinspool -lwinmm -lshell32 -lcomctl32 -lcomdlg32 -lctl3d32 -ladvapi32 -lwsock32 -lgdi32 -lkernel32 -luser32
WXFLAGS = -I/usr/local/lib/wx/include/msw-ansi-release-static-2.8 -I/usr/local/include/wx-2.8 -D__WXMSW__ -mthreads \
		-L/usr/local/lib -mno-cygwin -mwindows -mthreads  -mwindows -Wl,--subsystem,windows -mwindows /cygdrive/c/wxWidgets-2.8.11/lib/libwx_msw_stc-2.8.dll.a /usr/local/lib/libwx_msw_core-2.8.a -lwx_base-2.8 -lrpcrt4 -loleaut32 -lole32 -luuid -lwinspool -lwinmm -lshell32 -lcomctl32 -lcomdlg32 -lctl3d32 -ladvapi32 -lwsock32 -lgdi32

all: Eva.g EvaTree.g Expr.c Gui.cpp
	$(JAVA) org.antlr.Tool Eva.g EvaTree.g
	$(CPP) $(CFLAGS) -c EvaLexer.c EvaParser.c EvaTree.c Expr.c Util.c
	$(CPP) $(CPPFLAGS) $(WX_STC_INCLUDE_DIR) *.o -o Eva Gui.cpp  $(WXFLAGS) $(WX_STC_LIB_DIR) -lwx_msw_stc-2.8 -L /usr/local/lib -lantlr3c  -fdata-sections -ffunction-sections  -Wl,--gc-sections -g0
	strip --strip-all ./Eva

clean:
	rm -f *.o *.exe EvaLexer.h EvaLexer.tokens EvaLexer.c EvaParser.h EvaParser.tokens EvaParser.c Eva.h Eva.c Eva.tokens EvaTree.c EvaTree.h EvaTree.tokens
