/*
 * File: service.h
 */

#ifndef _SERVICE_H_
#define _SERVICE_H_

typedef struct cookie {
	char key[100];
	char value[100];
	int action; // 2 = add, 1 = delete, 0 = uninitalized
}cookie;

typedef struct route {
	int method;
	const char * route;
} route;

typedef struct req_header {
	const char * body;
	int method;
	const char * uri;
	const char * path;
	const char * pragma;
	const char * content_length;
	const char * accept_encoding;
	const char * host;
	cookie * cookies;
	int num_cookies;
} req_header;


char * get_date();
cookie * parse_cookies(char * header);
char * handle_route(req_header * header, int route_index);
char * build_header(char * header_type, char * header_content);
void init_req_header(req_header * req, char * header);
char * handle_request(req_header * header);
char * wrap_header(req_header * header, char * content);
char * match_routes(req_header * header);
char * add_header(char * header_type, char * header_content, char * content);
void initialize_routes();
void handle_client(int socket);
char * extract_path(char * path);
#endif
