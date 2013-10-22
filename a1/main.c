/*
 *  main.c
 *  Author: Jonatan Schroeder
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "phreak.h"

#ifndef PART
#define PART 1
#endif

/* Largest line in the input file. */
#define MAXLINE 1000

const static char *signal_name[] = {
  "???", "connect_req", "connected", "busy", "disconnected" };

static entity str_to_entity(const char *str) {
  
  entity entity;
  switch (entity.type = str[0]) {
  case ENTITY_PHONE:
    entity.phone_number = str[2] - '0';
  case ENTITY_SWITCH:
    entity.switch_number = str[1] - '0';
  default: break;
  }
  return entity;
}

static connection str_to_connection(const char *str) {
  
  connection conn;
  conn.type = str ? str[0] : CONN_NONE;
  if (conn.type) {
    conn.switch_number = str[1] - '0';
    conn.phone_number = str[2] - '0';
  }
  return conn;
}

static void print_signal(entity from, entity to, signal signal) {
  
  printf("Signal %c", from.type);
  switch (from.type) {
  case ENTITY_PHONE:
    printf("%d%d", from.switch_number, from.phone_number); break;
  case ENTITY_SWITCH:
    printf("%d", from.switch_number); break;
  default: break;
  }
  printf(" to %c", to.type);
  switch (to.type) {
  case ENTITY_PHONE:
    printf("%d%d", to.switch_number, to.phone_number); break;
  case ENTITY_SWITCH:
    printf("%d", to.switch_number); break;
  default: break;
  }
  
  if (signal.type < 0 || signal.type >= NUM_POSSIBLE_SIGNALS) signal.type = SIGNAL_INVALID;
  
  printf(": %s %c%d%d", signal_name[signal.type],
	 signal.source.type, signal.source.switch_number, signal.source.phone_number);
  if (signal.destination.type)
    printf(" to %c%d%d", signal.destination.type,
	   signal.destination.switch_number, signal.destination.phone_number);
  printf("\n");
}

static inline void print_connection(connection conn, char conn_end) {
    if (conn.type == CONN_NONE)
      printf("   F ");
    else
      printf("%c%c%d%d ", conn.type, conn_end, conn.switch_number, conn.phone_number);
}

static inline void print_state(system_state *state) {
  
  int i;
  connection conn;
  
  printf("\n");
  
#if PART==1
  for (i = 0; i < NUM_TRUNK_PORTS; i++)
    printf("% 4d ", i);
  printf("\n");
  conn.type = CONN_TRUNK;
  conn.switch_number = 0;
  for (conn.trunk_index = 0; conn.trunk_index < NUM_TRUNK_PORTS;
       conn.trunk_index++) {
    print_connection(switch_current_connection(state, conn), 'S');
  }
  
  printf("\n");
  for (i = 0; i < NUM_PHONES_PER_SWITCH; i++)
    printf("% 4d ", i);
  printf("\n");
  conn.type = CONN_LINE;
  conn.switch_number = 0;
  for (conn.phone_number = 0; conn.phone_number < NUM_PHONES_PER_SWITCH;
       conn.phone_number++) {
    print_connection(switch_current_connection(state, conn), 'S');
  }
  printf("\n");
  
#endif
#if PART==2 || PART==3
  
  printf("   ");
  for (i = 0; i < NUM_TRUNKS; i++)
    printf("% 4d ", i);
  printf("\n");
  conn.type = CONN_TRUNK;
  for (conn.trunk_index = 0; conn.trunk_index < NUM_TRUNK_PORTS; conn.trunk_index++) {
    printf("% 2d ", conn.trunk_index);
    for (conn.switch_number = 0; conn.switch_number < NUM_TRUNKS; conn.switch_number++) {
      print_connection(root_current_connection(state, conn), 'R');
    }
    printf("\n");
  }
  
#endif
#if PART==3
  
  printf("   ");
  for (i = 0; i < NUM_TRUNKS; i++)
    printf("% 4d ", i);
  printf("\n");
  conn.type = CONN_TRUNK;
  for (conn.trunk_index = 0; conn.trunk_index < NUM_TRUNK_PORTS; conn.trunk_index++) {
    printf("% 2d ", conn.trunk_index);
    for (conn.switch_number = 0; conn.switch_number < NUM_TRUNKS; conn.switch_number++) {
      print_connection(switch_current_connection(state, conn), 'S');
    }
    printf("\n");
  }
  
  printf("   ");
  for (i = 0; i < NUM_TRUNKS; i++)
    printf("% 4d ", i);
  printf("\n");
  conn.type = CONN_LINE;
  for (conn.phone_number = 0; conn.phone_number < NUM_PHONES_PER_SWITCH; conn.phone_number++) {
    printf("% 2d ", conn.phone_number);
    for (conn.switch_number = 0; conn.switch_number < NUM_TRUNKS; conn.switch_number++) {
      print_connection(switch_current_connection(state, conn), 'S');
    }
    printf("\n");
  }
  
#endif
}

void forward_signal(system_state *state, entity from, entity to, signal signal) {
  
  print_signal(from, to, signal);
  
  switch(to.type) {
  case ENTITY_SWITCH:
#if PART!=2
    switch_process_signal(state, to.switch_number, from, signal);
#endif
    break;
  case ENTITY_ROOT:
#if PART!=1
    root_process_signal(state, from, signal);
#endif
    break;
  case ENTITY_PHONE: default:
    // do nothing
    break;
  }
}

int main(int argc, char *argv[]) {
  
  FILE *input;
  char *filename;
  char line[MAXLINE];
  char *command, *fromstr, *tostr, *signal_type, *source, *destination, *tokptr;
  system_state state;
  entity from, to;
  signal signal;
  int i;
  
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
  
  initialize_system_state(&state);
  
  while(fgets(line, MAXLINE, input)) {
    
    command = strtok_r(line, " \t\r\n", &tokptr);
    if (!command) continue;
    
    if (!strcasecmp(command, "SIGNAL")) {
      
      fromstr = strtok_r(NULL, " \t\r\n", &tokptr);
      strtok_r(NULL, " \t\r\n", &tokptr); // ignore "to"
      tostr = strtok_r(NULL, ": \t\r\n", &tokptr);
      signal_type = strtok_r(NULL, " \t\r\n", &tokptr);
      source = strtok_r(NULL, " \t\r\n", &tokptr);
      strtok_r(NULL, " \t\r\n", &tokptr); // ignore potential "to"
      destination = strtok_r(NULL, " \t\r\n", &tokptr);
      
      from = str_to_entity(fromstr);
      to = str_to_entity(tostr);
      signal.type = SIGNAL_INVALID;
      signal.source = str_to_connection(source);
      signal.destination = str_to_connection(destination);
      
      for (i = 0; i < NUM_POSSIBLE_SIGNALS; i++)
	if (!strcasecmp(signal_type, signal_name[i]))
	  signal.type = i;
      
      forward_signal(&state, from, to, signal);
      printf("\n");
    }
    else if (!strcasecmp(command, "QUIT")) {
      
      break;
    }
    else {
      
      printf("Invalid command: %s\n", command);
    }
  }
  
  print_state(&state);
  destroy_system_state(&state);
  
  if (ferror(input)) {
    perror("Could not read from input file");
    fclose(input);
    return 3;
  }
  
  fclose(input);
  
  return EXIT_SUCCESS;
}
