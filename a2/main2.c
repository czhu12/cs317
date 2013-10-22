/*
 *  main2.c
 *  Author: Jonatan Schroeder
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <inttypes.h>

#include "eth_switch2.h"

/* Largest line in the input file. */
#define MAXLINE 1000
//int main(int argc, char *argv[]){
//  FILE *st_output;
//  char *filename;
//  char line[MAXLINE];
//  char *token, *tokptr, *strtok_1p;
//  uint16_t frameid;
//  int i, n;
//  bounded_queue queue;
//	uint16_t deq;
//	
//	queue = create_bounded_queue();
//	enqueue_bounded(queue, 10);
//	enqueue_bounded(queue, 2);
//	enqueue_bounded(queue, 230);
//	deq = dequeue_bounded(queue);
//	deq = dequeue_bounded(queue);
//  //printf("-%03"PRId16" ", deq);
//
//	printf("\nFRONT %d, BACK: %d\n", queue->front, queue->back);
//
//}

int main(int argc, char *argv[]) {
  
  FILE *st_output;
  char *filename;
  char line[MAXLINE];
  char *token, *tokptr, *strtok_1p;
  uint16_t frameid;
  int i, n, num_queues = 0;
  bounded_queue *queues;
  
  if (argc == 1) {
    filename = "buffer_state.txt";
  }
  else {
    filename = argv[1];
    if (!strcmp(filename, "-"))
      filename = "/dev/stdout";
  }
  
  if (fgets(line, MAXLINE, stdin)) {
    
    st_output = fopen(filename, "w");
    if (!st_output) {
      perror("Could not open file for writing");
      return 2;
    }
    
    // Prints the header. \n is included in original line, so not included here.
    printf("%s", line);
    fprintf(st_output, "%s", line);
    
    // Counts how many queues should be created
    if (strtok_r(line, " \t\r\n", &tokptr)) {
      do num_queues++;
      while (strtok_r(NULL, " \t\r\n", &tokptr));
    }
    
    // Creates dynamic array of queues.
    queues = (bounded_queue *) calloc(num_queues, sizeof(bounded_queue));
    for (i = 0; i < num_queues; i++)
      queues[i] = create_bounded_queue();
    
    while (fgets(line, MAXLINE, stdin)) {
      
      strtok_1p = line;
      for (i = 0; i < num_queues; i++) {
	
	token = strtok_r(strtok_1p, " \t\r\n", &tokptr);
	if (!token) break;
	strtok_1p = NULL;
	
	if (token[0] == '+') {
	  
	  frameid = strtol(token + 1, NULL, 10);
	  // Ignore output
	  enqueue_bounded(queues[i], frameid);
	}
	else if (!strcasecmp(token, "DEQ")) {
	  
	  if (queue_is_empty(queues[i]))
	    printf(" --- ");
	  else
	    printf("-%03"PRId16" ", dequeue_bounded(queues[i]));
	}
	else if (!strcmp(token, "---")) {
	  // Do nothing
	}
	else {
	  
	  fprintf(stderr, "Invalid token: %s\n", token);
	}
      }
      
      printf("\n");
    }
    
    for (n = 0; n < BUFFER_SIZE; n++) {
      
      for (i = 0; i < num_queues; i++)
	if (queue_is_empty(queues[i]))
	  fprintf(st_output, " --- ");
	else
	  fprintf(st_output, " %03"PRId16" ", dequeue_bounded(queues[i]));
      fprintf(st_output, "\n");
    }
    
    for (i = 0; i < num_queues; i++)
      destroy_bounded_queue(queues[i]);
    free(queues);
  }
  
  if (ferror(st_output)) {
    perror("Could not read from input file");
    fclose(st_output);
    return 3;
  }
  
  fclose(st_output);
  
  return EXIT_SUCCESS;
}
