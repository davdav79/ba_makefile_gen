#pragma once
#include <argp.h>

/* The options we understand. */
static struct argp_option options[] = {
    {"output", 'o', "NAME", 0, "Write to a different File than Makefile",0},
    {"path", 'p', "PATH", 0, "Path to the project from whicht the Makefile is to be generated",0},
    {"object", 'c', 0,0, "Write source files as *.c instead of *.o to the Makefile",0},
    {0}
};
/* Program documentation. */
static char doc[] = "A simple tool to generate Makefiles of c Projects.";

/* A description of the arguments we accept. */
static char args_doc[] = "";

error_t parse_opt(int key, char *arg, struct argp_state *state);

/* Used by main to communicate with parse_opt. */
struct arguments {
    int object;
    char *path;
    char *output;
};
