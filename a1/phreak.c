/*
 *  phreak.c
 *  Author: Jonatan Schroeder
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "phreak.h"

/* Initialize the system with a state where no lines are connected or in use. */
int find_next_trunk_index(system_state *state, int switch_number){
	int i;
	int index = -1;
	for(i = 0; i < NUM_TRUNK_PORTS; i++){
		if(state->switch_state[switch_number].trunk_ports[i].type == 0){
			//found empty port
			index = i;
			break;
		}
	}
	return index;
}
int find_root_trunk_index(system_state *state, int trunk_number){
	int i;
	int result = -1;
	for(i = 0; i < NUM_TRUNK_PORTS; i++){
		if(state->root_state.trunk_ports[trunk_number][i].type == 0){
			result = i;
			break;
		}
	}
	return result;
}

void initialize_system_state(system_state *state) {
	int i;
	int j;
	int x;
	int y;
	
	for(i = 0; i < NUM_TRUNKS; i++){
		for(x = 0; x < NUM_TRUNK_PORTS; x++){
			state->switch_state[i].trunk_ports[x].type = 0;
		}
		for(y = 0; y < NUM_PHONES_PER_SWITCH; y++){
			state->switch_state[i].line_ports[y].type = 0;
		}
	}
	for(i = 0; i < NUM_TRUNKS; i++){
		for(j = 0; j < NUM_TRUNK_PORTS; j++){
			state->root_state.trunk_ports[i][j].type = 0;
		}
	}
}

/* Free potential dynamically allocated space and close any resources no longer in use. */
void destroy_system_state(system_state *state) {
  // TODO
}

/* Handles an incoming signal received by the switch identified by
   'switch_number', coming from the entity represented by the
   parameter 'from' (the root trunk or a phone). If the result of this
   function is the generation of a new signal, this function will call
   forward_signal with appropriate parameters. */
void switch_process_signal(system_state *state, int switch_number, entity from, signal recv_signal) {
  // TODO Signal R to S0: connect_req T03 to P05
	switch(recv_signal.type){
		case SIGNAL_CONNECT_REQUEST:
			//first check to make sure that the phone in question isnt already active
			if(from.type == 'P'){
				if(state->switch_state[switch_number].line_ports[from.phone_number].type != 0){
					entity current;
					current.switch_number = switch_number;
					current.type = 'S';
					
					recv_signal.type = SIGNAL_BUSY;

					forward_signal(state, current, from, recv_signal);
					return;
				}
			}
			//has to check if signal destination switch is on the same switch as the one currently at
			if(switch_number == recv_signal.destination.switch_number){
				//found the correct switch
				if(state->switch_state[switch_number].line_ports[recv_signal.destination.phone_number].type != 0){
					entity current;
					current.switch_number = switch_number;
					current.type='S';
					recv_signal.type = SIGNAL_BUSY;
					forward_signal(state, current, from, recv_signal);
					return;
				}

				if(recv_signal.source.type == recv_signal.destination.type){//check if its line port to line port
					//connecting line ports
					state->switch_state[switch_number].line_ports[recv_signal.source.phone_number] = recv_signal.destination;
					state->switch_state[switch_number].line_ports[recv_signal.destination.phone_number] = recv_signal.source;
				}else{ //connect specified trunk line to destination
					
					state->switch_state[switch_number].trunk_ports[recv_signal.source.trunk_index] = recv_signal.destination;
					state->switch_state[switch_number].line_ports[recv_signal.destination.phone_number] = recv_signal.source;
				}

				entity current;
				current.switch_number = switch_number;
				current.type='S';

				recv_signal.type = SIGNAL_CONNECTED;
				forward_signal(state, current, from, recv_signal);
				return;
			}else{
				if(from.type == 'P'){//(if the signal came from the root, it would be up there)
					//has to connect phone line to trunk line
					int trunk_index = find_next_trunk_index(state, switch_number);
					if(trunk_index == -1){ // no more trunk lines
						entity current;
						current.type = 'S';
						current.switch_number = switch_number;
						recv_signal.type = SIGNAL_BUSY;

						forward_signal(state, current, from, recv_signal);
						return;
					}
					
					state->switch_state[switch_number].line_ports[recv_signal.source.phone_number].type='T';
					state->switch_state[switch_number].line_ports[recv_signal.source.phone_number].trunk_index = trunk_index;
					state->switch_state[switch_number].line_ports[recv_signal.source.phone_number].switch_number = switch_number;

					state->switch_state[switch_number].trunk_ports[trunk_index].type='L';
					state->switch_state[switch_number].trunk_ports[trunk_index].phone_number = from.phone_number;
					//state->switch_state[switch_number].trunk_ports[trunk_index].phone_number = recv_signal.destination.phone_number;
					state->switch_state[switch_number].trunk_ports[trunk_index].switch_number = switch_number;
					//
					entity current;
					current.type='S';
					current.switch_number = switch_number;

					entity to; 
					to.type='R';
					
					recv_signal.source = state->switch_state[switch_number].line_ports[recv_signal.source.phone_number];

					forward_signal(state, current, to, recv_signal);
					return;
					//signal sent from Phone which means that it will be for sure Signal Pxx to Sx: connect_req Lyy to Lxx
					//create connection from Lyy 
				}
			}
		break;
		case SIGNAL_CONNECTED:
			if(from.type == 'R'){

				connection con = switch_current_connection(state, recv_signal.source);

				entity from;
				from.switch_number = switch_number;
				from.type = 'S';

				entity to;
				to.phone_number = con.phone_number;
				to.switch_number = switch_number;
				to.type='P';

				recv_signal.source = con;
				forward_signal(state, from, to, recv_signal);
				return;
			}
				
		break;
		case SIGNAL_BUSY:
			if( from.type == 'R'){
				entity current;
				current.type='S';
				current.switch_number = switch_number;

				connection conn = switch_current_connection(state, recv_signal.source);

				int trunk_index = recv_signal.source.trunk_index;
				int line_index = conn.phone_number;

				state->switch_state[switch_number].trunk_ports[trunk_index].type = 0;
				state->switch_state[switch_number].line_ports[line_index].type = 0;

				entity next;
				next.type = 'P';
				next.phone_number = conn.phone_number;
				next.switch_number = switch_number;

				recv_signal.source = conn;
				forward_signal(state, current, next, recv_signal);
				return;
			}
		break;
		case SIGNAL_DISCONNECTED:
			if(from.type == 'R'){
				connection conn = switch_current_connection(state, recv_signal.source);
				entity current;
				current.type = 'S';
				current.switch_number = switch_number;
				
				entity phone;
				phone.type = 'P';
				phone.phone_number = conn.phone_number;
				phone.switch_number = switch_number;
				
				int trunk_index = recv_signal.source.trunk_index;
				int line_index = conn.phone_number;
				state->switch_state[switch_number].trunk_ports[trunk_index].type = 0;
				state->switch_state[switch_number].line_ports[line_index].type = 0;
				recv_signal.source = conn;
				forward_signal(state, current, phone, recv_signal);
			}else if(from.type == 'P'){
				entity current;
				current.type = 'S';
				current.switch_number = switch_number;

				connection conn = switch_current_connection(state, recv_signal.source);//does this assign conn to mem address?
				entity next_entity;

				if(conn.type == 'T'){
					//in this case, must send up to root
					next_entity.type = 'R';
					int trunk_port_index = conn.trunk_index;
					int line_port_index = recv_signal.source.phone_number;
					state->switch_state[switch_number].line_ports[line_port_index].type = 0;
					state->switch_state[switch_number].trunk_ports[trunk_port_index].type = 0;
					recv_signal.source = conn;
					forward_signal(state, current, next_entity, recv_signal);
				}
				if(conn.type == 'L'){
					next_entity.type = 'P';
					next_entity.phone_number = conn.phone_number;
					next_entity.switch_number = switch_number;
					
					int line_index_1 = recv_signal.source.phone_number;
					int line_index_2 = conn.phone_number;
					state->switch_state[switch_number].line_ports[line_index_1].type = 0;
					state->switch_state[switch_number].line_ports[line_index_2].type = 0;
					recv_signal.source = conn;
					forward_signal(state, current, next_entity, recv_signal);
					return;
				}
				
			}
		break;
	}
}

/* Returns the status of the current connection linked to connection
   conn in switch conn.switch_number. If the connection is not
   currently linked to anything, returns a connection with type
   CONN_NONE. */
connection switch_current_connection(system_state *state, connection conn) {
  // TODO
	if(conn.type == 'T'){
		return state->switch_state[conn.switch_number].trunk_ports[conn.phone_number];
	}	
	else{ //conn.type = 'L'
		return state->switch_state[conn.switch_number].line_ports[conn.trunk_index];
	}	
}

/* Handles an incoming signal received by the root switch, coming from
   the entity represented by the parameter 'from' (assumed to be a
   switch). If the result of this function is the generation of a new
   signal, this function will call forward_signal with appropriate
   parameters. */
void root_process_signal(system_state *state, entity from, signal recv_signal) {
	entity root;
	root.type = 'R';
	switch(recv_signal.type){
		case SIGNAL_CONNECT_REQUEST:
			{
				int source_switch_number = recv_signal.source.switch_number;
				int source_trunk_index = recv_signal.source.trunk_index;
				int dest_switch_number = recv_signal.destination.switch_number;
				int dest_trunk_index = find_root_trunk_index(state, dest_switch_number);
				if(dest_trunk_index == -1){
					recv_signal.type = SIGNAL_BUSY;
					forward_signal(state, root, from, recv_signal);
					return;
				}

				state->root_state.trunk_ports[dest_switch_number][dest_trunk_index].type = 'T';
				state->root_state.trunk_ports[dest_switch_number][dest_trunk_index].trunk_index = source_trunk_index;
				state->root_state.trunk_ports[dest_switch_number][dest_trunk_index].switch_number = source_switch_number;

				state->root_state.trunk_ports[source_switch_number][source_trunk_index].type = 'T';
				state->root_state.trunk_ports[source_switch_number][source_trunk_index].trunk_index = dest_trunk_index;
				state->root_state.trunk_ports[source_switch_number][source_trunk_index].switch_number = dest_switch_number;
				entity next;
				next.type='S';
				next.switch_number = dest_switch_number;
				recv_signal.source = state->root_state.trunk_ports[source_switch_number][source_trunk_index];
				forward_signal(state, root, next, recv_signal);
				return;
			}
		break;
		case SIGNAL_CONNECTED:
			{
				int recv_switch_number = recv_signal.source.switch_number;
				int recv_trunk_index = recv_signal.source.trunk_index;
				connection conn = state->root_state.trunk_ports[recv_switch_number][recv_trunk_index];
				int back_switch_number = conn.switch_number;
				int back_trunk_index = conn.trunk_index;
				entity next_switch;
				next_switch.type = 'S';
				next_switch.switch_number = back_switch_number;
				recv_signal.source = conn;
				forward_signal(state, root, next_switch, recv_signal);
				return;
			}
		break;
		case SIGNAL_BUSY:
			{
				int recv_switch_number = recv_signal.source.switch_number;
				int recv_trunk_index = recv_signal.source.trunk_index;

				connection conn = state->root_state.trunk_ports[recv_switch_number][recv_trunk_index];
				int back_switch_number = conn.switch_number;
				int back_trunk_index = conn.trunk_index;

				//clear connections
				state->root_state.trunk_ports[recv_switch_number][recv_trunk_index].type = 0;
				state->root_state.trunk_ports[back_switch_number][back_trunk_index].type = 0;

				entity next_switch;
				next_switch.type = 'S';
				next_switch.switch_number = back_switch_number;
				
				recv_signal.source = conn;

				forward_signal(state, root, next_switch, recv_signal);
				return;
			}
		break;
		case SIGNAL_DISCONNECTED:
			{
				int recv_switch_number = recv_signal.source.switch_number;
				int recv_trunk_index = recv_signal.source.trunk_index;

				connection conn = state->root_state.trunk_ports[recv_switch_number][recv_trunk_index];
				int back_switch_number = conn.switch_number;
				int back_trunk_index = conn.trunk_index;

				state->root_state.trunk_ports[recv_switch_number][recv_trunk_index].type = 0;
				state->root_state.trunk_ports[back_switch_number][back_trunk_index].type = 0;

				entity next_switch;
				next_switch.type = 'S';
				next_switch.switch_number = back_switch_number;

				recv_signal.source = conn;
				forward_signal(state, root, next_switch, recv_signal);
				return;
			}
		break;
	}
  // TODO
}

/* Returns the status of the current connection linked to connection
   conn in the root. If the connection is not currently linked to
   anything, returns a connection with type CONN_NONE. */
connection root_current_connection(system_state *state, connection conn) {
	connection c = state->root_state.trunk_ports[conn.switch_number][conn.trunk_index];
	return c;
}

