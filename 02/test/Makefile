SRCS?=main.c mycat.c
HDRS=main.h mycat.h
OBJS=$(SRCS:%.c=%.o)
EXES=mycat
GENS=large.bin small1.bin small2.bin small3.bin small4.bin small-all.bin
REPORTS=$(SRCS) $(HDRS) report.txt

CC=gcc
CFLAGS=-std=gnu99 -pedantic -Wall -Werror
LDFLAGS=
RM=rm -f
TIME=/usr/bin/time

SIZE_10MB=10485760
SIZE_10MBP=10485764
SIZE_1GB=1073741824

URL=https://www.se.cs.titech.ac.jp/lecture/sysprog/2-io
DLS=$(SRCS) $(HDRS) test-cmp.sh report.txt
URLS=$(DLS:%=$(URL)/%)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: all clean genclean allclean zip download generate

all: $(EXES)

main.o: $(HDRS)

logger.o: $(HDRS)

mycat.o: $(HDRS)

mycat: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

download:
	curl --remote-name-all $(URLS)

generate: $(GENS)

large.bin:
	cat /dev/random | base64 | head -c $(SIZE_1GB) > $@
small1.bin:
	cat /dev/random | base64 | head -c $(SIZE_10MB) > $@
small2.bin:
	cat /dev/random | base64 | head -c $(SIZE_10MB) > $@
small3.bin:
	cat /dev/random | base64 | head -c $(SIZE_10MB) > $@
small4.bin:
	cat /dev/random | base64 | head -c $(SIZE_10MBP) > $@
small-all.bin: small1.bin small2.bin small3.bin
	cat $+ > $@

report.zip: $(REPORTS)
	zip $@ $+

time-syscall.txt: $(EXES) large.bin
	-rm $@
	$(TIME) ./mycat -q   -s 4194304 large.bin >/dev/null 2>>$@
	$(TIME) ./mycat -q   -s  524288 large.bin >/dev/null 2>>$@
	$(TIME) ./mycat -q   -s   65536 large.bin >/dev/null 2>>$@
	$(TIME) ./mycat -q   -s    8192 large.bin >/dev/null 2>>$@
	$(TIME) ./mycat -q   -s    1024 large.bin >/dev/null 2>>$@
	$(TIME) ./mycat -q   -s     128 large.bin >/dev/null 2>>$@
	$(TIME) ./mycat -q   -s      16 large.bin >/dev/null 2>>$@

time-stdio.txt: $(EXES) large.bin
	-rm $@
	$(TIME) ./mycat -q -l -s 4194304 large.bin >/dev/null 2>>$@
	$(TIME) ./mycat -q -l -s  524288 large.bin >/dev/null 2>>$@
	$(TIME) ./mycat -q -l -s   65536 large.bin >/dev/null 2>>$@
	$(TIME) ./mycat -q -l -s    8192 large.bin >/dev/null 2>>$@
	$(TIME) ./mycat -q -l -s    1024 large.bin >/dev/null 2>>$@
	$(TIME) ./mycat -q -l -s     128 large.bin >/dev/null 2>>$@
	$(TIME) ./mycat -q -l -s      16 large.bin >/dev/null 2>>$@

clean:
	-$(RM) $(EXES)
	-$(RM) $(OBJS)

genclean:
	-$(RM) $(GENS)

allclean: clean genclean
	-$(RM) *.o *~ a.out
	-$(RM) time-syscall.txt time-stdio.txt

test: test1 test2 testA

test1: small1.bin small4.bin
	./mycat -q small1.bin > tmp.bin
	@sh test-cmp.sh test1 tmp.bin small1.bin
	./mycat -q small4.bin > tmp.bin
	@sh test-cmp.sh test1-p tmp.bin small4.bin

test3: small1.bin small4.bin
	./mycat -q -l small1.bin > tmp.bin
	@sh test-cmp.sh test3 tmp.bin small1.bin
	./mycat -q -l small4.bin > tmp.bin
	@sh test-cmp.sh test3-p tmp.bin small4.bin

testA: small1.bin small2.bin small3.bin
	./mycat -q small1.bin small2.bin small3.bin > tmp.bin
	@sh test-cmp.sh testA-multi tmp.bin small-all.bin
	./mycat -q -l small1.bin small2.bin small3.bin > tmp.bin
	@sh test-cmp.sh testA-multi-stdio tmp.bin small-all.bin
	./mycat -q < small1.bin > tmp.bin
	@sh test-cmp.sh testA-stdin tmp.bin small1.bin
	./mycat -q -l < small1.bin > tmp.bin
	@sh test-cmp.sh testA-stdin-stdio tmp.bin small1.bin
	./mycat -q small1.bin - small3.bin < small2.bin > tmp.bin
	@sh test-cmp.sh testA-mixed tmp.bin small-all.bin
	./mycat -q -l small1.bin - small3.bin < small2.bin > tmp.bin
	@sh test-cmp.sh testA-mixed-stdio tmp.bin small-all.bin
