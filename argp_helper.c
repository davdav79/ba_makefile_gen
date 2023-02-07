#include "argp_helper.h"

/* Parse a single option. */
error_t parse_opt(int key, char *arg, struct argp_state *state) {
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    struct arguments *arguments = state->input;

    switch (key) {
        case 'p':
            arguments->path = arg;
            break;
        case 'o':
            arguments->output = arg;
            break;
        case 'c':
            arguments->object = 0;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}