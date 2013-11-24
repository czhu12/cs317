#include <stdio.h>
#include <string.h>
#include <stdlib.h>
char * extract_path(char * path){
	char buffer[strlen(path)];
	strcpy(buffer, path);
	char * split = strtok(buffer, "=/?&");
	char * new_path = malloc(strlen(buffer));
	strcpy(new_path, split);
	return new_path;
}

int main(){
  char req[] = "Post http://www.example.com/test?name=value&name2=value2 HTTP/1.1\r\n"
      "Content-Length: 12345\r\n"
      "Host:   www.example.com  \r\n"
			"Cookie: hello=world \r\n"
      "Pragma: no-cache\r\n"
      "Accept-Encoding: text/plain\r\n"
      "\r\nTHIS_IS_THE_BODY";
	char * split = strtok(req, "\n\r");
	while(split != NULL){
	//	printf("%s\n", split);
		split = strtok(NULL, "\n\r");
		if(split && strstr(split, "Cookie: ")){
			char * cookie_split = strtok(split, ":= ");
			cookie_split = strtok(NULL, ":= ");
			while(cookie_split != NULL){
				printf("%s\n", cookie_split);
				cookie_split = strtok(NULL, ":= ");
			}
		}
	}
	//char testValue[] = "/test?name=value&name2=value2";
	//char testValue[] = "/test";
	//char * split = strtok(testValue, "=/?&");
	//while(split != NULL){
	//	//printf("%s\n", split);
	//	split = strtok(NULL, "=/?&");
	//}
	//printf("%s", extract_path("/knock"));
}

//#include <string.h>
//#include <stdio.h>
//
//#define LINE_MAX 100
//
//int main() {
//	char line[LINE_MAX];
//	char *key;
//	char *value;
//
//	while (EOF != scanf("%s\n", line)) {
//		key = strtok(line, "=");
//		value = strtok(NULL, "=");
//		printf("%s=%s\n", key, value);
//	}
//}
