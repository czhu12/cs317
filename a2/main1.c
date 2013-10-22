/*
 *  main1.c
 *  Author: Jonatan Schroeder
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <inttypes.h>

#include "eth_switch1.h"

/* Largest line in the input file. */
#define MAXLINE 1000

/* Prints a set of ports to be enqueued. */
static void print_port_set(port_set set, uint16_t frameid) {
  
  int i;
  
  for (i = 0; i < NUM_PORTS; i++)
    if (PORT_SET_HAS_PORT(set, i))
      printf("+%03"PRIu16" ", frameid);
    else
      printf(" --- ");
  printf("\n");
}

int main(int argc, char *argv[]) {
  
  FILE *st_output;
  char *filename;
  char line[MAXLINE];
  char *command, *deststr, *srcstr, *frameidstr, *tokptr;
  uint8_t port;
  uint16_t destination, source, frameid;
  switch_table table;
  int i;
  
  if (argc == 1) {
    filename = "switch_table.txt";
  }
  else {
    filename = argv[1];
    if (!strcmp(filename, "-"))
      filename = "/dev/stdout";
  }
  
  table = create_switch_table();
  
  // Print header
  for (i = 0; i < NUM_PORTS; i++) printf(" P%-2"PRIu16" ", i);
  printf("\n");
  
  while (fgets(line, MAXLINE, stdin)) {
    
    command = strtok_r(line, " \t\r\n", &tokptr);
    if (!command) continue;
    
    if (toupper(command[0]) == 'P') {
      
      deststr = strtok_r(NULL, " \t\r\n", &tokptr);
      srcstr = strtok_r(NULL, " \t\r\n", &tokptr);
      frameidstr = strtok_r(NULL, " \t\r\n", &tokptr);
      
      port = strtol(command + 1, NULL, 10);
      destination = strtol(deststr, NULL, 16);
      source = strtol(srcstr, NULL, 16);
      frameid = strtol(frameidstr, NULL, 10);
      
      port_set ports_to_enqueue = forward_incoming_frame(&table, port, destination, source, frameid);
      print_port_set(ports_to_enqueue, frameid);
    }
    else if (!strcasecmp(command, "TICK")) {
      
      for (i = 0; i < NUM_PORTS; i++) printf(" DEQ ");
      printf("\n");
    }
    else if (!strcasecmp(command, "QUIT")) {
      
      break;
    }
    else {
      
      printf("Invalid command: %s\n", command);
    }
  }
  
  st_output = fopen(filename, "w");
  if (!st_output) {
    perror("Could not open file for writing");
    return 2;
  }
  
  print_switch_table(table, st_output);
  destroy_switch_table(table);
  
  if (ferror(st_output)) {
    perror("Could not read from input file");
    fclose(st_output);
    return 3;
  }
  
  fclose(st_output);
  
  return EXIT_SUCCESS;
}
