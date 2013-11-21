#include <string.h>
#include <stdio.h>
#include <time.h>

char * build_header(char * header_type, char * header_content){
	char * new_str;
	char * endToken = "\r\n";
	new_str = malloc(strlen(header_type) + strlen(header_content) + strlen(endToken) + 1);
	strcat(new_str, header_type);
	strcat(new_str, header_content);
	strcat(new_str, endToken);
	return new_str;
}

char * add_header(char * header_type, char * header_content, char * content){
	char * header = build_header(header_type, header_content);
	char * new_str;
	new_str = malloc(strlen(header) + strlen(content) + 1);
	strcat(new_str, header);
	strcat(new_str, content);
	return new_str;
}

int main(void) {
	char * test = "Date: ";
	char * test2 = "12 Nov";
	char * test3 = "\r\n";
	
	printf("%s", add_header("Date: ", "Nov 12 Sun", "whos there"));
	return 0;
}
