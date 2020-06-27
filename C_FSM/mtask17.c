#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// some things for debugging
#ifdef DEBUG
#define debug(...)	{ fprintf(stderr, __VA_ARGS__); fflush(stderr); }
#else
#define debug(...)
#endif

#define INPUT_SIZE	100
#define NUM_STATES	21
#define NUM_CHARS	21

char char_map[NUM_CHARS] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', 'e', 'E', '-', '+', 'a', 'b', 'c', 'x', 'y', 'z' };
int final[NUM_STATES] = { 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0};
int arc[NUM_CHARS][NUM_STATES] = {
//0   1    2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20
{ 2,  2,  20,  4, -1, -1, -1, -1,  9,  9, 11, 11, 13, -1, 18,  9, 17, 11, 19, -1, -1}, //0
{ 2,  2,  20,  4, -1, -1, -1, -1,  9,  9, 11, 11, 13, -1, 18,  9, 17, 11, 19, -1, -1}, //1
{ 2,  2,  20,  4, -1, -1, -1, -1,  9,  9, 11, 11, 13, -1, 18,  9, 17, 11, 19, -1, -1}, //2
{ 2,  2,  20,  4, -1, -1, -1, -1,  9,  9, 11, 11, 13, -1, 18,  9, 17, 11, 19, -1, -1}, //3
{ 2,  2,  20,  4, -1, -1, -1, -1,  9,  9, 11, 11, 13, -1, 18,  9, 17, 11, 19, -1, -1}, //4
{ 2,  2,  20,  4, -1, -1, -1, -1,  9,  9, 11, 11, 13, -1, 18,  9, 17, 11, 19, -1, -1}, //5
{ 2,  2,  20,  4, -1, -1, -1, -1,  9,  9, 11, 11, 13, -1, 18,  9, 17, 11, 19, -1, -1}, //6
{ 2,  2,  20,  4, -1, -1, -1, -1,  9,  9, 11, 11, 13, -1, 18,  9, 17, 11, 19, -1, -1}, //7
{ 2,  2,  20,  4, -1, -1, -1, -1,  9,  9, 11, 11, 13, -1, 18,  9, 17, 11, 19, -1, -1}, //8
{ 2,  2,  20,  4, -1, -1, -1, -1,  9,  9, 11, 11, 13, -1, 18,  9, 17, 11, 19, -1, -1}, //9
{ 8,  -1,  8, -1, -1, -1, -1, -1, -1, 16, -1, 12, -1, -1,  8, -1, -1, -1, -1, -1, -1}, //.
{ 18, -1,  3, -1, -1, -1, -1, -1, 15, 10, -1, -1, -1, -1, -1, -1, 10, -1, -1, -1, -1}, //e
{ 18, -1,  3, -1, -1, -1, -1, -1, 15, 10, -1, -1, -1, -1, -1, -1, 10, -1, -1, -1, -1}, //E
{ 14, -1, -1, -1, -1, -1, -1, -1, -1, -1, 17, -1, -1, -1, -1, 19, -1, -1, -1, -1, -1}, //-
{ 14, -1, -1, -1, -1, -1, -1, -1, -1, -1, 17, -1, -1, -1, -1, 19, -1, -1, -1, -1, -1}, //+
{ 1,   1, -1, -1,  5,  6,  7, -1,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, //a
{ 1,   1, -1, -1,  5,  6,  7, -1,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, //b
{ 1,   1, -1, -1,  5,  6,  7, -1,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, //c
{ 1,   1, -1, -1,  5,  6,  7, -1,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, //x
{ 1,   1, -1, -1,  5,  6,  7, -1,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, //y
{ 1,   1, -1, -1,  5,  6,  7, -1,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}  //z
};

void display_fsm(void);
void get_opts(int argc, char *argv[], char **inf);
void usage(char *cmd);
int char_index(const char c);
bool find_token(char *line, int *loc, int *len);
bool apply_fsm(char *first_char, int *len);


int main(int argc, char *argv[])
{
	char *inf;					// name of file containing strings for the fsm
	FILE *input_fp = stdin;

	get_opts(argc, argv, &inf);

	if (inf) {
		// user wants strings read from a file instead of stdin
		errno = 0;
		input_fp = fopen(inf, "r");
		if (input_fp == NULL) {
			fprintf(stderr, "Unable to open %s for reading, errno %d\n", inf, errno);
			return 1;
		}
	}

#ifdef DEBUG
	display_fsm();
#endif

	char input[INPUT_SIZE];
	int loc, length;

	// prompt if reading from stdin
	if (input_fp == stdin)
		fprintf(stdout, "> ");

	while (fgets(input, INPUT_SIZE, input_fp) && *input != '\n') {

		// strip newline from the input
		input[strlen(input) - 1] = '\0';

		if (!find_token(input, &loc, &length)) {
			// token not found
			printf("\n\"%s\" does not contain a recognized token\n", input);
		}
		else {
			// token has size length and is at index loc in input
			printf("token \"");
			int i;
			for (i = loc; i<loc + length; i++)
				printf("%c", input[i]);
			printf("\" found at location %d\n", loc);
		}

		// prompt if reading from stdin
		if (input_fp == stdin)
			fprintf(stdout, "> ");
	}

	return 0;
}


void get_opts(int argc, char *argv[], char **inf)
{
	*inf = NULL;

	if (argc == 1)
		return;			// -f not specified

	if (argc != 3) {
		usage(argv[0]);
		return;
	}

	if (argv[1][0] != '-' || argv[1][1] != 'f') {
		// -f is not the second token on the command line
		usage(argv[0]);
		return;
	}

	// third argument is a data file
	*inf = argv[2];
	return;
}


void usage(char *cmd)
{
	printf("usage: %s [ -f inputfile ]\n", cmd);
	return;
}


void display_fsm()
{
	int i, j;

	printf("%d states, %d recognized characters\n", NUM_STATES, NUM_CHARS);
	printf("characters are");
	for (i = 0; i<NUM_CHARS; i++)
		printf(" '%c'", char_map[i]);
	printf("\n");
	printf("Transition table:\n");
	for (i = 0; i<NUM_CHARS; i++) {
		for (j = 0; j<NUM_STATES; j++) {
			printf("%4d", arc[i][j]);
		}
		printf("\n");
	}
	printf("Final state(s):");
	for (i = 0; i<NUM_STATES; i++) {
		if (final[i]) {
			printf(" %d", i);
		}
	}
	printf("\n\n");
}


int char_index(const char c)
{
	int i;
	for (i = 0; i<NUM_CHARS; i++) {
		if (char_map[i] == c) {
			return i;
		}
	}

	return -1;
}

bool find_token(char *line, int *loc, int *len)
{
	char *p = line;
	char *first_rec_char;
	bool found_token = false;
	int cindex = -1;

	// loop while not done and not end of line
	while (*p && !found_token) {
		debug("\tfind_token: Finding token in %s\n", p);
		// find first recognized character
		while (*p) {
			cindex = char_index(*p);
			if (cindex >= 0)
				break;
			p++;
		}

		if (cindex < 0) {
			debug("\tfind_token: no recognized characters in %s\n", line);
			return false;		// no recognized characters, so no token
		}

		debug("\tfind_token: first_rec_char at line index %d ('%c')\n", (int)(p - line), *p);
		first_rec_char = p;

		found_token = apply_fsm(p, len);

		if (!found_token) {
			debug("\tfind_token: token not found in %s, look again\n", p);
			p = first_rec_char + 1;
		}
	}

	debug("\tfind_token: end of input line, found_token %d, length %d\n", found_token, *len);

	if (!found_token)
		return false;

	*loc = (int)(first_rec_char - line);

	return true;
}


bool apply_fsm(char *first_char, int *len)
{
	// we know p is a recognized character or we wouldn't be here
	int state = 0;
	char *p = first_char;
	int last_final_state = -1;
	char *last_final_char;

	debug("apply_fsm: %s\n", first_char);

	while (*p && state != -1) {
		debug("apply_fsm: processing character '%c'\n", *p);
		int cindex = char_index(*p);
		if (cindex < 0) {
			debug("apply_fsm: '%c' not a recognized character\n", *p);
			break;		// end of token (char not recognized)
		}
		debug("apply_fsm: current state %d, next_state %d (cindex %d)\n", state, arc[cindex][state], cindex);
		state = arc[cindex][state];
		if (state != -1) {
			if (final[state]) {
				last_final_state = state;
				last_final_char = p;
			}
			p++;
		}
	}

	if (last_final_state == -1)
		return false;		// we are done and did not pass through a final state, no token found

							// token found

							// the last final state we made it to is the end of the recognized token
	*len = last_final_char - first_char + 1;
	return true;
}
