compile: simple-webserver my-webclient my-webserver

URL=https://www.se.cs.titech.ac.jp/lecture/sysprog/5-socket

download: simple-webserver.c my-webclient.c my-webserver.c index.html report.txt

simple-webserver.c:
	curl $(URL)/$@ > $@
my-webclient.c:
	curl $(URL)/$@ > $@
my-webserver.c:
	curl $(URL)/$@ > $@
index.html:
	curl $(URL)/res/$@ > $@
report.txt:
	curl $(URL)/$@ > $@


simple-webserver: simple-webserver.c
	gcc -g -Wall $< -o $@

my-webclient: my-webclient.c
	gcc -g -Wall $< -o $@

my-webserver: my-webserver.c
	gcc -g -Wall $< -o $@

clean:
	-rm simple-webserver my-webclient my-webserver

zip: report.zip
report.zip: my-webclient.c my-webserver.c report.txt
	zip $@ $+
