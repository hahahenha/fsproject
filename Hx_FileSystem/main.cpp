/*
* Copyright (c) 2016, Randolph Han, Zhejiang University of Technology, China
* All rights reserved.
* Project name£ºSimple File System
* Programmer£ºRandolph Han
* Finish£º2016.12.10
*
*/
#include "head.h"

//Function declare
void Sys_start(FILE *fp);		//start function
void shell(FILE *fp);			//shell

//Global variables
Sys_cmd cmd[25];				//23 commands
usernote L_user[10];            //users array
usernote cur_user;				//current user
SqStack cur_dir;     			//current directory

int f_inode;					//current active inode number

super_block hx_superblock;		//super block
inode  file_inode[512];			//inode
dir file_dir[512];				//directory
physicalBlock phy[20500];       //data


UserOpenTable  user_open_table[10];		//user open table
SystemOpenTable sys_open_table[200];	//system open table
ActiveNode active_inode_table;			//active inode table


char buffer[260];			    //file content buffer
int	address_buffer[1000];       //file address buffer

FTreepoint L_Ftree;				//file tree

void main(){
	FILE *fp;
	fp = fopen("filesystem", "rb");		//read from disk
	Sys_start(fp);						//start system
	shell(fp);							//start shell function
}
