#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {

	char c;
	while ( (c = getchar()) != EOF){
		putchar(c);
	}

	for (int i = 0; i < 5; i++) {
		int fd = open("/dev/null", O_RDONLY);
		printf("%d ", fd);
	}

	return 0;
}
