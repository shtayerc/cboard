PREFIX?=/usr
BINDIR?=$(PREFIX)/bin
SHAREDIR?=$(PREFIX)/share/cboard
MANDIR?=$(PREFIX)/share/man
APPDIR?=$(PREFIX)/share/applications

CC?=clang
FILES=*.c
EXE=cboard
LIBS=-lSDL2 -lSDL2_image -lSDL2_ttf -lm
WINLIBS=-lmingw32 -lSDL2main
FLAGS=-Wextra -Wall -Wformat-security
DESKTOPFILE=../cboard.desktop

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

desktop:
	@echo "[Desktop Entry]" > $(DESKTOPFILE)
	@echo "Encoding=UTF-8" >> $(DESKTOPFILE)
	@echo "Version=1.0" >> $(DESKTOPFILE)
	@echo "Type=Application" >> $(DESKTOPFILE)
	@echo "Terminal=false" >> $(DESKTOPFILE)
	@echo "Exec=$(PREFIX)/bin/$(EXE) %f" >> $(DESKTOPFILE)
	@echo "Name=Cboard" >> $(DESKTOPFILE)
	@echo "MimeType=application/x-chess-pgn;" >> $(DESKTOPFILE)
	@echo "Categories=Game;BoardGame;" >> $(DESKTOPFILE)
	@echo "Icon=$(PREFIX)/share/cboard/alpha/wp.png" >> $(DESKTOPFILE)

clean:
	rm -f $(EXE) $(OBJECTS) $(EXE)-64bit.exe $(EXE)-32bit.exe $(DESKTOPFILE)

check: debug
check:
	valgrind -q --leak-check=full --track-origins=yes ./$(EXE)

install: cboard desktop
install:
	mkdir -m 755 -p $(DESTDIR)$(SHAREDIR) $(DESTDIR)$(BINDIR) $(DESTDIR)$(MANDIR)/man1
	install -m 755 cboard $(DESTDIR)$(BINDIR)/cboard
	install -m 644 ../cboard.1 $(DESTDIR)$(MANDIR)/man1/cboard.1
	cp -R ../resources/* $(DESTDIR)$(SHAREDIR)
	@if [ "$(shell uname)" = "Linux" ]; then \
		install -m 644 ../cboard.desktop $(DESTDIR)$(APPDIR)/cboard.desktop; \
	fi

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/cboard
	rm -rf $(DESTDIR)$(SHAREDIR)
	rm -f $(DESTDIR)$(APPDIR)/cboard.desktop
