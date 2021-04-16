# See LICENSE file for copyright and license details.

include config.mk

SRC = steg.c png_io.c
OBJ = ${SRC:.c=.o}

all: options steg

options:
	@echo steg build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

steg: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f steg ${OBJ}

install: all
	mkdir -p ${PREFIX}/bin
	cp -f steg ${PREFIX}/bin
	chmod 755 ${PREFIX}/bin/steg

uninstall:
	rm -f ${PREFIX}/bin/steg

.PHONY: all options clean install uninstall
