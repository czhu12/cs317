/*
 *  ip_forward.h
 *  Author: Jonatan Schroeder
 */

#ifndef _IP_FORWARD_H_
#define _IP_FORWARD_H_

#include <stdint.h>

#include "capacity.h"

typedef struct node_key {
	uint32_t ip;
	uint8_t netsize;
} node_key;

typedef struct node {
	struct node * next;
	node_key key;
	int value;//nic
} node;

typedef struct router_state {
  // TODO Add all fields deamed necessary for the state of the router
  node * head;
} *router_state;

router_state initialize_router(void);
void populate_forwarding_table(router_state *state, uint32_t ip, uint8_t netsize, int nic);
void forward_packet(router_state state, uint32_t ip, unsigned int packet_id);
void print_router_state(router_state state, FILE *output);
void destroy_router(router_state state);
node * node_insert(node * head, uint32_t ip, uint8_t netsize, int nic);
int key_compare(node_key key1, node_key key2);
void print_nodes(node * node, FILE * output);
void free_nodes(node * n);
int find_longest_prefix_match(node * n, uint32_t ip);
int check_match(node * n, uint32_t ip);	
node * node_delete(node * n, uint32_t ip, uint8_t netsize);
int key_equals(node_key key1, node_key key2);
void print(node * n);
#endif
