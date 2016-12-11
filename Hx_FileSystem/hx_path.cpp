/*
* Copyright (c) 2016, Randolph Han, Zhejiang University of Technology, China
* All rights reserved.
* Project name£ºSimple File System
* Programmer£ºRandolph Han
* Finish£º2016.12.10
*
*/
#include "head.h"

int InitStack(SqStack &S);		//Init path
int pop(SqStack &S, char e[]);	//pop path
int push(SqStack &S, char e[]);	//push path
int Gettop(SqStack S, char e[]);//get the top path
int find_path(SqStack S);		//get absolute path

int InitStack(SqStack &S){
	S.base = (path*)malloc((100) * sizeof(path));
	if (!S.base)exit(-2);
	S.top = S.base;
	S.stacksize = 100;
	return 1;
}

int pop(SqStack &S, char e[]){
	if (S.base == S.top) return 0;
	S.top--;
	strcpy(e, ((*S.top).data));
	return 1;
}

int push(SqStack &S, char e[]){
	strcpy((*S.top).data, e);
	S.top++;
	return 1;
}

int Gettop(SqStack S, char e[]){
	if (S.base == S.top) return 0;
	strcpy(e, (*(S.top - 1)).data);
	return 1;
}


int find_path(SqStack S){
	path *a;
	if (S.base == S.top) return 0;
	a = S.base;
	while (a != S.top){
		printf("%s/", (*a).data);
		a++;
	}
	return 1;
}