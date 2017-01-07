/*
* Copyright (c) 2016, Randolph Han, Zhejiang University of Technology, China
* All rights reserved.
* Project name��Simple File System
* Programmer��Randolph Han
* Finish��2016.12.10��2016.12.25
*
*/
#include "head.h"

//Global variable
extern usernote cur_user;			//current user
extern SqStack cur_dir;     		//current directory
extern Sys_cmd cmd[COM_NUM];				//23 commands
extern usernote L_user[USER_COUNT];			//users array
extern int32_t f_inode;					//current active inode number
extern FILE *ff;					//disk
extern super_block hx_superblock;   //super block
extern inode file_inode[INODES_COUNT];		//inode
extern dir file_dir[DIR_COUNT];			//directory


extern  UserOpenTable user_open_table[USER_ALLOW_OPEN_COUNT];	//user open table
extern SystemOpenTable sys_open_table[SYSTEM_ALLOW_OPEN_COUNT];	//system open table
extern ActiveNode active_inode_table;		//active inode table

extern FTreepoint L_Ftree;                 //file tree

//File operating
void ReadFromFile();	//read file
void WriteToFile();		//write file
void ReadUsers(FILE *fp);		//get users
void SaveUsers();		//save users
extern FILE *ff;					//disk
//Init function
void InitSystem();		//Init system
void InitDisks();				//Init disk
void InitUsers();				//Init users
void InitTable();				//Init table
void InitCommand();				//Init command
void shell();			//Init shell

//cmd
int32_t login();					//login function
void help();					//help function
void show_curdir();				//show current directory
void go_dir(char tmp[]);		//jump to a directory
void back_dir();				//back to last directory
void create_dir(char tmp[]);	//create a directory
void del_dir(char tmp[]);		//delete a directory
void create_file(char tmp[]);	//create a file
void open_file(char tmp[]);		//open a file
void read_file(char tmp[]);		//read a file
void write_file(char tmp[]);	//write a file
void close_file(char tmp[]);	//close a file
void delete_file(char tmp[]);	//delete a file
void copy_file(char tmp[]);		//copy a file
void show_info();				//show system information
void logout();			//logout
int32_t change_user(char tmp[]);	//change user
void change_mode(char tmp[]);	//chang file mode
void change_owner(char filename[]);//change file owner
void change_group(char filename[]);//change file group
void manage_user();				//user management
void h_link(char tmp[]);		//hard link
void s_link(char tmp[]);		//soft link
void rename(char tmp[]);		//rename a file
void change_pwd();				//change password

//path
int32_t InitStack(SqStack &S);		//Init path
int32_t pop(SqStack &S, char e[]);	//pop path
int32_t push(SqStack &S, char e[]);	//push path
int32_t Gettop(SqStack S, char e[]);//get the top path
int32_t find_path(SqStack S);		//get absolute path

//file tree
void InitfileTree(FTreepoint &T);    //Init file tree


//system start
void Sys_start(){
	InitSystem();     //Init system

	printf("\t\t\t\t**************************************************************\n");
	printf("\t\t\t\t*                                                            *\n");
	printf("\t\t\t\t*  **           **                                           *\n");
	printf("\t\t\t\t*  **           **                                           *\n");
	printf("\t\t\t\t*  **           **                                           *\n");
	printf("\t\t\t\t*  **           **                                           *\n");
	printf("\t\t\t\t*  **           **    ***         *    **    **       **     *\n");
	printf("\t\t\t\t*  **           **    *  *        *           **     **      *\n");
	printf("\t\t\t\t*  **           **    *   *       *    **      **   **       *\n");
	printf("\t\t\t\t*  **           **    *    *      *    **       ** **        *\n");
	printf("\t\t\t\t*  **           **    *     *     *    **         **         *\n");
	printf("\t\t\t\t*  **           **    *      *    *    **       **  **       *\n");
	printf("\t\t\t\t*  **           **    *       *   *    **      **    **      *\n");
	printf("\t\t\t\t*  **           **    *        *  *    **     **      **     *\n");
	printf("\t\t\t\t*  ***************    *         ***    **    **        **    *\n");
	printf("\t\t\t\t*                                                            *\n");
	printf("\t\t\t\t**************************************************************\n");
}


//Init system, if file does not existed, create a new file
void InitSystem(){
	//read file
	FILE *fp = fopen(DISK_NAME, "rb");
	if (fp == NULL) {
		InitDisks();		//Init the disk
	}
	else {
		ReadFromFile();   //read disk
	}
	InitTable();			//Init file table
	InitCommand();
	InitStack(cur_dir);      //Init stack path
	push(cur_dir, "root");
	InitfileTree(L_Ftree);
	InitUsers();			//Init users
	WriteToFile();
}

//Init command
void InitCommand(){
	strcpy(cmd[0].com, "exit");
	strcpy(cmd[1].com, "help");
	strcpy(cmd[2].com, "ls");
	strcpy(cmd[3].com, "cd");
	strcpy(cmd[4].com, "cd..");
	strcpy(cmd[5].com, "mkdir");
	strcpy(cmd[6].com, "rmdir");
	strcpy(cmd[7].com, "create");
	strcpy(cmd[8].com, "open");
	strcpy(cmd[9].com, "cat");
	strcpy(cmd[10].com, "write");
	strcpy(cmd[11].com, "close");
	strcpy(cmd[12].com, "rm");
	strcpy(cmd[13].com, "info");
	strcpy(cmd[14].com, "logout");
	strcpy(cmd[15].com, "su");
	strcpy(cmd[16].com, "chmod");
	strcpy(cmd[17].com, "Muser");
	strcpy(cmd[18].com, "hlink");
	strcpy(cmd[19].com, "slink");
	strcpy(cmd[20].com, "mv");
	strcpy(cmd[21].com, "pwd");
	strcpy(cmd[22].com, "chown");
	strcpy(cmd[23].com, "chgrp");
	strcpy(cmd[24].com, "passwd");
	strcpy(cmd[25].com, "cp");
}

//Shell
void shell(){
	int32_t i, p;				//p: command number
	char com[CMD_LENGTH], tmp[2*CMD_LENGTH];  //com: current command, tmp: parameter
	while (1){
		if (!login()) return;
		while (1){
			printf("[%s]@/", cur_user.username);
			find_path(cur_dir);
			printf(">");
			scanf("%s", com);       //input a command 
			if (strcmp(com, "cd.") == 0) continue;
			for (i = 0;i<COM_NUM;i++){
				if (strcmp(com, cmd[i].com) == 0){
					p = i;
					break;
				}
			}
			if (i == COM_NUM){			//Not find from the list
				p = COM_NUM;
			}
			switch (p) {
			case 0: WriteToFile();fclose(ff);return;     //shut down
			case 1: help();break;				//help
			case 2: show_curdir();break;		//show current directory content
			case 3: scanf("%s", tmp);go_dir(tmp);break;//jump to the directory given
			case 4: back_dir();break;			//go back to last directory
			case 5: scanf("%s", tmp);create_dir(tmp);break;		//create a directory
			case 6: scanf("%s", tmp);del_dir(tmp);break;		//remove a directory
			case 7: scanf("%s", tmp);create_file(tmp);break;	//create a file
			case 8: scanf("%s", tmp);open_file(tmp);break;		//open a file
			case 9: scanf("%s", tmp);read_file(tmp);break;		//read a file
			case 10: scanf("%s", tmp);write_file(tmp);break;	//write a file
			case 11: scanf("%s", tmp);close_file(tmp);break;	//close the file
			case 12: scanf("%s", tmp);delete_file(tmp);break;	//remove a file
			case 13: show_info();break;			//show system information
			case 14: logout();break;			//logout
			case 15: scanf("%s", tmp);			//change user
				if (change_user(tmp))
					break;
				else
					return;
			case 16: scanf("%s", tmp); change_mode(tmp);break;	//change file mode
			case 17: manage_user();break;						//user magagement
			case 18: scanf("%s", tmp);h_link(tmp);break;		//hard link
			case 19: scanf("%s", tmp);s_link(tmp);break;		//soft link
			case 20: scanf("%s", tmp);rename(tmp);break;		//rename
			case 21: find_path(cur_dir);printf("\n");break;		//show path
			case 22: scanf("%s", tmp);change_owner(tmp);break;	//change file modebreak;
			case 23: scanf("%s", tmp);change_group(tmp);break;	//change file modebreak;
			case 24: change_pwd();break;						//change password
			case 25: scanf("%s", tmp);copy_file(tmp);break;		//copy a file
			default: printf(E21);break; 						//error			 
			}
		}

	}
}

//Init disk
void InitDisks(){
	int32_t i, j, num = 1;
	hx_superblock.number_data = PHY_DATA_SIZE;
	hx_superblock.number_dir = DIR_COUNT;
	hx_superblock.number_inode = INODES_COUNT;
	for (i = 0;i<INODES_COUNT;i++){
		hx_superblock.inode_info[i] = 0;
		hx_superblock.dir_info[i] = 0;
	}
	//Init super block
	hx_superblock.special_stack.bg_number=1;
	hx_superblock.special_stack.free_num= BLOCK_GROUP_NUM;
	hx_superblock.special_stack.next=2;
	for (i = 0;i < FREE_INODE_STACK_SIZE;i++) {
		hx_superblock.special_stack.free[i].flag = 0;
		hx_superblock.special_stack.free[i].b_number = num;
		num++;
	}
	num=0;
	for (i = 0;i < BLOCK_GROUP_SIZE;i++) {
		hx_superblock.memory[i].bg_number = i + 1;
		hx_superblock.memory[i].free_num = BLOCK_GROUP_NUM;
		hx_superblock.memory[i].next = i + 2;
		if (i == INODES_COUNT - 1)
			hx_superblock.memory[i].next = 0;
		for (j = 0;j < BLOCK_GROUP_NUM;j++) {
			hx_superblock.memory[i].free[j].flag = 0;
			hx_superblock.memory[i].free[j].b_number = num;
			num++;
		}
	}

	//Init inode
	for (i = 0;i<INODES_COUNT;i++){
		for (j = 0;j<PERMISSIONS;j++)
			file_inode[i].file_mode[j] = -1;	//read or write permission
		for (j = 0;j<DATA_COUNT;j++)
			file_inode[i].file_address[j] = -1;	//file addressing
		for (j = 0;j<FILE_NAME_LENGTH;j++)
			file_inode[i].dir_name[j] = -1;		//directory
		file_inode[i].inode_number = -1;		//inode number
		file_inode[i].file_length = -1;			//file length
		file_inode[i].file_style = -1;			//file type
		file_inode[i].file_icount = -1;			//open file number
		file_inode[i].file_userid = -1;			//user id
		file_inode[i].file_groupid = -1;		//group id
	}
	//Init root directory
	for (i = 0;i<DIR_COUNT;i++){
		strcpy(file_dir[i].file_name, "");   //file name
		file_dir[i].dir_inode = -1;          //file inode
	}
}

//Init users
void InitUsers()
{
	FILE *fps;
	if ((fps = fopen(DISK_NAME, "rb")) == NULL){
		L_user[0].userid = 0;
		strcpy(L_user[0].username, "admin");
		strcpy(L_user[0].password, "admin");
		L_user[0].group = 1;
		L_user[0].level = 1;
		for (int32_t i = 1;i<USER_COUNT;i++)
		{
			L_user[i].userid = -1;
			strcpy(L_user[i].username, "");
			strcpy(L_user[i].password, "");
			L_user[i].group = -1;
			L_user[i].level = -1;
		}
		ff = fopen(DISK_NAME, "wb+");		//read from disk
		create_file("pw");
		SaveUsers();
	}
	else
	{
		//����ϵͳ�û�
		ReadUsers(fps);
		fclose(fps);
		ff = fopen(DISK_NAME, "rb+");
	}

}

//Init others
void InitTable(){
	int32_t i, j;
	
	//Init user open table
	for (i = 0;i<USER_COUNT;i++)
		for (j = 0;j<USER_ALLOW_OPEN_COUNT;j++)
			user_open_table[i].point[j] = -1;
	//Init system open table
	for (i = 0; i<SYSTEM_ALLOW_OPEN_COUNT; i++){
		sys_open_table[i].f_inode = -1;
		sys_open_table[i].f_count = -1;
	}
	//Init active inode
	for (i = 0;i<SYSTEM_ALLOW_OPEN_COUNT;i++){
		for (j = 0;j<PERMISSIONS;j++)
			active_inode_table.activeinode[i].file_mode[j] = -1;	//permission
		for (j = 0;j<DATA_COUNT;j++)
			active_inode_table.activeinode[i].file_address[j] = -1;	//file address 
		for (j = 0;j<DIR_NAME_LENGTH;j++)
			active_inode_table.activeinode[i].dir_name[j] = -1;		//directory
		active_inode_table.activeinode[i].inode_number = -1;		//inode number
		active_inode_table.activeinode[i].file_length = -1;			//file length
		active_inode_table.activeinode[i].file_style = -1;			//file type
		active_inode_table.activeinode[i].file_icount = -1;			//open file number
		active_inode_table.activeinode[i].file_userid = -1;			//user id
		active_inode_table.activeinode[i].file_groupid = -1;		//group id
	}
}

