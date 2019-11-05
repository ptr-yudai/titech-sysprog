SRCS?=mysh-log.c mysh-node.c mysh-main.c mysh2.c signal0.c count.c
CC_SRCS=mysh-scan.l mysh-parse.y
GEN_SRCS=lex.yy.c y.tab.c
ALL_SRCS?=$(SRCS) $(GEN_SRCS)
HDRS=mysh.h y.tab.h
OBJS=$(ALL_SRCS:%.c=%.o)
EXES=count signal0 mysh2
REPORTS=$(ALL_SRCS) $(HDRS) report.txt

CC=gcc
CFLAGS=-std=gnu99 -Wall -Werror
LDFLAGS=
RM=rm

URL=https://www.se.cs.titech.ac.jp/lecture/sysprog/4-signal
DLS=$(ALL_SRCS) $(CC_SRCS) $(HDRS) fire.txt report.txt
TESTS=t/test3 t/testA
ANS=t/ans/test3-ans.txt t/ans/testA-ans.txt
URLS=$(DLS:%=$(URL)/%)
TEST_URLS=$(TESTS:%=$(URL)/%)
ANS_URLS=$(ANS:%=$(URL)/%)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: all clean allclean zip download

all: $(EXES)

lex.yy.c: mysh-scan.l
	flex -o $@ $<

y.tab.c: mysh-parse.y
	bison -t -v -d -o $@ $<

mysh2.o: mysh.h

mysh-log.o: mysh.h

mysh-node.o: mysh.h

mysh-main.o: mysh.h

lex.yy.o: y.tab.c y.tab.h mysh.h

y.tab.o: mysh.h y.tab.h

mysh2: mysh2.o mysh-main.o mysh-log.o mysh-node.o lex.yy.o y.tab.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

signal0: signal0.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

download:
	curl --remote-name-all $(URLS)
	mkdir -p t/ans
	(cd t && curl --remote-name-all $(TEST_URLS))
	(cd t/ans && curl --remote-name-all $(ANS_URLS))

zip: report.zip

report.zip: $(REPORTS)
	zip $@ $+

clean:
	-$(RM) $(EXES)
	-$(RM) $(OBJS)

allclean: clean
	-$(RM) *.o *~ a.out

test:
	true
