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
	const char * connection;
	cookie * cookies;
	int num_cookies;
	const char * cache_control;
} req_header;


char * get_date(int timeforward);
cookie * parse_cookies(char * header, int * num_cookies);
char * handle_route(req_header * header, int route_index);
char * build_header(const char * header_type, const char * header_content);
void init_req_header(req_header * req, char * header);
char * handle_request(req_header * header);
char * wrap_header(req_header * header, char * content);
char * match_routes(req_header * header);
char * add_header(const char * header_type, const char * header_content, char * content);
void initialize_routes();
void handle_client(int socket);
const char * extract_path(const char * path);
int send_string(int socket, char * string, int size);
#endif
