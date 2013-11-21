/*
 * File: service.c
 */

#include <time.h>
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

#include "service.h"
#include "util.h"

#define NUM_ROUTES 10
    /* TODO Loop receiving requests and sending appropriate responses,
     *      until one of the conditions to close the connection is
     *      met.
     */

static route routes[NUM_ROUTES];

void initialize_routes(){
	routes[0].method = 0;
	routes[0].route = "/knock";
	routes[1].method = 1;
	routes[1].route = "/login";
}

void handle_client(int socket) {
		printf("hello world");
		//initialize_routes();
		req_header header;
		printf("%d", sizeof(req_header));
		char * buffer = malloc(5000);
		int size = recv(socket, buffer, 5000, 0);
		printf("calling init req header\n");
		init_req_header(&header, buffer);
		printf("hello world!!!!!!!!!");

		char * returnString = handle_request(&header);

		send(socket, returnString, strlen(returnString), 0);
}

char * handle_request(req_header * header){
	char * returnContent;
	returnContent = match_routes(header);
	
	//handle 404's here
	returnContent = wrap_header(returnContent);
	return returnContent;
}

char * wrap_header( char * content){
	int content_length = strlen(content);
	char content_length_str[5];
	sprintf(content_length_str, "%d", content_length);
	////set cookies here
	content = add_header("Content-Type: ","text/plain", content);
	content = add_header("Content-Length: ", content_length_str, content);
	content = add_header("Date: ",get_date(), content);
	content = add_header("Cache-Control: ", "public", content);
	
	return content;
}
char * match_routes(req_header * header){
	int i;
	for(i = 0; i < NUM_ROUTES; i++){
		if(strcmp(routes[i].route, header->path)){
			return handle_route(&routes[i]);
		}
	}
	return "\n";
}
char * handle_route(route * route){
	char * hello = "Who's There?";
	return hello;
}


char * get_date(){
	return "Sun 18 Nov 2013 04:26:45 GMT";
}

char * add_header(char * header_type, char * header_content, char * content){
	char * header = build_header(header_type, header_content);
	char * new_str =(char * ) malloc((int)strlen(header) + (int)strlen(content) + 100);
	strcat(new_str, header);
	strcat(new_str, content);
	return new_str;
}

char * build_header(char * header_type, char * header_content){
	//char * new_str;
	char * endToken = "\r\n";
	//printf("%s%s\n", header_type, header_content);

	char * new_str = (char * )malloc((int)strlen(header_type) + (int)strlen(header_content) + (int)strlen(endToken) + 1);
	strcat(new_str, header_type);
	strcat(new_str, header_content);
	strcat(new_str, endToken);
	return new_str;
}

void init_req_header(req_header * req, char * header){
	int len = strlen(header);
	printf("%d", len);

	//printf("%s", header);
	req->body = http_parse_body(header, len);
	req->method = http_parse_method((char *) header);
	req->uri = http_parse_uri(header);
	req->path = http_parse_path(http_parse_uri(header));
	req->pragma = http_parse_header_field(header, len, "Pragma");
	req->content_length = http_parse_header_field(header, len, "Content-length");
	req->accept_encoding = http_parse_header_field(header, len, "Accept-Encoding");
	req->host = http_parse_header_field(header, len, "Host");
}
void print_req_header(req_header * header){
    printf("Body: %s\n", header->body);
    printf("Method: %d (%s)\n", header->method, http_method_str[header->method]);
    printf("URI: '%s' (path is '%s')\n", header->uri, header->path);
    printf("Pragma: '%s'\n", header->pragma);
    printf("Content-length: '%s'\n", header->content_length);
    printf("Accept-Encoding: '%s'\n", header->accept_encoding);
    printf("Host: '%s'\n", header->host);
}
