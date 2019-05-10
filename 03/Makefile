SRCS?=mysh-log.c mysh-node.c mysh-main.c mysh.c fork0.c
CC_SRCS=mysh-scan.l mysh-parse.y
GEN_SRCS=lex.yy.c y.tab.c
ALL_SRCS?=$(SRCS) $(GEN_SRCS)
HDRS=mysh.h y.tab.h
OBJS=$(ALL_SRCS:%.c=%.o)
EXES=mysh fork0
REPORTS=$(ALL_SRCS) $(HDRS) report.txt

CC=gcc
CFLAGS=-std=gnu99 -Wall -Werror
LDFLAGS=
RM=rm

URL=https://www.se.cs.titech.ac.jp/lecture/sysprog/3-proc
DLS=$(ALL_SRCS) $(CC_SRCS) $(HDRS) report.txt
TESTS=t/test2-echo t/test3-fd t/test3-pipe t/test4-fd t/test4-words t/test5-append t/test5-fd-append t/test5-fd-stdin t/test5-fd-stdout t/test5-multi t/test5-stdin t/test5-stdout t/testA-echo t/testA-fd t/testB-and t/testB-or t/testC
DLS2=fd.c fd2.c test-cmp.sh test-cmp-all.sh
URLS=$(DLS:%=$(URL)/%)
URLS2=$(DLS2:%=$(URL)/%)
URLS_T=$(TESTS:%=$(URL)/%)

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

download2:
	mkdir -p t
	curl --remote-name-all $(URLS2)
	(cd t && curl --remote-name-all $(URLS_T))

	$(CC) $(CFLAGS) $(LDFLAGS) fd.c -o fd
	$(CC) $(CFLAGS) $(LDFLAGS) fd2.c -o fd2

report.zip: $(REPORTS)
	zip $@ $+

clean:
	-$(RM) $(EXES)
	-$(RM) $(OBJS)

allclean: clean
	-$(RM) *.o *~ a.out

test:
	true
