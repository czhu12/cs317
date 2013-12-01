/*
 * File: service.c
 */
#define NUM_ROUTES 8
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

cookie * parse_cookies(char * header, int * num_cookies){
	printf("%s\n", header);
	cookie * cookie_walker;
	cookie * cookie_list = malloc(10 * sizeof(cookie));
	cookie_walker = cookie_list;
	char * split = NULL;
	split = strtok(header, "\n\r");
	printf("%s\n", split);
	while(split != NULL){
		split = strtok(NULL, "\n\r");
		if(split && strstr(split, "Cookie: ")){
			char * cookie_split = strtok(split, ":= ");
			char * key = strtok(NULL, ":= ");
			char * value = strtok(NULL, ":= ");
			strcpy(cookie_walker->key, key);
			strcpy(cookie_walker->value, value);
			*num_cookies++;
			//cookie_split = strtok(NULL, ":= ");
			//char * key = NULL;
			//char * value = NULL;
			
			//while(cookie_split != NULL){
			//	key = strtok(NULL, ":= ");
			//	value = strtok(NULL, ":= ");
			//	cookie_walker->action = 2;
			//	strcpy(cookie_walker->key, key);
			//	strcpy(cookie_walker->value, value);
			//	cookie_walker++;
			//	*num_cookies++;
			//}
		}
	}
	return cookie_list;
}

//cookie * parse_cookies(char * header, int * num_cookies){
//	int cookie_counter = 0;
//	cookie * cookie_walker;
//	cookie * cookie_list = malloc(10 * sizeof(cookie));
//	cookie_walker = cookie_list;
//	char * split = NULL;
//	split = strtok(header, "\n\r");
//	while(split != NULL){
//		split = strtok(NULL, "\n\r");
//		
//		if(split && strstr(split, "Cookie: ")){
//			char * cookie_split = strtok(split, ":= ");
//			//cookie_split = strtok(NULL, ":= ");
//			//printf("key:: %s\n", cookie_split);
//
//			//char * key = NULL;
//			//char * value = NULL;
//			char * key = strtok(NULL, ":= ");
//			char * value = strtok(NULL, ":= ");
//			printf("cookie: KEY: %s, VALUE: %s\n", key, value);
//			strcpy(cookie_walker->key, key);
//			strcpy(cookie_walker->value, value);
//			cookie_walker++;
//			cookie_counter++;
//
//			//while(key != NULL){
//			//	key = strtok(NULL, ":= ");
//			//	value = strtok(NULL, ":= ");
//			//	cookie_walker->action = 2;
//			//	strcpy(cookie_walker->key, key);
//			//	strcpy(cookie_walker->value, value);
//			//	cookie_walker++;
//			//	cookie_counter++;
//			//}
//		}
//	*num_cookies = cookie_counter;
//	
//	}
//	return cookie_list;
//}

void init_req_header(req_header * req, char * header){
	char * cookie_header = malloc(strlen(header));
	strcpy(cookie_header, header);
	int len = strlen(header);
	req->body = http_parse_body(header, len);
	req->method = http_parse_method((char *) header);
	req->uri = http_parse_uri(header);
	req->path = http_parse_path(http_parse_uri(header));
	req->connection = http_parse_header_field( header, len, "Connection");
	req->cache_control = http_parse_header_field(header, len, "Cache-Control");

	cookie * cookies = parse_cookies(cookie_header, &(req->num_cookies));
	req->cookies = cookies;
}

void handle_client(int socket) {
	initialize_routes();
	req_header header;

	int recv_bytes;
	int total_recv;

	while(1){
		char buffer[5000] = {0};
		recv_bytes = 0;
		total_recv=0;

		do {
			recv_bytes = recv(socket, buffer + total_recv, 5000, 0);
			if(recv_bytes == -1 || recv_bytes == 0) return;
			total_recv += recv_bytes ;
		}while(http_header_complete(buffer, total_recv) == -1);

		init_req_header(&header, buffer);
		char * returnString = handle_request(&header);
		printf("%s\n", returnString);
		int actlen = strlen(returnString);
		int send_length = send(socket, returnString, strlen(returnString), 0);
		//send_string(socket, returnString, strlen(returnString) + 2);
		//fflush(socket);
		if(!strncmp(header.connection, "close", 5)){
			return;
		}
	}
	//}while(!strcmp(header.connection, "keep-alive"));

	//close(socket);

/* TODO Loop receiving requests and sending appropriate responses,
 *      until one of the conditions to close the connection is
 *      met.
 */
return;
}

int send_string(int socket, char * string, int size){
	int bytes_remaining = size;
	int bytes_sent = 0;
	while(bytes_sent < size){
		int bytes = send(socket, string + bytes_sent, bytes_remaining, 0);
		if(bytes == -1){
			return -1;
		}
		bytes_sent += bytes;
		bytes_remaining -= bytes;
	}
	return 1;
}

char * handle_request(req_header * header){
	return match_routes(header);
}

char * return404(){
	char * returnString = "Command not found\n";
	returnString = add_header("HTTP/1.1 ", "404 Not Found\r\n", returnString);
	return returnString;
}

char * match_routes(req_header * header){
	int i;
	int return_code = 200;
	for(i = 0; i < NUM_ROUTES; i++){

		if(!strcmp(routes[i].route, extract_path(header->path))){
			char * content = handle_route(header, i, &return_code);
			//if( == NULL){
			//	return return404();
			//}
			content = wrap_header(header, content);
			printf("%s", content);
			return content;
		}
	}
	return return404();
}

char * handle_route(req_header * header, int route_index, int * return_code){
	cookie * request_cookies = malloc((header->num_cookies + 1) * sizeof(cookie));
	memcpy(request_cookies, header->cookies, (header->num_cookies + 1) * sizeof(cookie));//SOURCE OF MEMCORRUPTIONS
	free(header->cookies);
	header->cookies = NULL;

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
				*content = '\0';
				sprintf(content, "%s%s\n", "Username: ", username);

				cookie * cookie = malloc(sizeof(*cookie));
				cookie->action = 2;
				strcpy(cookie->key, "username");
				strcpy(cookie->value, username);
				header->cookies = cookie;
				header->num_cookies = 1;
				return content;
			}
			break;
		case 2://logout
			{
				char buffer[100];
				strcpy(buffer, header->path);
				char * split;
				split = strtok(buffer, "/=?&");
				char * username = header->cookies->value;
				printf("%s\n", username);
				
				header->cookies->action = 2;
				char * content = malloc(strlen("User ") + strlen(username) + strlen("was logged out") + 10);
				sprintf(content, "%s%s%s", "User ", username, " was logged out");
				return content;
			}
			break;
		case 3://getfile
			{
				char buffer[100];
				strcpy(buffer, header->path);
				char * split;
				split = strtok(buffer, "/=?&");

				char * filename = NULL;

				while(split != NULL){
					if(!strcmp(strtok(NULL, "/=?&"), "filename")){
						filename = strtok(NULL, "/=?&");
						break;
					}
				}
				//return read_file(filename);
				return "";
			}
			break;
		case 4://putfile
			{
				char buffer[100];
				strcpy(buffer, header->path);
				char * split;
				split = strtok(buffer, "/=?&");
				char * filename = NULL;

				while(split != NULL){
					if(!strcmp(strtok(NULL, "/=?&"), "filename")){
						filename = strtok(NULL, "/=?&");
						break;
					}
				}
				int status = put_file(filename, header->body);
				if(!status){
					return NULL;
				}
				return "";

			}
			break;
		case 5://addcart
			{
				return "";
			}
			break;
		case 6://delcart
			{
				return "";
			}
			break;
		case 7://checkout
			{
				return "";
			}
			break;
	}
	return NULL;
}

char * build_cookie_string(cookie * cookies){
	char * buffer = malloc(strlen(cookies->key) + strlen(cookies->value) + strlen(get_date(24)) + 1);
	if(cookies->action == 2){
		sprintf(buffer, "%s=%s;Expires=%s", cookies->key, cookies->value, get_date(24));
	}else {
		sprintf(buffer, "%s=%s;Expires=%s", cookies->key, cookies->value, get_date(0));
	}
	//while(cookies){ 
	//	strcat(buffer, cookies->key);
	//	strcat(buffer, "=");
	//	strcat(buffer, cookies->value);
	//	cookies++;
	//}
	return buffer;
}

char * wrap_header(req_header * header, char * content ){
	int content_length = strlen(content);
	char content_length_str[4] = {0}; sprintf(content_length_str, "%d", content_length);

	char * cookie_string = NULL;
	content = add_header("Content-Type: ","text/plain\r\n", content);
	//content = add_header("","\r\n", content);
	//content = add_header("Content-Type: ","text/plain", content);

	if(header->num_cookies > 0){
		cookie_string = build_cookie_string(header->cookies);
		content = add_header("Set-Cookie: ",cookie_string, content);
	}

	//content = add_header("Content-Type: ","text/plain", content);
	content = add_header("Content-Length: ", content_length_str, content);
	content = add_header("Connection: ", header->connection, content);
	content = add_header("Date: ", get_date(0), content);
	content = add_header("Cache-Control: ", "public", content);
	content = add_header("HTTP/1.1 ", "200 OK", content);
	return content;
}

char * add_header(const char * header_type, const char * header_content, char * content){
	char * header = build_header(header_type, header_content);
	char * new_str =  malloc(strlen(header) + strlen(content) + 4);
	sprintf(new_str, "%s%s", header, content);
	//printf("Size of header: %d, sizeof content, %d, sizeof new_str: %d\n", (int)strlen(header),(int)strlen(content),(int)strlen(new_str));
	free(header);
	return new_str;
}

char * build_header(const char * header_type, const char * header_content){
	//char * new_str;
	char * endToken = "\r\n";

	char * new_str = (char * )malloc((int)strlen(header_type) + (int)strlen(header_content) + (int)strlen(endToken) + 1);
	sprintf(new_str, "%s%s%s", header_type, header_content, endToken);
	return new_str;
}
const char * extract_path(const char * path){
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
char * get_date(int timeforward){
	char buf[1000];
	time_t now = time(0);
	now = now + (60 * 60 * timeforward);
	struct tm tm = *gmtime(&now);
	strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &tm);
	char * return_date = malloc(strlen(buf));
	strcpy(return_date, buf);
	return return_date;
}

void initialize_routes(){
	routes[0].method = 0;
	routes[0].route = "knock";
	routes[1].method = 0;
	routes[1].route = "login";
	routes[2].method = 0;
	routes[2].route = "logout";
	routes[3].method = 0;
	routes[3].route = "getfile";
	routes[4].method = 1;
	routes[4].route = "putfile";
	routes[5].method = 0;
	routes[5].route = "addcart";
	routes[6].method = 0;
	routes[6].route = "delcart";
	routes[7].method = 0;
	routes[7].route = "checkout";
}

char * read_file( char * filename){
	FILE * file;
	file = fopen(filename, "r");
	if(!file){
		return NULL;
	}

	fseek(file, 0L, SEEK_END);
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char * fileBuffer = malloc(20*sizeof(size)+1);
	fileBuffer[size+1] = 0;
	fread(fileBuffer, size, 1, file);
	fclose(file);

	return fileBuffer;
}
int put_file(char * filename, const char * content){
	FILE * file;
	file = fopen(filename, "w");
	if(file == NULL){
		return 0;
	}
	fputs(content, file);
	fclose(file);
	return 1;

}
