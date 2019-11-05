#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {

	for (int i = 0; i < 5; i++) {
		int fd = open("/dev/null", O_RDONLY);
		printf("%d ", fd);
	}
	return 0;
}
