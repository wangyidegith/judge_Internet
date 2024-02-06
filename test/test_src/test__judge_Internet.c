#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "../../src/judge_Internet.h"


int main(int argc, char* argv[]) {
	bool flag;
	if (argc == 1) {
		flag = judge_Internet(NULL);
		if (flag == true) {
			printf("Host has Internet.\n");
		} else {
			printf("Host no Internet.\n");
		}
	} else if (argc == 2) {
		flag = judge_Internet(argv[1]);
		if (flag == true) {
			printf("Proxy has Internet.\n");
		} else {
			printf("Proxy no Internet.\n");
		}
	} else {
		fprintf(stderr, "Err: too many arguments.");
	}
	return 0;
}
