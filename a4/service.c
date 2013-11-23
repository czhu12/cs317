/*
 * File: service.c
 */
#define NUM_ROUTES 2
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#include "service.h"
#include "util.h"

route routes[NUM_ROUTES];

cookie * parse_cookies(char * header){
	cookie * cookie_walker;
	cookie * cookie_list = malloc(10 * sizeof(cookie));
	cookie_walker = cookie_list;
	char * split = NULL;
	split = strtok(header, "\n\r");
	while(split != NULL){
		split = strtok(NULL, "\n\r");
		if(split && strstr(split, "Cookie: ")){
			char * cookie_split = strtok(split, ":= ");
			cookie_split = strtok(NULL, ":= ");
			char * key = NULL;
			char * value = NULL;
			while(cookie_split != NULL){
				key = strtok(NULL, ":= ");
				value = strtok(NULL, ":= ");
				cookie_walker->action = 2;
				strcpy(cookie_walker->key, key);
				strcpy(cookie_walker->value, value);
				cookie_walker++;
			}
		}
	}
	return cookie_list;
}

void init_req_header(req_header * req, char * header){
	int len = strlen(header);
	req->body = http_parse_body(header, len);
	req->method = http_parse_method((char *) header);
	req->uri = http_parse_uri(header);
	req->path = http_parse_path(http_parse_uri(header));
	cookie * cookies = parse_cookies(header);
	req->cookies = cookies;
	req->connection = http_parse_header_field(req, len, "Connection");
}

void handle_client(int socket) {
	initialize_routes();
	req_header header;

	do {
	char buffer[5000];
	int status = recv(socket, buffer, 4000, 0);
	if(status == 0 || status == -1) break;
	init_req_header(&header, buffer);
	char * returnString = handle_request(&header);
	send(socket, returnString, strlen(returnString), 0);
	}while(!strcmp(header.connection, "keep-alive"));

	close(socket);

	/* TODO Loop receiving requests and sending appropriate responses,
	 *      until one of the conditions to close the connection is
	 *      met.
	 */

	return;
}

char * handle_request(req_header * header){
	return match_routes(header);
}

char * return404(){
	char * returnString = "Command not found";
	returnString = add_header("HTTP/1.1 ", "404 Not Found\r\n", returnString);
	printf("%s\n", returnString);
	return returnString;
}

char * match_routes(req_header * header){
	int i;
	for(i = 0; i < NUM_ROUTES; i++){
		if(!strcmp(routes[i].route, extract_path(header->path))){
			printf("Routing to %s\n", extract_path(header->path));
			char * content = handle_route(header, i);
			content = wrap_header(header, content);
			printf("%s\n", content);
			return content;
		}
	}
	//printf("did not find any matching routes...\n");
	return return404();
}

char * handle_route(req_header * header, int route_index){

	switch (route_index){
		case 0://knock
			return "Who's there?";
			break;
		case 1://login
			{
				char buffer[100];
				strcpy(buffer, header->path);
				char * split;
				split = strtok(buffer, "/=?&");
				char * username = NULL;

				while(split != NULL){
					if(!strcmp(strtok(NULL, "/=?&"), "username")){
						username = strtok(NULL, "/=?&");
						break;
					}
				}
				char * content = malloc(strlen("Username: ") + strlen(username) + 10);
				sprintf(content, "%s%s", "Username: ", username);

				cookie * cookie = malloc(sizeof(cookie));
				cookie->action = 2;
				strcpy(cookie->key, "username");
				strcpy(cookie->value, username);
				header->cookies = cookie;
				return content;
			}
			break;
		case 2://logout
			{
				char buffer[100];
				strcpy(buffer, header->path);
				char * split;
				split = strtok(buffer, "/=?&");
				char * username = NULL;

				while(split != NULL){
					if(!strcmp(strtok(NULL, "/=?&"), "username")){
						username = strtok(NULL, "/=?&");
						break;
					}
				}
				char * content = malloc(strlen("Username: ") + strlen(username) + 10);
				sprintf(content, "%s%s%s", "User: ", username, " was logged out");
				
				
			}
			break;
		case 3://getfile
			break;
		case 4://putfile
			break;
		case 5://addcart
			break;
		case 6://delcart
			break;
		case 7://checkout
			break;
	}
	return NULL;
}

char * build_cookie_string(cookie * cookies){
	char * buffer = malloc(strlen(cookies->key) + strlen(cookies->value) + 10);
	if(cookies){
		sprintf(buffer, "%s=%s", cookies->key, cookies->value);
		//while(cookies){ 
		//	strcat(buffer, cookies->key);
		//	strcat(buffer, "=");
		//	strcat(buffer, cookies->value);
		//	cookies++;
		//}
	}
	return buffer;
}

char * wrap_header(req_header * header, char * content ){
	int content_length = strlen(content);
	char content_length_str[4] = {0};
	sprintf(content_length_str, "%d", content_length);

	char * cookie_string = NULL;
	cookie_string = build_cookie_string(header->cookies);

	content = add_header("Content-Type: ","text/plain\r\n", content);

	if(header->cookies){
		content = add_header("Set-Cookie: ",cookie_string, content);
	}
	content = add_header("Content-Type: ","text/plain", content);
	content = add_header("Content-Length: ", content_length_str, content);
	content = add_header("Connection: ", "keep-alive", content);
	content = add_header("Date: ",get_date(), content);
	content = add_header("Cache-Control: ", "public", content);
	content = add_header("HTTP/1.1 ", "200 OK", content);
	return content;
}
char * add_header(char * header_type, char * header_content, char * content){
	char * header = build_header(header_type, header_content);
	char * new_str = (char * ) malloc((int)strlen(header) + (int)strlen(content) + 1);
	sprintf(new_str, "%s%s", header, content);

	free(header);
	return new_str;
}

char * build_header(char * header_type, char * header_content){
	//char * new_str;
	char * endToken = "\r\n";

	char * new_str = (char * )malloc((int)strlen(header_type) + (int)strlen(header_content) + (int)strlen(endToken) + 1);
	sprintf(new_str, "%s%s%s", header_type, header_content, endToken);
	return new_str;
}
char * extract_path(char * path){
	if(*path == '/' && (strlen(path) == 1)){
		return path;
	}
	char buffer[strlen(path)];
	strcpy(buffer, path);
	char * split = NULL;
	split = strtok(buffer, "=/?&");
	char * new_path = malloc(strlen(buffer));
	strcpy(new_path, split);
	return new_path;
}
char * get_date(){
	char buf[1000];
	time_t now = time(0);
	struct tm tm = *gmtime(&now);
	strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &tm);
	char * return_date = malloc(strlen(buf));
	strcpy(return_date, buf);
	return return_date;
}

void initialize_routes(){
	routes[0].method = 0;
	routes[0].route = "knock";
	routes[1].method = 1;
	routes[1].route = "login";
}
