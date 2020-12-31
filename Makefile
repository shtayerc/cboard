PREFIX?=/usr
BINDIR?=$(PREFIX)/bin
SHAREDIR?=$(PREFIX)/share/cboard
MANDIR?=$(PREFIX)/share/man

CC=cc
FILES=*.c
EXE=cboard
LIBS=-lSDL2 -lSDL2_image -lSDL2_ttf
FLAGS=-Wextra -Wall -Wformat-security -Wno-format-truncation

OBJECTS=$(FILES:.c=.o)
all:
	$(CC) $(FLAGS) -c $(FILES) -DPREFIX=\"$(PREFIX)\"
	$(CC) -o $(EXE) $(OBJECTS) $(LIBS)

debug: FLAGS += -g
debug: all

clean:
	rm -f $(EXE) $(OBJECTS)

check: debug
check:
	valgrind -q --leak-check=full --track-origins=yes ./$(EXE)

install: all
install:
	mkdir -m 755 -p $(DESTDIR)$(SHAREDIR) $(DESTDIR)$(BINDIR) $(DESTDIR)$(MANDIR)/man1
	install -m 755 cboard $(DESTDIR)$(BINDIR)/cboard
	install -m 644 cboard.1 $(DESTDIR)$(MANDIR)/man1/cboard.1
	cp -R --preserve=mode resources/* $(DESTDIR)$(SHAREDIR)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/cboard
	rm -rf $(DESTDIR)$(SHAREDIR)
