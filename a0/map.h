/*
 *  map.h
 *  Author: Jonatan Schroeder
 */

#ifndef _MAP_H_
#define _MAP_H_

/* Types for the key and value of the map type */
typedef int map_key;
typedef char *map_value;

#define PRINTF_PH_MAPKEY "%d"
#define PRINTF_PH_MAPVALUE "%s"

/* Type corresponding to a map. You may need additional fields or
   structures to keep your entries and data. */
typedef struct node {
	struct node * next;
	map_key key;
	map_value value;
} node;

typedef struct map {
  // Field included so this code compiles. You may replace it with
  // your own fields.
	struct node * head;
} *map;

/* Functions to deal with keys and values directly. These functions
   need to be revised if the key and value types are changed. */
int map_key_cmp(map_key key1, map_key key2);
map_key str_to_map_key(char *s);
map_value str_to_map_value(char *s);
void print_mapping(map_key key, map_value value);

/* Operations in a map. */
map map_create(void);
void map_destroy(map m);
int map_insert(map *m, map_key key, map_value value, map_value *old_value);
int map_lookup(map m, map_key key, map_value *value);
int map_size(map m);
int map_delete(map *m, map_key key, map_value *value);
void map_foreach(map m, void (*f)(map_key, map_value));
node * deleteNode(node * n, map_key key);
void destroyNode(node * n);
node * insertNode(node * n, node * newNode);

#endif
