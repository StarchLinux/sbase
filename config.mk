# sbase version
VERSION = 0.0

# paths
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

#CC = gcc
#CC = musl-gcc
LD = $(CC)
CPPFLAGS = -D_POSIX_C_SOURCE=200112L
CFLAGS   = -Os -ansi -Wall -Wextra -pedantic $(CPPFLAGS)
LDFLAGS  = -static #-s

#CC = tcc
#LD = $(CC)
#CPPFLAGS = -D_POSIX_C_SOURCE=200112L
#CFLAGS   = -Os -Wall $(CPPFLAGS)
#LDFLAGS  =
