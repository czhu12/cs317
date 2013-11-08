/*
 *  ip_route.h
 *  Author: Jonatan Schroeder
 */

#ifndef _IP_ROUTE_H_
#define _IP_ROUTE_H_

#include <stdint.h>

#include "capacity.h"

typedef struct router_state {
  
} *router_state;

router_state initialize_router(void);
void process_update(router_state *state, uint32_t ip, uint8_t netsize,
		    int nic, unsigned int metric, unsigned int update_id);
void destroy_router(router_state state);

#endif
