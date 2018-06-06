#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"
#include "../productlist.h"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

// static const char *JSON_STRING =
// 	"{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
// 	"\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";
//
static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

char * readJSONFile() {
	char * JSON_STRING;
	char filename[30];
	char str[256];
	int count = 0;
	FILE *f;

	// printf("원하는 파일명 입력: ");
	// scanf("%s", filename);
	// strcat(filename,".json");

	if ((f = fopen("data4.json","r")) == NULL) {
		printf("%s 파일이 존재하지 않습니다.\n", filename);
		return NULL;
	}

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

int * jsonObjectList(char * jsonstr, jsmntok_t * t, int tokcount) {
	int i, count = 1; // 반복문을 위한 변수 i, Name count를 위한 변수 count 생성 및 1로 초기

	int * objectTokIndex = (int *)malloc(sizeof(int)*count);

	if (t[0].type == JSMN_OBJECT && t[2].type == JSMN_ARRAY) {
		for (i = 0; i < tokcount; i++)
			if(t[i].parent == 2) {
				*(objectTokIndex+count-1) = i;
				count++;
				objectTokIndex = (int *)realloc(objectTokIndex,sizeof(int)*(count+1));
			}
	}
	else if (t[0].type == JSMN_OBJECT) {
		for (i = 0; i < tokcount; i++)
			if(t[i].parent == -1) {
				*(objectTokIndex+count-1) = i;
				count++;
				objectTokIndex = (int *)realloc(objectTokIndex,sizeof(int)*(count+1));
			}
	}
	else {
		for (i = 0; i < tokcount; i++)
			if(t[i].parent == 0) {
				*(objectTokIndex+count-1) = i;
				count++;
				objectTokIndex = (int *)realloc(objectTokIndex,sizeof(int)*(count+1));
			}
	}

	*(objectTokIndex+count-1) = i;
	*(objectTokIndex+count) = 0;

	return objectTokIndex;
}

int jsonNameList(char * jsonstr, jsmntok_t * t, int tokcount, NameTokenInfo * nameTokenInfo) {
	int i = 0, j, count = 1;
	int * objectTokIndex = jsonObjectList(jsonstr, t, tokcount);

	do {
		for (j = 1; j < tokcount; j++)
			if(t[j].parent == *(objectTokIndex+i)) {
				(nameTokenInfo+count-1)->tokindex = j;
				(nameTokenInfo+count-1)->objectindex = i+1;
				count++;
			}
			i++;
	} while(*(objectTokIndex+i+1)!= 0);

	(nameTokenInfo+count-1)->tokindex = 0;
	(nameTokenInfo+count-1)->objectindex = 0;

	return (count-1);
}

void printList(char * jsonstr, jsmntok_t * t, NameTokenInfo * nameTokenInfo) {
	int i = 1, index, price, count;
	char str_price[10], str_count[8];

	printf("*********************************************************\n");
	printf(" 번호      제품명      제조사     가격    개수    총가격\n");
	printf("*********************************************************\n");

	do {
		index = getTokIndex(jsonstr, t, nameTokenInfo, i, "제품명");
		if (index == -1) break;
		printf(" %2d ", i);
		printf("   %13.*s", t[index].end-t[index].start,jsonstr + t[index].start);
		index = getTokIndex(jsonstr, t, nameTokenInfo, i, "제조사");
		printf("     %8.*s", t[index].end-t[index].start,jsonstr + t[index].start);
		index = getTokIndex(jsonstr, t, nameTokenInfo, i, "가격");
		strncpy(str_price, &jsonstr[t[index].start], t[index].end-t[index].start);
		price = atoi(str_price);
		printf("    %5d", price);
		index = getTokIndex(jsonstr, t, nameTokenInfo, i, "개수");
		strncpy(str_count, &jsonstr[t[index].start], t[index].end-t[index].start);
		count = atoi(str_count);
		printf("      %d", count);
		printf("       %d\n", price*count);
		i++;
	} while(1);

}

int getTokIndex(char * jsonstr, jsmntok_t * t, NameTokenInfo * nameTokenInfo, int object, char * name) {
	int i = 0;

	for (i = 0; (nameTokenInfo+i)->tokindex != 0; i++) {
		if ((nameTokenInfo+i)->objectindex == object) {
			if (strcmp(name, "제품명") == 0)
				return (nameTokenInfo+i)->tokindex + 3;
			else if (strcmp(name, "제조사") == 0)
				return (nameTokenInfo+i)->tokindex + 1;
			else if (strcmp(name, "가격") == 0)
				return (nameTokenInfo+i)->tokindex + 5;
			else if (strcmp(name, "개수") == 0)
				return (nameTokenInfo+i)->tokindex + 7;
		}
	}

	return -1;
}

void printObjectList (char * jsonstr, jsmntok_t * t, int tokcount, int * objectTokIndex) {
	int i = 0; // 반복문을 위한 변수 i, Name count를 위한 변수 count 생성 및 1로 초기

	printf("******* Object List *******\n"); // **Object List** 출력
	do {
		printf("[NAME%2d]: %.*s\n", i+1, t[*(objectTokIndex+i)+2].end-t[*(objectTokIndex+i)+2].start,
		 		 			jsonstr + t[*(objectTokIndex+i)+2].start);
		i++;
	}while (*(objectTokIndex+i+1)!= 0);

}

void printNameList(char * jsonstr, jsmntok_t * t, int * nameTokIndex) {
	int i;

	printf("******** Name List ********\n");
	for (i = 0; *(nameTokIndex+i)!=0; i++) {
		printf("[NAME%2d]: %.*s\n", i+1, t[*(nameTokIndex+i)].end-t[*(nameTokIndex+i)].start,
					jsonstr + t[*(nameTokIndex+i)].start);
	}
	printf("\n");
}

void selectNameList(char * jsonstr, jsmntok_t * t, int * nameTokIndex) {
	int n;

	do {
		printf("\nSelect Name's no (exit:0) >> ");
		scanf("%d", &n);
		if (n==0)
			break;
		printf("[Name%2d] %.*s\n", n, t[*(nameTokIndex+n-1)].end-t[*(nameTokIndex+n-1)].start,
					jsonstr + t[*(nameTokIndex+n-1)].start);
		printf("%.*s\n", t[*(nameTokIndex+n-1)+1].end-t[*(nameTokIndex+n-1)+1].start,
					jsonstr + t[*(nameTokIndex+n-1)+1].start);
	} while(1);

}

void selectObjectList(char * jsonstr, jsmntok_t * t, int * objectTokIndex) {
	int n, count = 0;

	do {
		printf("\n원하는 번호 입력 (exit:0) >> ");
		scanf("%d", &n);
		if (n==0)
			break;

		printf("%.*s : %.*s\n", t[*(objectTokIndex+n-1)+1].end-t[*(objectTokIndex+n-1)+1].start,
						jsonstr + t[*(objectTokIndex+n-1)+1].start,
						t[*(objectTokIndex+n-1)+2].end-t[*(objectTokIndex+n-1)+2].start,
						jsonstr + t[*(objectTokIndex+n-1)+2].start);

		for (int i = 3; *(objectTokIndex+n-1)+i < *(objectTokIndex+n) ; i++) {
			if (t[*(objectTokIndex+n-1)+i].type == JSMN_STRING && t[*(objectTokIndex+n-1)+i].size != 0)
				printf("	[%.*s]	%.*s\n", t[*(objectTokIndex+n-1)+i].end-t[*(objectTokIndex+n-1)+i].start,
								jsonstr + t[*(objectTokIndex+n-1)+i].start,
								t[*(objectTokIndex+n-1)+i+1].end-t[*(objectTokIndex+n-1)+i+1].start,
								jsonstr + t[*(objectTokIndex+n-1)+i+1].start);
		}
	}while(1);

}

//void printNameValueOfObject(char * jsonstr, jsmntok_t * t, int * objectTokIndex, int n, )

int main() {

	//int * nameTokIndex = (int *)malloc(sizeof(int)*100);
	//int * objectTokIndex;
	NameTokenInfo * nameTokenInfo = (NameTokenInfo *)malloc(sizeof(NameTokenInfo)*64);
	int count;
	char * str_example = (char *)malloc(sizeof(readJSONFile())+1);
	str_example = readJSONFile();

	if (str_example == NULL)
		return 1;
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
	if (r < 1 || (t[0].type != JSMN_OBJECT && t[0].type != JSMN_ARRAY)) {
		printf("Object or Array expected\n");
		return 1;
	}

	//objectTokIndex = jsonObjectList(str_example, t, r);
	count = jsonNameList(str_example, t, r, nameTokenInfo);
	nameTokenInfo = (NameTokenInfo *)realloc(nameTokenInfo, sizeof(NameTokenInfo)*(count+1));
	printList(str_example, t, nameTokenInfo);
	//printNameList(str_example, t, nameTokIndex);
	//selectNameList(str_example, t, nameTokIndex);
	//printObjectList(str_example, t, r, objectTokIndex); // 전체 객체의 첫번째 데이터 value list를 보여주는 function
	//selectObjectList(str_example, t, objectTokIndex);

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
