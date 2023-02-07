#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mle.h"

typedef struct token_s {
	char str[16];
	struct token_s* next;
} token_t;

void free_tokens(token_t* first) {

}
int is_ws(char c) {
	return (c == ' ' || c == '\t' || c == '\n');
}
int is_letter(char c) {
	return (c <= 'Z' && c >= 'A') || (c <= 'z' && c >= 'a') ||  (c <= '9' && c >= '0');
}
int is_symbol(char c) {
	return !is_letter(c) && !is_ws(c) && c > 32/*Before that there are specials*/;
}

token_t* tokenize(char* data) {
	token_t* first = malloc(sizeof(token_t));

	char writing = 0;
	char writing_word = 0;
	token_t* last = first;
	int str_i = 0;

	for (;; data++) {
		// If we are not and this isn't whitespace start writing.
		if (!is_ws(*data) && !writing) {
			writing_word = is_letter(*data);
			writing = 1;
		}
		// If we are currently writing a word
		if (writing) {
			// If we hit whitespace 
			if (is_ws(*data) || !*data || *data == EOF) {
				last->str[str_i] = 0;
				if (!*data || *data == EOF)
					break;
				last->next = malloc(sizeof(token_t));
				last = last->next;
				last->next = NULL;
				writing = 0;
				str_i = 0;
			}
			else {
				// Otherwise, if we hit a different kind of token
				if (writing_word != is_letter(*data)) {
					writing_word = is_letter(*data);
					last->str[str_i] = 0;
					last->next = malloc(sizeof(token_t));
					last = last->next;
					last->next = NULL;
					str_i = 0;
				}
				last->str[str_i++] = *data;
			}
		}
	}

	return first;
}

typedef struct {
	char* name;
} entry_t;

// Returns malloc() buffer of a compiled version of data
char* compile(char* data) {
	int last;
	for (token_t* t = tokenize(data); t; t = t->next) {
		if (strcmp(str, "layer")) {

		}
	}
}

// Returns malloc() buffer of a reversed(decompiled) version of data
char* reverse(const char* data) {

}

int main(int args_n, char** args) {
	compile("  Hello=world =2 world =");
	return 0;
}
