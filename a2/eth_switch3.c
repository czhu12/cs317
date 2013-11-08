#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	if(argc != 3){
		printf("Wrong number of arguments!\n");
		return 1;
	}
	char buffer[512];
	sprintf(buffer, "./eth_switch1 %s | ./eth_switch2 %s", argv[1], argv[2]);
	system((char *) buffer);
	return 0;
}
