/*
 *  ip_route.h
 *  Author: Jonatan Schroeder
 */

#ifndef _IP_ROUTE_H_
#define _IP_ROUTE_H_

#include <stdint.h>

#include "capacity.h"


typedef struct node_key {
	uint32_t ip;
	uint8_t netsize;
} node_key;

typedef struct node_value {
	int nic_distances[NUM_NICS];
} node_value;

typedef struct node {
	struct node * next;
	node_key key; // ip address
	node_value value; // nics
} node;

typedef struct router_state {
  node * head;
} *router_state;


router_state initialize_router(void);
void process_update(router_state *state, uint32_t ip, uint8_t netsize,
		    int nic, unsigned int metric, unsigned int update_id);
void destroy_router(router_state state);
node * node_insert(node * head, uint32_t ip, uint8_t netsize, int nic, unsigned int metric);
int key_compare(node_key key1, node_key key2);
int key_equals(node_key key1, node_key key2);
node * node_delete(node * n, uint32_t ip, uint8_t netsize);
void initialize_node_value( node_value *n );
node * node_find_subnet(node * n, node_key key);
int find_shortest_nic(node * n, int * best_nic);

#endif
