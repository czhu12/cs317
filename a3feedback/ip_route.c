/*
 * ip_route.c
 * Author:
 */

#include <stdio.h>
#include <stdlib.h>

#include "ip_route.h"

static inline void print_advertisement(uint32_t ip, uint8_t netsize, int nic,
                                       unsigned int metric, unsigned int update_id) {
  printf("A %u.%u.%u.%u/%u %u %u\n"
         "T %u.%u.%u.%u/%u %d\n",
         (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF, netsize, metric, update_id,
         (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF, netsize, nic);
}

/* This function initializes the state of the router.
 */
router_state initialize_router(void) {
  return malloc(sizeof(struct router_state));
}


/* This function is called for every line corresponding to a routing
 * update. The IP is represented as a 32-bit unsigned integer. The
 * netsize parameter corresponds to the size of the prefix
 * corresponding to the network part of the IP address. The metric
 * corresponds to the value informed by the neighboring router, and
 * does not include the cost to reach that router (which is assumed to
 * be always one). If the update triggers an advertisement, this
 * function prints the advertisement in the standard output.
 */
void process_update(router_state *state, uint32_t ip, uint8_t netsize,
		    int nic, unsigned int metric, unsigned int update_id) {
  // TODO Implement this function.
	int distance;
	int new_distance;
	int best_nic;
	node_key node_key;

	node_key.ip = ip;
	node_key.netsize = netsize;
	//find subnet info
	node * n = node_find_subnet((*state)->head, node_key);

	//find minimum distance info
	if(n) distance = find_shortest_nic(n, &best_nic);

	//insert new data
	(*state)->head = node_insert((*state)->head, ip, netsize, nic, metric + 1);

	//find new_minimum distance info
	if(!n){ //node wasnt found previously
		node * new_node = node_find_subnet((*state)->head, node_key);
		new_distance = find_shortest_nic(new_node, &best_nic);
		print_advertisement(ip, netsize, best_nic, new_distance, update_id );
		return;
	}else{
	new_distance = find_shortest_nic(n, &best_nic);
	
	//if distance isnt -1 (which means nothing was found in the first search, 
	//and distance is not new distance (best distance changed)
	//then advertise
		if (distance != new_distance){
			print_advertisement(ip, netsize, best_nic, new_distance, update_id );
		}
	}

}

/* Destroys all memory dynamically allocated through this state (such
 * as the forwarding table) and frees all resources used by the
 * router.
 */
void destroy_router(router_state state) {
  free(state->head);
	free(state);
}

void free_nodes(node * n){
	if(n==NULL) return;	
	free_nodes(n->next);
	free(n);
}

void initialize_node_value( node_value * n ){
	//int *ptr = n->nic_distances;
	int i;
	
	for(i = 0; i < NUM_NICS; i++){
		n->nic_distances[i] = METRIC_UNREACHABLE;
	}
}


node * node_find_subnet( node * n, node_key key ){
	if(n == NULL){
		return NULL;
	}
	if(key_equals(key, n->key)){
		return n;
	}
	return node_find_subnet(n->next, key);
}

int find_shortest_nic(node * n, int * best_nic){
	if(n == NULL){
		return -1;
	}
	node_value node_value = n->value;
	int i;
	int best = METRIC_UNREACHABLE;
	for(i= 0; i < NUM_NICS; i++){
		if (node_value.nic_distances[i] < best) {
			best = node_value.nic_distances[i];
			*best_nic = i;
		}
	}
	return best;
}









int key_equals(node_key key1, node_key key2){
	return key1.ip == key2.ip && key1.netsize == key2.netsize;
}

int key_compare (node_key key1, node_key key2){
	if (key1.ip == key2.ip){
		return key1.netsize > key2.netsize;
	}
	return key1.ip > key2.ip;
}

node * node_insert(node * head, uint32_t ip, uint8_t netsize, int nic, unsigned int metric){
	node * newNode = malloc(sizeof(struct node));
	newNode->key.ip = ip;
	newNode->key.netsize = netsize;
	initialize_node_value(&(newNode->value));
	newNode->value.nic_distances[nic] = metric;

	if(head == NULL || key_compare(head->key, newNode->key)){ // newNode < head then newNode->head
		newNode->next = head;
		return newNode;
	}else if(key_equals(head->key, newNode->key)){
		// add NIC to existing node
		//head->value = newNode->value;
		head->value.nic_distances[nic] = metric;
	}else{
		node * walker = head;
		while(walker->next != NULL && !key_compare(walker->next->key, newNode->key) && !key_equals(walker->next->key, newNode->key)){ //while walker->next > newNode, keep traversing
			walker = walker->next;
		}
		if(walker->next == NULL){
			walker->next = newNode;
		}else if(key_equals(walker->next->key, newNode->key)){
			// add NIC to existing node
			walker->next->value = newNode->value;
			head->value.nic_distances[nic] = metric;
		}else{
			newNode->next = walker->next;
			walker->next = newNode;
		}
	}
	return head;
}
