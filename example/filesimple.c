#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

// static const char *JSON_STRING =
// 	"{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
// 	"\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";
//
// static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
// 	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
// 			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
// 		return 0;
// 	}
// 	return -1;
// }

char * readJSONFile() {

	FILE *f = fopen("data.json","r");
	char * JSON_STRING;
	char str[256];
	int count = 0;

	JSON_STRING = (char *)malloc(sizeof(str));

	while (1) {
		fgets(str, 256, f);
		if (feof(f)) break;
		count+= strlen(str)+1;
		JSON_STRING = (char*)realloc(JSON_STRING, sizeof(char)*count);
		strcat(JSON_STRING, str);
	}

	fclose(f);

	return JSON_STRING;

}

void jsonNameList(char * jsonstr, jsmntok_t *t, int tokcount, int * nameTokIndex) {
	int i, count = 1;

	printf("******* Name List *******\n");
	for (i = 1; i < tokcount; i++)
		if (t[i].type == JSMN_STRING && t[i].size > 0) {
			printf("[NAME%2d]: %.*s\n", count, t[i].end-t[i].start,
						jsonstr + t[i].start);
						count++;
	}

}
	// int i, count = 1;
	//
	// for (i = 1; i<tokcount && count<=100; i++)
	// 	if (t[i].size > 0 && t[i].type == JSMN_STRING) {
	// 		*(nameTokIndex+count-1) = i;
	// 		count++;
	// 	}
// }

void printNameList (char * jsonstr, jsmntok_t * t, int * nameTokIndex) {
	int i;

	printf("******* Name List *******\n");
	for (i = 0; *(nameTokIndex+i)!=0; i++) {
		printf("[NAME %d]: %.*s\n", i+1, t[*(nameTokIndex+i)].end-t[*(nameTokIndex+i)].start,
					jsonstr + t[*(nameTokIndex+i)].start);
	}
}

int main() {

	// int * nameTokIndex = (int *)malloc(sizeof(int)*100);
	char * str_example = (char *)malloc(sizeof(readJSONFile())+1);
	str_example = readJSONFile();
	// printf("%s", str_example);

	int i;
	int r;
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */

	jsmn_init(&p);
	r = jsmn_parse(&p, str_example, strlen(str_example), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("Object expected\n");
		return 1;
	}

	int * nameTokIndex = (int *)malloc(sizeof(int)*100);
	jsonNameList(str_example, t, r, nameTokIndex);
	//printNameList(str_example, t, nameTokIndex);

	// /* Loop over all keys of the root object */
	// for (i = 1; i < r; i++) {
	// 	if (jsoneq(str_example, &t[i], "name") == 0) {
	// 		/* We may use strndup() to fetch string value */
	// 		printf("- name: %.*s\n", t[i+1].end-t[i+1].start,
	// 				str_example + t[i+1].start);
	// 		i++;
	// 	} else if (jsoneq(str_example, &t[i], "keywords") == 0) {
	// 		/* We may additionally check if the value is either "true" or "false" */
	// 		printf("- keywords: %.*s\n", t[i+1].end-t[i+1].start,
	// 				str_example + t[i+1].start);
	// 		i++;
	// 	} else if (jsoneq(str_example, &t[i], "description") == 0) {
	// 		/* We may want to do strtol() here to get numeric value */
	// 		printf("- UID: %.*s\n", t[i+1].end-t[i+1].start,
	// 				str_example + t[i+1].start);
	// 		i++;
	// 	} else if (jsoneq(str_example, &t[i], "examples") == 0) {
	// 		int j;
	// 		printf("- examples:\n");
	// 		if (t[i+1].type != JSMN_ARRAY) {
	// 			continue; /* We expect groups to be an array of strings */
	// 		}
	// 		for (j = 0; j < t[i+1].size; j++) {
	// 			jsmntok_t *g = &t[i+j+2];
	// 			printf("  * %.*s\n", g->end - g->start, str_example + g->start);
	// 		}
	// 		i += t[i+1].size + 1;
	// 	} else {
	// 		continue;
	// 		printf("Unexpected key: %.*s\n", t[i].end-t[i].start,
	// 				str_example + t[i].start);
	// 	}
	// }

	return EXIT_SUCCESS;
}
