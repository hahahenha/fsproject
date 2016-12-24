/*
* Copyright (c) 2016, Randolph Han, Zhejiang University of Technology, China
* All rights reserved.
* Project name£∫Simple File System
* Programmer£∫Randolph Han
* Finish£∫2016.12.24
*
*/
#include "head.h"

//Global variable
extern usernote cur_user;			//current user
extern SqStack cur_dir;     		//current directory
extern Sys_cmd cmd[COM_NUM];				//23 commands
extern usernote L_user[USER_COUNT];			//users array
extern int f_inode;					//current active inode number

extern super_block hx_superblock;   //super block
extern inode file_inode[INODES_COUNT];		//inode
extern dir file_dir[DIR_COUNT];			//directory
extern physicalBlock phy[PHY_DATA_SIZE];	//data

extern  UserOpenTable user_open_table[USER_ALLOW_OPEN_COUNT];	//user open table
extern SystemOpenTable sys_open_table[SYSTEM_ALLOW_OPEN_COUNT];	//system open table
extern ActiveNode active_inode_table;		//active inode table

//table operatorate
int cul_num_usetalbe(int i);	//culate the number of files opened, i:userid
int cul_num_systable();			//culate the number of files system opened
int cul_num_inodetable();		//culate the number of active inode
void InsertUserTable(int a, int i); //insert the a number inode to user open table
void DelUserTable(int a, int i);  //”√delete inode from user open table


//system function ??
void InsertTable(char filename[], int i_num);
int SelectTable(char filename[]);
int IsPermit(int i_num, char operation);
int AllotDisks(int length);
void RecoverDisks(int length);

int InitStack(SqStack &S);   //stack init
int pop(SqStack &S, char e[]);
int push(SqStack &S, char e[]);
int Gettop(SqStack S, char e[]);
int find_path(SqStack S);   //get the path


int find_free_inode(){
	for (int i = 0;i<INODES_COUNT;i++){
		if (file_inode[i].inode_number == -1){
			return i;
		}
	}
	printf(E3);return -1;
}

//the number of user open table, i:user id
int cul_num_usetalbe(int i) {  
	int num = 0;
	for (int j = 0;j < USER_ALLOW_OPEN_COUNT;j++) {
		if (user_open_table[i].point[j] > -1) {
			num++;
		}
	}
	return num;
}

//number of system open table
int cul_num_systable() {    
	int num = 0;
	for (int j = 0;j < SYSTEM_ALLOW_OPEN_COUNT;j++) {
		if (sys_open_table[j].f_inode > -1) {
			num++;
		}
	}
	return num;
}

//active inode number
int cul_num_inodetable(){    
	int num = 0;
	for (int j = 0;j<SYSTEM_ALLOW_OPEN_COUNT;j++){
		if (active_inode_table.activeinode[j].inode_number>-1){
			num++;
		}
	}
	return num;
}

//insert to active inode table£¨a:inode
void insertInodeTable(int a){ 
	for (int i = 0;i<SYSTEM_ALLOW_OPEN_COUNT;i++){
		if (active_inode_table.activeinode[i].inode_number == -1){
			active_inode_table.activeinode[i].inode_number = a;
			strcpy(active_inode_table.activeinode[i].dir_name, file_inode[a].dir_name);
			active_inode_table.activeinode[i].file_groupid = file_inode[a].file_groupid;
			active_inode_table.activeinode[i].file_icount = file_inode[a].file_icount;
			active_inode_table.activeinode[i].file_length = file_inode[a].file_length;
			active_inode_table.activeinode[i].file_style = file_inode[a].file_style;
			active_inode_table.activeinode[i].file_userid = file_inode[a].file_userid;
			int j;
			for (j = 0;j<PERMISSIONS;j++){
				active_inode_table.activeinode[i].file_mode[j] = file_inode[a].file_mode[j];
			}
			for (j = 0;j<DATA_COUNT;j++){
				active_inode_table.activeinode[i].file_address[j] = file_inode[a].file_address[j];
			}
			return;
		}
	}
	printf(E4);
}

//insert inode to system open table, inode number: a
int insertSysTable(int a) {   
	int i;
	for (i = 0;i < SYSTEM_ALLOW_OPEN_COUNT;i++) {
		if (sys_open_table[i].f_inode == a) {  //if exists, conut++;
			sys_open_table[i].f_count++;
			return i;
		}
	}
	for (i = 0;i < SYSTEM_ALLOW_OPEN_COUNT;i++) {
		if (sys_open_table[i].f_inode == -1) {  //else create
			sys_open_table[i].f_count = 1;
			sys_open_table[i].f_inode = a;
			insertInodeTable(a);
			return i;
		}
	}
	printf(E1);
	return -1;
}

//insert inode to user open table
void InsertUserTable(int a, int i){
	for (int j = 0;j<USER_ALLOW_OPEN_COUNT;j++){
		if (user_open_table[i].point[j] == -1){
			user_open_table[i].point[j] = insertSysTable(a);
			return;
		}
	}
	printf(E2);
}

//delete
void DelInodeTable(int a){
	for (int i = 0;i<SYSTEM_ALLOW_OPEN_COUNT;i++){
		if (active_inode_table.activeinode[i].inode_number == a){
			active_inode_table.activeinode[i].inode_number = -1;
			return;
		}
	}
	printf(E5);
}

//delete from system open table
void DelSysTable(int a) {
	for (int i = 0;i < SYSTEM_ALLOW_OPEN_COUNT;i++) {
		if (sys_open_table[i].f_inode == a) { 
			sys_open_table[i].f_count--;
			if (sys_open_table[i].f_count == 0) {
				DelInodeTable(a);
				sys_open_table[i].f_inode = -1;
				return;
			}
			return;
		}
	}
	printf(E6);
}

//delete from user open table
void DelUserTable(int a, int i) { 
	for (int j = 0;j < USER_ALLOW_OPEN_COUNT;j++) {
		if ((user_open_table[i].point[j] > -1) && (sys_open_table[user_open_table[i].point[j]].f_inode == a)) {
			user_open_table[i].point[j] = -1;
			DelSysTable(a);
			return;
		}
	}
	printf(E7);
}

//path to stack
void str2stack(SqStack &s) {
	fflush(stdin);
	InitStack(s);
	printf("Please input the absolute path, end with '#'(ex:root/1/2/#\n)");
	char ch, c1[STACK_SIZE];
	int num = 0;
	while ((ch = getchar()) != '#') {
		if (ch != '/') {
			c1[num] = ch;
			num++;
		}
		else {
			c1[num] = 0;
			push(s, c1);
			num = 0;
		}
	}
}

//find the inode
int findtreeinode(SqStack S, FTreepoint T, FTreepoint &p){
	FTreepoint t1;
	t1 = T;
	path *a;
	if (S.base == S.top) return 0;
	a = S.base;
	while (a != S.top){
		if (strcmp((*a).data, t1->data.file_name) == 0){
			a++;
			if (a == S.top) break;
			t1 = t1->lchild;

		}
		else{
			while (t1 != NULL){
				if (strcmp((*a).data, t1->data.file_name) == 0){
					break;
				}
				t1 = t1->rchild;
			}
		}
		if (t1 == NULL){
			break;
		}
	}
	p = t1;
	return 1;
}