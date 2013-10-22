/*
 *  main.c
 *  Author: Jonatan Schroeder
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"

/* Largest line in the input file. */
#define MAXLINE 1000
//int main(int argc, char * argv[]){
//	map m = map_create();
//	map_value value, old_value;
//	m->head = NULL;
//	
//	node * n1 = malloc(sizeof(node));
//	node * n2 = malloc(sizeof(node));
//	node * n3 = malloc(sizeof(node));
//	n1->key = 1;
//	n1->value = "hell";
//	n2->key = 2;
//	n2->value = "hello world";
//	n3->key = 3;
//	n3->value = "heasd";
//	n3->next = NULL;
//	n1->next = n2;
//	n2->next = n3;
//
//	m->head = n1;
//	//printf("%d\n", m->head->key);
//	//printf("%s\n", m->head->value);
//	//printf("%d\n", m->head->next->key);
//	//printf("%s\n", m->head->next->value);
//	//printf("%d\n", m->head->next->next->key);
//	//printf("%s\n", m->head->next->next->value);
//	map_key key = 1;
//	value = "hello";
//  if (map_insert(&m, key, value, &old_value)) {
//    if (old_value) {
//      printf("Mapping <%d, %s> added successfully (old value was <"
//             PRINTF_PH_MAPVALUE ">).\n", key, value, old_value);
//    } else
//      printf("Mapping <%d, %s> added successfully.\n", key, value);
//  } else {
//    printf("Could not add mapping <%d, %s>.\n", key, value);
//  }
//	return 0;
//}

int main(int argc, char *argv[]) {
  
  FILE *input;
  char *filename;
  char line[MAXLINE];
  char *command, *keystr, *valuestr, *tokptr;
  map_key key;
  map_value value, old_value;
  map m;
  
  if (argc == 1) {
    filename = "/dev/stdin";
  }
  else {
    filename = argv[1];
    if (strcmp(filename, "-"))
      filename = "/dev/stdin";
  }
  
  input = fopen(filename, "r");
  if (!input) {
    perror("Could not open file for reading");
    return 2;
  }
   
 	m = map_create();
   
  while(fgets(line, MAXLINE, input)) {
    
    command = strtok_r(line, " \t\n", &tokptr);
    if (!command) continue;
    
    if (!strcasecmp(command, "INSERT")) {
      
      keystr = strtok_r(NULL, " \t\n", &tokptr);
      valuestr = strtok_r(NULL, "\n", &tokptr);
      
      key = str_to_map_key(keystr);
      value = str_to_map_value(valuestr);
      if (map_insert(&m, key, value, &old_value)) {
        if (old_value) {
          printf("Mapping <%s, %s> added successfully (old value was <"
                 PRINTF_PH_MAPVALUE ">).\n", keystr, valuestr, old_value);
          //free(old_value);
        } else
          printf("Mapping <%s, %s> added successfully.\n", keystr, valuestr);
      } else {
        printf("Could not add mapping <%s, %s>.\n", keystr, valuestr);
      }
    }
    else if (!strcasecmp(command, "DELETE")) {
      
      keystr = strtok_r(NULL, " \t\n", &tokptr);
      
      key = str_to_map_key(keystr);
      
      if (map_delete(&m, key, &value)) {
        printf("Mapping <%s, " PRINTF_PH_MAPVALUE "> deleted successfully.\n",
               keystr, value);
        //free(value);
      } else {
        printf("Could not delete mapping for key <%s>.\n", keystr);
      }
    }
    else if (!strcasecmp(command, "SEARCH")) {
      
      keystr = strtok_r(NULL, " \t\n", &tokptr);
      
      key = str_to_map_key(keystr);
      
      if (map_lookup(m, key, &value)) {
        printf("Key <%s> is currently mapped to value <"
               PRINTF_PH_MAPVALUE ">.\n", keystr, value);
      } else {
        printf("Key <%s> is not currently mapped to any value.\n", keystr);
      }
    }
    else if (!strcasecmp(command, "LIST")) {
      map_foreach(m, print_mapping);
    }
    else if (!strcasecmp(command, "QUIT")) {
      
      break;
    }
    else {
      
      printf("Invalid command: %s\n", command);
    }
  }
  
  map_destroy(m);
  
  if (ferror(input)) {
    perror("Could not read from input file");
    fclose(input);
    return 3;
  }
  
  fclose(input);
  
  return EXIT_SUCCESS;
}
