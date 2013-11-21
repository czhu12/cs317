/*
 * File: service.h
 */

#ifndef _SERVICE_H_
#define _SERVICE_H_
typedef struct req_header {
	const char * body;
	int method;
	const char * uri;
	const char * path;
	const char * pragma;
	const char * content_length;
	const char * accept_encoding;
	const char * host;
} req_header;

typedef struct route {
	int method;
	const char * route;
} route;

void handle_client(int socket);
void parseHeader(void * ptr, req_header * header);
void init_req_header(req_header * req, char * header);
void print_req_header(req_header * header);
char * handle_request(req_header * header);
char * wrap_header( char * content);
char * match_routes(req_header * header);
char * handle_route(int route_index);
char * add_header(char * header_type, char * header_content, char * content);
char * build_header(char * header_type, char * header_content);
char * get_date();
#endif
