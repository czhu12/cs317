/*
 * ip_route.c
 * Author:
 */

#include <stdio.h>
#include <stdlib.h>

#include "ip_route.h"

static inline void print_advertisement(uint32_t ip, uint8_t netsize, int nic,
                                       unsigned int metric, unsigned int update_id) {
  printf("A %u.%u.%u.%u/%u %u %u\n"
         "T %u.%u.%u.%u/%u %d\n",
         (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF, netsize, metric, update_id,
         (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF, netsize, nic);
}

/* This function initializes the state of the router.
 */
router_state initialize_router(void) {
  
  // TODO Implement this function.
}

/* This function is called for every line corresponding to a routing
 * update. The IP is represented as a 32-bit unsigned integer. The
 * netsize parameter corresponds to the size of the prefix
 * corresponding to the network part of the IP address. The metric
 * corresponds to the value informed by the neighboring router, and
 * does not include the cost to reach that router (which is assumed to
 * be always one). If the update triggers an advertisement, this
 * function prints the advertisement in the standard output.
 */
void process_update(router_state *state, uint32_t ip, uint8_t netsize,
		    int nic, unsigned int metric, unsigned int update_id) {
	unsigned int new_metric = metric + 1;
  
  // TODO Implement this function.
}

/* Destroys all memory dynamically allocated through this state (such
 * as the forwarding table) and frees all resources used by the
 * router.
 */
void destroy_router(router_state state) {
  
  // TODO Implement this function.
}

