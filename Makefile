include config.mk

.POSIX:
.SUFFIXES: .c .o

HDR = fs.h text.h util.h arg.h
LIB = \
	util/afgets.o    \
	util/agetcwd.o   \
	util/apathmax.o  \
	util/concat.o    \
	util/cp.o        \
	util/enmasse.o   \
	util/eprintf.o   \
	util/estrtol.o   \
	util/fnck.o      \
	util/getlines.o  \
	util/putword.o   \
	util/recurse.o   \
	util/rm.o

MAN = \
	basename.1 \
	cat.1      \
	chmod.1    \
	chown.1    \
	chroot.8   \
	cksum.1    \
	cmp.1      \
	cp.1       \
	cut.1      \
	date.1     \
	dirname.1  \
	du.1       \
	echo.1     \
	false.1    \
	fold.1     \
	grep.1     \
	head.1     \
	kill.1     \
	ln.1       \
	ls.1       \
	mc.1       \
	mkdir.1    \
	mkfifo.1   \
	mv.1       \
	nl.1       \
	nohup.1    \
	pwd.1      \
	rm.1       \
	sleep.1    \
	sort.1     \
	tail.1     \
	tee.1      \
	touch.1    \
	true.1     \
	tty.1      \
	uname.1    \
	uniq.1     \
	seq.1      \
	wc.1       \
	yes.1

SRC = $(MAN:S/.1/.c/:S/.8/.c/)
OBJ = $(SRC:S/.c/.o/) $(LIB)
BIN = $(SRC:S/.c//)

all: $(BIN)

$(OBJ): util.h config.mk
$(BIN): util.a
cat.o fold.o grep.o nl.o sort.o tail.o uniq.o: text.h
cp.o mv.o rm.o: fs.h

.for x in $(BIN)
$x: $x.o
	@echo LD $@
	@$(LD) -o $@ $x.o util.a $(LDFLAGS)
.endfor

.c.o:
	@echo CC $<
	@$(CC) -c -o $@ $< $(CFLAGS)

util.a: $(LIB)
	@echo AR $@
	@$(AR) -r -c $@ $(LIB)
	@ranlib $@

install: all
	@echo installing executables to $(DESTDIR)$(PREFIX)/bin
	@mkdir -p $(DESTDIR)$(PREFIX)/bin
	@cp $(BIN) $(DESTDIR)$(PREFIX)/bin
	@(cd $(DESTDIR)$(PREFIX)/bin && chmod 755 $(BIN))
	@echo installing manual pages to $(DESTDIR)$(MANPREFIX)/man1
	@mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	@cp $(MAN) $(DESTDIR)$(MANPREFIX)/man1
	@(cd $(DESTDIR)$(MANPREFIX)/man1 && chmod 644 $(MAN))

uninstall:
	@echo removing executables from $(DESTDIR)$(PREFIX)/bin
	@(cd $(DESTDIR)$(PREFIX)/bin && rm -f $(BIN))
	@echo removing manual pages from $(DESTDIR)$(MANPREFIX)/man1
	@(cd $(DESTDIR)$(MANPREFIX)/man1 && rm -f $(MAN))

dist: clean
	@echo creating dist tarball
	@mkdir -p sbase-$(VERSION)
	@cp -r LICENSE Makefile config.mk $(SRC) $(MAN) util $(HDR) sbase-$(VERSION)
	@tar -cf sbase-$(VERSION).tar sbase-$(VERSION)
	@gzip sbase-$(VERSION).tar
	@rm -rf sbase-$(VERSION)

sbase-box: $(SRC) util.a
	@echo creating box binary
	@mkdir -p build
	@cp $(HDR) build
	@for f in $(SRC); do sed "s/^main(/`basename $$f .c`_&/" < $$f > build/$$f; done
	@echo '#include <libgen.h>'  > build/$@.c
	@echo '#include <stdlib.h>' >> build/$@.c
	@echo '#include <string.h>' >> build/$@.c
	@echo '#include "util.h"'   >> build/$@.c
	@for f in $(SRC); do echo "int `basename $$f .c`_main(int, char **);" >> build/$@.c; done
	@echo 'int main(int argc, char *argv[]) { char *s = basename(argv[0]); if(0) ;' >> build/$@.c
	@for f in $(SRC); do echo "else if(!strcmp(s, \"`basename $$f .c`\")) `basename $$f .c`_main(argc, argv);" >> build/$@.c; done
	@printf 'else eprintf("%%s: unknown program\\n", s); return EXIT_SUCCESS; }\n' >> build/$@.c
	@echo LD $@
	@$(LD) -o $@ build/*.c util.a $(CFLAGS) $(LDFLAGS)
	@rm -r build

clean:
	@echo cleaning
	@rm -f $(BIN) $(OBJ) $(LIB) util.a sbase-box
