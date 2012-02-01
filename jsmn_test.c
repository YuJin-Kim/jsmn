#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsmn.c"

static int test_passed = 0;
static int test_failed = 0;

/* Terminate current test with error */
#define fail()	return __LINE__

/* Successfull end of the test case */
#define done() return 0

/* Check single condition */
#define check(cond) do { if (!(cond)) fail(); } while (0)

/* Test runner */
static void test(int (*func)(void), const char *name) {
	int r = func();
	if (r == 0) {
		test_passed++;
	} else {
		test_failed++;
		printf("FAILED: %s (at line %d)\n", name, r);
	}
}

#define TOKEN_EQ(t, tok_start, tok_end, tok_type) \
	((t).start == tok_start \
	 && (t).end == tok_end  \
	 && (t).type == (tok_type))

#define TOKEN_STIRNG(js, t, s) \
	(strncmp(js+(t).start, s, (t).end - (t).start) == 0 \
	 && strlen(s) == (t).end - (t).start)

#define TOKEN_PRINT(t) \
	printf("start: %d, end: %d, type: %d\n", (t).start, (t).end, (t).type)



int test_simple() {
	const char *js;
	int r;
	jsmn_parser p;
	jsmntok_t tokens[10];

	js = "{\"a\": 0}";

	jsmn_init(&p);
	r = jsmn_parse(&p, js, tokens, 10);
	check(r == JSMN_SUCCESS);
	check(TOKEN_EQ(tokens[0], 0, 8, JSMN_OBJECT));
	check(TOKEN_EQ(tokens[1], 2, 3, JSMN_STRING));
	check(TOKEN_EQ(tokens[2], 6, 7, JSMN_PRIMITIVE));

	check(TOKEN_STIRNG(js, tokens[0], js));
	check(TOKEN_STIRNG(js, tokens[1], "a"));
	check(TOKEN_STIRNG(js, tokens[2], "0"));

	return 0;
}

int test_primitive() {
	int r;
	jsmn_parser p;
	jsmntok_t tok[10];
	const char *js;

	js = "\"boolVar\" : true";
	jsmn_init(&p);
	r = jsmn_parse(&p, js, tok, 10);
	check(r == JSMN_SUCCESS && tok[0].type == JSMN_STRING 
			&& tok[1].type == JSMN_PRIMITIVE);
	check(TOKEN_STIRNG(js, tok[0], "boolVar"));
	check(TOKEN_STIRNG(js, tok[1], "true"));

	js = "\"boolVar\" : false";
	jsmn_init(&p);
	r = jsmn_parse(&p, js, tok, 10);
	check(r == JSMN_SUCCESS && tok[0].type == JSMN_STRING 
			&& tok[1].type == JSMN_PRIMITIVE);
	check(TOKEN_STIRNG(js, tok[0], "boolVar"));
	check(TOKEN_STIRNG(js, tok[1], "false"));

	js = "\"intVar\" : 12345";
	jsmn_init(&p);
	r = jsmn_parse(&p, js, tok, 10);
	check(r == JSMN_SUCCESS && tok[0].type == JSMN_STRING 
			&& tok[1].type == JSMN_PRIMITIVE);
	check(TOKEN_STIRNG(js, tok[0], "intVar"));
	check(TOKEN_STIRNG(js, tok[1], "12345"));

	js = "\"floatVar\" : 12.345";
	jsmn_init(&p);
	r = jsmn_parse(&p, js, tok, 10);
	check(r == JSMN_SUCCESS && tok[0].type == JSMN_STRING 
			&& tok[1].type == JSMN_PRIMITIVE);
	check(TOKEN_STIRNG(js, tok[0], "floatVar"));
	check(TOKEN_STIRNG(js, tok[1], "12.345"));

	js = "\"nullVar\" : null";
	jsmn_init(&p);
	r = jsmn_parse(&p, js, tok, 10);
	check(r == JSMN_SUCCESS && tok[0].type == JSMN_STRING 
			&& tok[1].type == JSMN_PRIMITIVE);
	check(TOKEN_STIRNG(js, tok[0], "nullVar"));
	check(TOKEN_STIRNG(js, tok[1], "null"));

	js = "\"strVar\" : \"hello world\"";
	jsmn_init(&p);
	r = jsmn_parse(&p, js, tok, 10);
	check(r == JSMN_SUCCESS && tok[0].type == JSMN_STRING 
			&& tok[1].type == JSMN_STRING);
	check(TOKEN_STIRNG(js, tok[0], "strVar"));
	check(TOKEN_STIRNG(js, tok[1], "hello world"));

	return 0;
}

int main() {
	test(test_simple, "general test for a simple JSON string");
	test(test_primitive, "test primitive JSON data types");
	printf("\nPASSED: %d\nFAILED: %d\n", test_passed, test_failed);
	return 0;
}
