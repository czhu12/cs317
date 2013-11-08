/*
 * ip_forward.c
 * Author:
 */

#include <stdio.h>
#include <stdlib.h>

#include "ip_forward.h"

/* Helper function that prints the output of a frame being forwarded. */
static inline void print_forwarding(unsigned int packet_id, int nic) {
  printf("O %u %d\n", packet_id, nic);
}

/* Helper function that prints a forwarding table entry. */
static inline void print_forwarding_table_entry(uint32_t ip, uint8_t netsize, int nic, FILE *output) {
  fprintf(output, "%u.%u.%u.%u/%u %d\n",
          (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF, netsize, nic);
}

/* This function initializes the state of the router.
 */
router_state initialize_router(void) {
  // TODO Implement this function.
	return malloc(sizeof(struct router_state));
}

/* This function is called for every line corresponding to a table
 * entry. The IP is represented as a 32-bit unsigned integer. The
 * netsize parameter corresponds to the size of the prefix
 * corresponding to the network part of the IP address. Nothing is
 * printed as a result of this function.
 */
void populate_forwarding_table(router_state *state, uint32_t ip, uint8_t netsize, int nic) {
  (*state)->head = node_insert((*state)->head, ip, netsize, nic);
  // TODO Implement this function.
	
}
int key_compare (node_key key1, node_key key2){
	if (key1.ip == key2.ip){
		return key1.netsize > key2.netsize;
	}
	return key1.ip > key2.ip;
}

node * node_insert(node * head, uint32_t ip, uint8_t netsize, int nic){
	node * newNode = malloc(sizeof(struct node));
	newNode->key.ip = ip;
	newNode->key.netsize = netsize;
	newNode->value = nic;

	if(head == NULL || key_compare(head->key, newNode->key)){ // newNode < head then newNode->head
		newNode->next = head;
		return newNode;
	}else{
		node * walker = head;
		while(walker->next != NULL && key_compare(walker->next->key, newNode->key)){ //while walker->next > newNode, keep traversing
			walker = walker->next;
		}
		if(walker->next == NULL){
			walker->next = newNode;
		}else{
			newNode->next = walker->next;
			walker->next = newNode;
		}
	}
	return head;
}

/* This function is called for every line corresponding to a packet to
 * be forwarded. The IP is represented as a 32-bit unsigned
 * integer. The forwarding table is consulted and the packet is
 * directed to the smallest subnet (longest prefix) in the table that
 * contains the informed destination IP. A line is printed with the
 * information about this forwarding (the function print_forwarding is
 * called in this case).
 */
void forward_packet(router_state state, uint32_t ip, unsigned int packet_id) {
  // TODO Implement this function.
	int nic = find_longest_matching_prefix(state->head, ip);
	print_forwarding(packet_id, 0);
}

//static inline void print_forwarding(unsigned int packet_id, int nic) {

/* Prints the current state of the router forwarding table. This
 * function will call the function print_forwarding_table_entry for
 * each valid entry in the forwarding table, in order of prefix
 * address (or in order of netsize if prefix is the same).
 */
void print_router_state(router_state state, FILE *output) {
	print_nodes(state->head, output);
  
}

/* Destroys all memory dynamically allocated through this state (such
 * as the forwarding table) and frees all resources used by the
 * router.
 */
void destroy_router(router_state state) {
	free_nodes(state->head);
	free(state);
  // TODO Implement this function.
}

void print_nodes(node * node, FILE * output){
	if(node == NULL){
		return;
	}
	print_forwarding_table_entry(node->key.ip, node->key.netsize, node->value, output);
	print_nodes(node->next, output);
}

void free_nodes(node * n){
	if(n==NULL){
		return;	
	}
	free_nodes(n->next);
	free(n);
	
}
