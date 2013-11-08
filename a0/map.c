/*
 *  map.c
 *  Author: Jonatan Schroeder
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"

/* Function used to compare two keys. It should return 0 if keys are
   the same, a negative value if the first key is smaller than the
   second key, or a positive value if the first key is larger than the
   second key. It is defined so that if the key is changed to a
   different type, the process doesn't need to be substantially
   changed. */
int map_key_cmp(map_key key1, map_key key2) {
  return key1 - key2;
}

/* Function that converts a string representation into a key. */
map_key str_to_map_key(char *s) {
  return atoi(s);
}

/* Function that converts a string representation into a value. */
map_value str_to_map_value(char *s) {
  if (!s) return s;
  map_value v = (map_value) malloc(strlen(s)+1);
  strcpy(v, s);
  return v;
}

/* Function that prints a mapping in a simple form. */
void print_mapping(map_key key, map_value value) {
  printf("\t<%d, %s>\n", key, value);
}

/* Creates a new empty map and returns the newly created map. */
map map_create(void) {
  // TODO Complete this function
	map myMap = malloc(sizeof(struct map));
	myMap->head = NULL;
	return myMap;
}

/* Inserts a new mapping from a key to a value in the map. If there is
   already a mapping from the given key to another value, this mapping
   must be replaced with the provided value. In this case, if the
   old_value parameter is not NULL, the old value associated to the
   key should be saved in *old_value. If there is no mapping for the
   given key, a new mapping should be created. In this case, if
   old_value is not NULL, *old_value is replaced with NULL. This
   function returns 0 if there is an error, or 1 if the mapping was
   created successfully. */
int map_insert(map *m, map_key key, map_value value, map_value *old_value) {

	map_lookup(m, key, old_value);
	node * newNode = malloc(sizeof(struct node));
	newNode->key = key;
	newNode->value = value;

	node * head = (*m)->head;
	if(head == NULL || head->key > newNode->key){
		(*m)->head = newNode;
		newNode->next = head;
	}else if(head->key == newNode->key){
		head->value = newNode->value;
	
	}else{
		while(head->next != NULL && head->next->key < newNode->key && head->next->key != newNode->key){
			head = head->next;
		}
		if(head->next == NULL){
			head->next = newNode;
		}
		else if( head->next->key == newNode->key ){//equals 
				head->next->value = newNode->value;
		} else {//greater than
				newNode->next = head->next;
				head->next = newNode;
		}
	}
	return 1; 
}

node * insertNode(node * n, node * newNode){
	if(n == NULL){
		return newNode;
	}
	if(newNode->key == n->key){
		n->value = newNode->value;
		return n;
	}
	if(newNode->key > n->key){
		newNode->next = n;
		return newNode;
	}
	n->next = insertNode(n->next, newNode);
	return n;
}

node * deleteNode(node * n, map_key key){
	if(n == NULL){
		return NULL;
	}
	if(n->key == key){
		node * next = n->next;
		free(n);
		return next;
	}
	n->next = deleteNode(n->next, key);
	return n;
}



/* Searches for the value associated to the given key. If the key is
   found, returns 1, and, if the value parameter is not NULL, saves
   the mapped value to *value. If the key is not found in the map,
   returns 0 and keeps *value unaltered. */
int map_lookup(map m, map_key key, map_value *value) {
  // TODO Complete this function
	node * node = m->head;
	while(node != NULL){
		if(node->key == key){
			if(node->value != NULL){
				*value = node->value;
				return 1;
			}
		}
		node = node->next;
	}
	*value = NULL;
	return 0;
}

/* Deletes an existing mapping for the given key. If the value
   parameter is not NULL, saves the existing value in *value. Returns
   1 if there was a mapping for the given key and it was successfully
   removed, or 0 otherwise. */
int map_delete(map *m, map_key key, map_value *value) {
  // TODO Complete this function
	int retValue = map_lookup(m, key, value);
	(*m)->head = deleteNode((*m)->head, key);
	return retValue;
}

/* Applies the function f to every mapping in the map. The mappings
   must be visited in increasing order of the mapping key (as defined
   by map_key_cmp), and each mapping must be visited exactly
   once. This function can be used to print the map by calling:
   map_foreach(m, print_mapping);
 */
void map_foreach(map m, void (*f)(map_key, map_value)) {
  // TODO Complete this function
	if(m->head){
		node * head = m->head;
		while(head != NULL){
			(*f)(head->key, head->value);
			head = head->next;
		}
	}
}

/* Frees all space occupied by the map, including all mappings and
   their values if applicable. */
void map_destroy(map m) {
  // TODO Complete this function
	node * n = m->head;
	destroyNode(n);
	free(m);
}

void destroyNode(node * n){
	if(n==NULL){
		return;	
	}
	destroyNode(n->next);
	free(n);
}
