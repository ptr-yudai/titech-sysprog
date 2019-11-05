SRCS?=mysh-log.c mysh-node.c mysh-main.c mysh.c fork0.c
CC_SRCS=mysh-scan.l mysh-parse.y
GEN_SRCS=lex.yy.c y.tab.c
ALL_SRCS?=$(SRCS) $(GEN_SRCS)
HDRS=mysh.h y.tab.h
OBJS=$(ALL_SRCS:%.c=%.o)
EXES=mysh fork0
REPORTS=$(ALL_SRCS) $(HDRS) report.txt

CC=gcc
CFLAGS=-std=gnu99 -Werror
LDFLAGS=
RM=rm

URL=https://www.se.cs.titech.ac.jp/lecture/sysprog/3-proc
DLS=$(ALL_SRCS) $(CC_SRCS) $(HDRS) report.txt
URLS=$(DLS:%=$(URL)/%)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: all clean allclean zip download

all: $(EXES)

lex.yy.c: mysh-scan.l
	flex -o $@ $<

y.tab.c: mysh-parse.y
	bison -t -v -d -o $@ $<

mysh.o: mysh.h

mysh-log.o: mysh.h

mysh-node.o: mysh.h

mysh-main.o: mysh.h

lex.yy.o: y.tab.c y.tab.h mysh.h

y.tab.o: mysh.h y.tab.h

mysh: mysh.o mysh-main.o mysh-log.o mysh-node.o lex.yy.o y.tab.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

fork0: fork0.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

download:
	curl --remote-name-all $(URLS)
	touch $(GEN_SRCS)

report.zip: $(REPORTS)
	zip $@ $+

clean:
	-$(RM) $(EXES)
	-$(RM) $(OBJS)

allclean: clean
	-$(RM) *.o *~ a.out

test:
	true
