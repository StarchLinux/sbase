# sbase version
VERSION = 0.0

# paths
PREFIX = 
MANPREFIX = $(PREFIX)/share/man

#CC = gcc
#CC = musl-gcc
LD = $(CC)
CPPFLAGS = -D_POSIX_C_SOURCE=200112L -D_BSD_SOURCE
CFLAGS   = -g -ansi -Wall -pedantic $(CPPFLAGS)
LDFLAGS  = -g -lutf

#CC = tcc
#LD = $(CC)
#CPPFLAGS = -D_POSIX_C_SOURCE=200112L -D_BSD_SOURCE
#CFLAGS   = -Os -Wall $(CPPFLAGS)
#LDFLAGS  =
