/*
* Copyright (c) 2016, Randolph Han, Zhejiang University of Technology, China
* All rights reserved.
* Project name��Simple File 

* Programmer��Randolph Han
* Finish��2016.12.10
*
*/
#include "head.h"

//Function declare
void Sys_start();		//start function
void shell();			//shell

//Global variables
Sys_cmd cmd[COM_NUM];				//23 commands
usernote L_user[USER_COUNT];            //users array
usernote cur_user;				//current user
SqStack cur_dir;     			//current directory

int32_t f_inode;					//current active inode number

super_block hx_superblock;		//super block
inode  file_inode[INODES_COUNT];			//inode
dir file_dir[DIR_COUNT];				//directory
FILE *ff;

UserOpenTable  user_open_table[USER_ALLOW_OPEN_COUNT];		//user open table
SystemOpenTable sys_open_table[SYSTEM_ALLOW_OPEN_COUNT];	//system open table
ActiveNode active_inode_table;			//active inode table


char buffer[FILE_BUFFER];			    //file content buffer
int32_t	address_buffer[FILE_ADDRESS_BUFFER];//file address buffer

FTreepoint L_Ftree;				//file tree

void main(){
	Sys_start();						//start system
	shell();	
	fclose(ff);							//start shell function
}
