PREFIX?=/usr
BINDIR?=$(PREFIX)/bin
SHAREDIR?=$(PREFIX)/share/cboard

CC=cc
FILES=SDL_FontCache.c window_data.c chessboard.c notation.c nag.c status.c machine.c textedit.c game_list.c main.c
EXE=cboard
LIBS=-lSDL2 -lSDL2_image -lSDL2_ttf
FLAGS=-Wextra -Wall -Wformat-security -Wno-format-truncation

OBJECTS=$(FILES:.c=.o)
all:
	$(CC) $(FLAGS) -c $(FILES)
	$(CC) -o $(EXE) $(OBJECTS) $(LIBS)

debug: FLAGS += -g
debug: all

clean:
	rm -f $(EXE) $(OBJECTS)

check: debug
check:
	valgrind -q --leak-check=full --track-origins=yes ./$(EXE)

install:
	mkdir -m 755 -p $(DESTDIR)$(SHAREDIR) $(DESTDIR)$(BINDIR)
	install -m 755 cboard $(DESTDIR)$(BINDIR)/cboard
	cp -R --preserve=mode resources/* $(DESTDIR)$(SHAREDIR)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/cboard
	rm -rf $(DESTDIR)$(SHAREDIR)
