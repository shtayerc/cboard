PREFIX?=/usr
BINDIR?=$(PREFIX)/bin
SHAREDIR?=$(PREFIX)/share/cboard
MANDIR?=$(PREFIX)/share/man

CC=cc
FILES=*.c
EXE=cboard
LIBS=-lSDL2 -lSDL2_image -lSDL2_ttf
WINLIBS=-lmingw32 -lSDL2main
FLAGS=-Wextra -Wall -Wformat-security -Wno-format-truncation

OBJECTS=$(FILES:.c=.o)
cboard: $(FILES) *.h
	$(CC) $(FLAGS) -c $(FILES) -D'PREFIX="$(PREFIX)"'
	$(CC) -o $(EXE) $(OBJECTS) $(LIBS)

win32: CC=i686-w64-mingw32-gcc
win32: EXE:=$(EXE)-32bit.exe
win32: LIBS:=$(WINLIBS) $(LIBS)
win32: cboard

win64: CC=x86_64-w64-mingw32-gcc
win64: EXE:=$(EXE)-64bit.exe
win64: LIBS:=$(WINLIBS) $(LIBS)
win64: cboard

debug: FLAGS += -g
debug: cboard

clean:
	rm -f $(EXE) $(OBJECTS)

check: debug
check:
	valgrind -q --leak-check=full --track-origins=yes ./$(EXE)

install: cboard
install:
	mkdir -m 755 -p $(DESTDIR)$(SHAREDIR) $(DESTDIR)$(BINDIR) $(DESTDIR)$(MANDIR)/man1
	install -m 755 cboard $(DESTDIR)$(BINDIR)/cboard
	install -m 644 cboard.1 $(DESTDIR)$(MANDIR)/man1/cboard.1
	cp -R --preserve=mode resources/* $(DESTDIR)$(SHAREDIR)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/cboard
	rm -rf $(DESTDIR)$(SHAREDIR)
