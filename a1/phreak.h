/*
 *  phreak.h
 *  Author: Jonatan Schroeder
 */

#ifndef _PHREAK_H_
#define _PHREAK_H_

#include "phreak_limits.h"
typedef struct connection {
  enum { CONN_NONE = 0, CONN_TRUNK = 'T', CONN_LINE = 'L' } type;
  int switch_number;
  union { 
    int phone_number;
    int trunk_index;
  };
} connection;

//this represents a switch, root, or a phone
typedef struct entity {
  enum { ENTITY_NONE = 0, ENTITY_PHONE = 'P', ENTITY_SWITCH = 'S', ENTITY_ROOT = 'R' } type;
  int switch_number;
  int phone_number;
} entity;

//possible signals: mainly busy, disconnected, connected
typedef struct signal {
  // NUM_POSSIBLE_SIGNALS added as a way to count possible signals, it's not a valid signal
  enum { SIGNAL_INVALID = 0, SIGNAL_CONNECT_REQUEST, SIGNAL_CONNECTED,
	 SIGNAL_BUSY, SIGNAL_DISCONNECTED, NUM_POSSIBLE_SIGNALS } type;
  connection source;
  connection destination; // Should be ignored in a DISCONNECT signal
} signal;

//represents the state of the root? i should pointers to connection structs??

typedef struct root_state {
  // TODO Fill with information required for the state of the ROOT.
	connection trunk_ports[NUM_TRUNKS][NUM_TRUNK_PORTS];
} root_state;

//state of switches. Pointer to all connections?
typedef struct switch_state {
	connection trunk_ports[NUM_TRUNK_PORTS];
	connection line_ports[NUM_PHONES_PER_SWITCH];
} switch_state;


//state of system
typedef struct system_state {
  
  root_state root_state;
  switch_state switch_state[NUM_TRUNKS];
  
} system_state;

// Non-static functions in main.c
void forward_signal(system_state *state, entity from, entity to, signal signal);

// Non-static functions in phreak.c
void initialize_system_state(system_state *state);
void destroy_system_state(system_state *state);
void switch_process_signal(system_state *state, int switch_number,
			   entity from, signal recv_signal);
connection switch_current_connection(system_state *state, connection conn);
void root_process_signal(system_state *state, entity from, signal recv_signal);
connection root_current_connection(system_state *state, connection conn);

//my stuff
int find_next_trunk_index(system_state *state, int switch_number);
int find_root_trunk_index(system_state *state, int trunk_number);

#endif
