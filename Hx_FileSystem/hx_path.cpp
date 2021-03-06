/*
* Copyright (c) 2016, Randolph Han, Zhejiang University of Technology, China
* All rights reserved.
* Project name��Simple File System
* Programmer��Randolph Han
* Finish��2016.12.10
*
*/
#include "head.h"

int32_t InitStack(SqStack &S);		//Init path
int32_t pop(SqStack &S, char e[]);	//pop path
int32_t push(SqStack &S, char e[]);	//push path
int32_t Gettop(SqStack S, char e[]);//get the top path
int32_t find_path(SqStack S);		//get absolute path

int32_t InitStack(SqStack &S){
	S.base = (path*)malloc((STACK_SIZE) * sizeof(path));
	if (!S.base)exit(-2);
	S.top = S.base;
	S.stacksize = STACK_SIZE;
	return 1;
}

int32_t pop(SqStack &S, char e[]){
	if (S.base == S.top) return 0;
	S.top--;
	strcpy(e, ((*S.top).data));
	return 1;
}

int32_t push(SqStack &S, char e[]){
	strcpy((*S.top).data, e);
	S.top++;
	return 1;
}

int32_t Gettop(SqStack S, char e[]){
	if (S.base == S.top) return 0;
	strcpy(e, (*(S.top - 1)).data);
	return 1;
}


int32_t find_path(SqStack S){
	path *a;
	if (S.base == S.top) return 0;
	a = S.base;
	while (a != S.top){
		printf("%s/", (*a).data);
		a++;
	}
	return 1;
}