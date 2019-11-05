compile: inst0 inst0-simple

URL=https://www.se.cs.titech.ac.jp/lecture/sysprog/6-cgtool

CC=gcc
CFLAGS=-g -Wall --std=gnu99 -finstrument-functions -no-pie -ldl -rdynamic

download: inst0.c inst0-simple.c cg.c gzip-1.2.4.tar.gz report.txt

inst0.c:
	curl $(URL)/$@ > $@
inst0-simple.c:
	curl $(URL)/$@ > $@
cg.c:
	curl $(URL)/$@ > $@
gzip-1.2.4.tar.gz:
	curl $(URL)/$@ > $@
report.txt:
	curl $(URL)/$@ > $@

inst0: inst0.c
	$(CC) $(CFLAGS) $< -o $@ -ldl
run-inst0: inst0
	./inst0

cg.o: cg.c
	$(CC) $(CFLAGS) -c $< -o $@ -ldl

inst0-simple.o: inst0-simple.c
	$(CC) $(CFLAGS) -c $< -o $@
inst0-simple: inst0-simple.o cg.o
	$(CC) $(CFLAGS) $+ -o $@ -ldl
run-inst0-simple: inst0-simple
	./inst0-simple
	dot -Tsvg cg.dot > inst0-simple.svg
run-inst0-simple-label: inst0-simple
	env SYSPROG_CG_LABEL=1 ./inst0-simple
	dot -Tsvg cg.dot > inst0-simple-label.svg

# gzip-1.2.4/configure: gzip-1.2.4.tar.gz
GZIP_NS=gzip zip deflate trees bits unzip inflate util crypt lzw unlzw unpack unlzh getopt
GZIP_OBJ_FILES=$(GZIP_NS:%=%.o)
GZIP_OBJS=$(GZIP_NS:%=gzip-1.2.4/%.o)
gzip-1.2.4/configure: gzip-1.2.4.tar.gz
	tar zxvf gzip-1.2.4.tar.gz
gzip-1.2.4/Makefile: gzip-1.2.4/configure
	(cd gzip-1.2.4 && env CC=$(CC) CFLAGS='$(CFLAGS)' ./configure)
gzip-1.2.4/gzip.o: gzip-1.2.4/Makefile
	(cd gzip-1.2.4 && make $(GZIP_OBJ_FILES))
mygzip: gzip-1.2.4/gzip.o cg.o
	$(CC) $(CFLAGS) $(GZIP_OBJS) cg.o -o $@ -ldl
run-mygzip: mygzip
	./mygzip -c inst0-simple.c > /dev/null
	dot -Tsvg cg.dot > mygzip.svg
run-mygzip-label: mygzip
	env SYSPROG_CG_LABEL=1 ./mygzip -c inst0-simple.c > /dev/null
	dot -Tsvg cg.dot > mygzip-label.svg

clean:
	-rm inst0 inst0-simple

zip: report.zip
report.zip: inst0.c cg.c report.txt
	zip $@ $+

