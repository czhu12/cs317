/*
 * eth_switch1.c
 * Author:
 */

#include <stdio.h>
#include <inttypes.h>

#include "eth_switch1.h"
/* Creates a new empty switch table (with no addresses associated to
 * any port).
 */
switch_table create_switch_table(void) {
	//a switch table maps a given address to a port
	switch_table table = malloc(sizeof(struct switch_table));
	table->head = NULL;
  return table;
}

/* Handles an incoming Ethernet frame. This function is responsible
 * with updating the switch table with the data that can be deduced
 * from the frame, and returning a set of ports where the frame must
 * be forwarded to. The set of ports is represented by the port_set
 * type, and the function may return one of the following macros:
 *
 * - PORT_SET_NONE - Forward to no port at all (drop frame);
 * - PORT_SET_ALL - Forward to all ports;
 * - PORT_SET_ADD_PORT(s, p) - Forward to all ports in set s and to p (e.g., 
 *     PORT_SET_ADD_PORT(PORT_SET_NONE, 2) for port 2 only);
 * - PORT_SET_REMOVE_PORT(s, p) - Forward to all ports in set s but not to p (e.g., 
 *     PORT_SET_REMOVE_PORT(PORT_SET_ALL, 7) for all ports except 7);
 */
port_set forward_incoming_frame(switch_table *table, uint8_t port, uint16_t destination,
				uint16_t source, uint16_t frameid) {
	if(destination == 65535){
		return PORT_SET_REMOVE_PORT(PORT_SET_ALL, port);
	}

	(*table)->head = insert_node((*table)->head, source, port);
	uint8_t dest_port = search_node((*table)->head, destination);
	if(dest_port == 255){
		//this mean it was not found
		return PORT_SET_REMOVE_PORT(PORT_SET_ALL, port);
	}else{
		//this means it was found
		return PORT_SET_ADD_PORT(PORT_SET_NONE, dest_port);
	}

  return PORT_SET_NONE;
}

/* Prints all the elements in the switch table in numerical order of
 * the MAC address. Elements are printed one per line, with each line
 * containing the MAC address (represented in hexadecimal with 4
 * characters) followed by a space and the port number (prefixed with
 * a P and with no leading zeros).
 */
void print_switch_table(switch_table table, FILE *output) {
	node * walker = table->head;

 	while(walker!=NULL){
  	fprintf(output, "%04"PRIx16" P%"PRIu8"\n", walker->key, walker->value);
		walker = walker->next;
	} 
  /* for each element in the switch table do:
     fprintf(output, "%04"PRIx16" P%"PRIu8"\n", mac_address, port);
   */
}

/* Frees any dynamically allocated space used by the switch table.
 */
void destroy_switch_table(switch_table table) {
  free_nodes(table->head);
	free(table);
}


node * insert_node(node * head, uint16_t key, uint8_t value){
	node * newNode = malloc(sizeof(struct node));
	newNode->key = key;
	newNode->value = value;
	
	if (head == NULL || head->key > newNode->key){
		newNode->next = head;
		return newNode;
	}else if(head->key == newNode->key){
		head->value = newNode->value;
	}else {
	 	node * walker = head;
		while(walker->next != NULL && walker->next->key < newNode->key && walker->next->key != newNode->key){
			walker = walker->next;
		}
		if(walker->next == NULL){
			walker->next= newNode;
		}else if(walker->next->key == newNode->key){
			walker->next->value = newNode->value;
		}else {
			newNode->next = walker->next;
			walker->next = newNode;
		}
	}
	//if(newNode->value == 0 ){
	//	printf("NEXT: %d, %d\n", newNode->next->key, newNode->next->value);
	//}
	return head;
}

int update_switch_table(switch_table table, uint8_t port, uint16_t source){
	table->head = insert_node(table->head, source, port);
	return 1;
}

uint8_t search_switch_table(switch_table table, uint16_t source){
	uint8_t port_num = search_node(table->head, source);
	return port_num;
}
void free_nodes(node * n){
	if(n == NULL){
		return;
	}
	free_nodes(n->next);
	free(n);
}
void print_list(node * n){
	if(n == NULL)
		return;
	printf("MAC ADDRESS: %d, PORT: %d\n", n->key, n->value);
	print_list(n->next);
}

node * delete_node(node * n, uint16_t key){
	if(n == NULL){
		return NULL;
	}
	if(n->key == key){
		node * next = n->next;
		free(n);
		return next;
	}
	return delete_node(n->next, key);
}

uint8_t search_node(node * n, uint16_t key){
	if(n == NULL){
		return -1;
	}
	if(n->key == key){
		return n->value;
	}
	return search_node(n->next, key);
}
