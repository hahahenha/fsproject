/*
* Copyright (c) 2016, Randolph Han, Zhejiang University of Technology, China
* All rights reserved.
* Project name��Simple File System
* Programmer��Randolph Han
* Finish��2016.12.12
*
*/
#include "head.h"

//Global variable
extern usernote cur_user;			//current user
extern SqStack cur_dir;     		//current directory
extern Sys_cmd cmd[25];				//23 commands
extern usernote L_user[10];			//users array
extern int f_inode;					//current active inode number

extern super_block hx_superblock;   //super block
extern inode file_inode[512];		//inode
extern dir file_dir[512];			//directory
extern physicalBlock phy[20500];	//data

extern  UserOpenTable user_open_table[10];	//user open table
extern SystemOpenTable sys_open_table[200];	//system open table
extern ActiveNode active_inode_table;		//active inode table

//���������
int cul_num_usetalbe(int i);  //i����userid,���û��򿪶����ļ�
int cul_num_systable();    //ϵͳ�򿪱�ĸ���
int cul_num_inodetable();    //�inode�򿪱�ĸ���
void InsertUserTable(int a, int i); //���û�i������inode��Ϊa���ļ��ڵ�
void DelUserTable(int a, int i);  //�û���ɾ��


//ϵͳ���к���
void InsertTable(char filename[], int i_num);
int SelectTable(char filename[]);
int IsPermit(int i_num, char operation);
int AllotDisks(int length);
void RecoverDisks(int length);

int cul_num_inodetable();   //�inode�򿪱�ĸ���
int cul_num_usetalbe(int i); //i����userid,���û��򿪶����ļ�
int cul_num_systable();   //ϵͳ�򿪱�ĸ���

int InitStack(SqStack &S);   //��ʼ��·��
int pop(SqStack &S, char e[]);
int push(SqStack &S, char e[]);
int Gettop(SqStack S, char e[]);
int find_path(SqStack S);   //��ȡ��·��

							//�жϵ�ǰinode�����һ������λ��
int find_free_inode(){
	for (int i = 0;i<512;i++){
		if (file_inode[i].inode_number == -1){
			return i;
		}
	}
	printf("inode has already run out!");return -1;
}

int cul_num_usetalbe(int i) {  //the number of user open table, i:user id
	int num = 0;
	for (int j = 0;j < 25;j++) {
		if (user_open_table[i].point[j] > -1) {
			num++;
		}
	}
	return num;
}

int cul_num_systable(){    //number of system open table
	int num = 0;
	for (int j = 0;j<200;j++){
		if (sys_open_table[j].f_inode>-1){
			num++;
		}
	}
	return num;
}


int cul_num_inodetable(){    //active inode number
	int num = 0;
	for (int j = 0;j<200;j++){
		if (active_inode_table.activeinode[j].inode_number>-1){
			num++;
		}
	}
	return num;
}

void insertInodeTable(int a){ //insert to active inode table��a:inode
	for (int i = 0;i<200;i++){
		if (active_inode_table.activeinode[i].inode_number == -1){
			active_inode_table.activeinode[i].inode_number = a;
			strcpy(active_inode_table.activeinode[i].dir_name, file_inode[a].dir_name);
			active_inode_table.activeinode[i].file_groupid = file_inode[a].file_groupid;
			active_inode_table.activeinode[i].file_icount = file_inode[a].file_icount;
			active_inode_table.activeinode[i].file_length = file_inode[a].file_length;
			active_inode_table.activeinode[i].file_style = file_inode[a].file_style;
			active_inode_table.activeinode[i].file_userid = file_inode[a].file_userid;
			int j;
			for (j = 0;j<9;j++){
				active_inode_table.activeinode[i].file_mode[j] = file_inode[a].file_mode[j];
			}
			for (j = 0;j<15;j++){
				active_inode_table.activeinode[i].file_address[j] = file_inode[a].file_address[j];
			}
			return;
		}
	}
	printf("Acitve inode table is full!\n");
}

int insertSysTable(int a){   //��ϵͳ�������ӣ�����ֵΪ���ӵ��±�(�����ҵ�һ��Ϊ�յ�λ��)��aΪ���ӵ�inode��
	int i;
	for (i = 0;i<200;i++){
		if (sys_open_table[i].f_inode == a){  //������conut++;
			sys_open_table[i].f_count++;
			return i;
		}
	}
	for (i = 0;i<200;i++){
		if (sys_open_table[i].f_inode == -1){  //���������½�,ͬʱ�ڻinode���в����µ�
			sys_open_table[i].f_count = 1;
			sys_open_table[i].f_inode = a;
			insertInodeTable(a);
			return i;
		}
	}
	printf("The system open table is full!\n");
	return -1;
}

void InsertUserTable(int a, int i){ //���û�i������inode��Ϊa���ļ��ڵ�
	for (int j = 0;j<25;j++){
		if (user_open_table[i].point[j] == -1){
			user_open_table[i].point[j] = insertSysTable(a);
			return;
		}
	}
	printf("You can't open more files at the same time!\n");
}

void DelInodeTable(int a){    //�inode��ɾ��
	for (int i = 0;i<200;i++){
		if (active_inode_table.activeinode[i].inode_number == a){
			active_inode_table.activeinode[i].inode_number = -1;
			return;
		}
	}
	printf("The file isn't in active inode table!\n");
}

void DelSysTable(int a){      //ϵͳ��ɾ��
	for (int i = 0;i<200;i++){
		if (sys_open_table[i].f_inode == a){  //������f_count--
			sys_open_table[i].f_count--;
			if (sys_open_table[i].f_count == 0){
				DelInodeTable(a);
				sys_open_table[i].f_inode = -1;
				return;
			}
			return;
		}
	}
	printf("The file isn't in system open file!\n");
}

void DelUserTable(int a, int i){  //�û���ɾ��
	for (int j = 0;j<25;j++){
		if ((user_open_table[i].point[j]>-1) && (sys_open_table[user_open_table[i].point[j]].f_inode == a)){
			user_open_table[i].point[j] = -1;
			DelSysTable(a);
			return;
		}
	}
	printf("The file isn't in user open table!\n");
}

void str2stack(SqStack &s){        //�����ļ�·����ת���ɶ�Ӧ��ջ·���洢
	fflush(stdin);
	InitStack(s);
	printf("���������ӵ��ļ�·��,��#��������:rooot/1/2/#\n");
	char ch, c1[10];
	int num = 0;
	while ((ch = getchar()) != '#'){
		if (ch != '/'){
			c1[num] = ch;
			num++;
		}
		else{
			c1[num] = 0;
			push(s, c1);
			num = 0;
		}
	}
}

int findtreeinode(SqStack S, FTreepoint T, FTreepoint &p){     //����·���ҵ���Ӧ�ڵ�
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
				if (strcmp((*a).data, t1->data.file_name) == 0){  //����ͬ���ļ�
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