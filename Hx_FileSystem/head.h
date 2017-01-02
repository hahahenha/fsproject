/*
* Copyright (c) 2016, Randolph Han, Zhejiang University of Technology, China
* All rights reserved.
* Project name£ºSimple File System
* Programmer£ºRandolph Han
* Finish£º2016.12.10
*
*/
#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <conio.h> 
#include <time.h>

#include "settings.h"
#include "expections.h"

using namespace std;

/**
*	Users Information
*/
typedef struct{
	char username[USER_NAME_LENGTH];	//Username
	char password[USER_PASSWORD_LENGTH];//password
	char level;				//level£¬root: 1 & common user: 0
	int32_t userid;			//user's id(start from 0)
	int32_t group;			//group number
} usernote,* userpoint;

/**
*	Commands Information
*/
typedef struct{ 
	char com[CMD_LENGTH];
} Sys_cmd,* Sys_cmd_point; 

/**
*	Block Imformation
*/
typedef struct{
	int32_t b_number;	//block number
	int32_t flag;		//whether occupied£¬1 stands for occupied¡¢0 for not
} block;

/**
*	Block Group Information(50 blocks/group)
*/
typedef struct{ 
	int32_t bg_number;      //block group number
	int32_t free_num;		//number of free blocks
	int32_t next;			//the next free block number of the next group
	block free[BLOCK_GROUP_NUM];		//block address
} block_group;

/**
*	Super Block Imformation
*/
typedef struct{ 
  	block_group special_stack;  //special stack
  	block_group memory[BLOCK_GROUP_SIZE];	//information of all blocks
	char phydata[PHY_DATA_SIZE];		//allocation of datas
	char inode_info[INODES_COUNT];		//allocation of inodes
	char dir_info[DIR_COUNT];			//allocation of directories
	int32_t number_inode;           //number of free inode blocks
	int32_t number_dir;             //number of free dir blocks
	int32_t number_data;            //number of free data blocks
} super_block;

/**
*	Inode Information(128bit)
*/
typedef struct{ 
	int32_t inode_number;		//inode number
	int32_t file_style;		//file type, 0 stand for a directory, 1 stand for a file 
	int32_t file_length;		//the length of the file
	char file_mode[PERMISSIONS];		//permission  
	int32_t file_userid;		//user id
	int32_t file_groupid;		//group id
	int32_t file_address[DATA_COUNT];	//addressing, 3 level indirect addressing
	int32_t file_icount;		//link, directory doesn't have hard link, but file does
	char dir_name[DIR_NAME_LENGTH];		//parent directory
	char time[57];
} inode;

/**
*	Directory/File Information 
*/
typedef struct{ 
	char file_name[FILE_NAME_LENGTH];		//file name
	int32_t  dir_inode;			//inode number(start from 0)
} dir;

/**
*	Data block Information
*/
typedef struct{
	char p[DATA_BLOCK_SIZE];
} physicalBlock;	

/**
*	System open table Information
*/
typedef struct{
	int32_t  f_inode;		//pointer, point to active inode
	int32_t f_count;       //shared?
} SystemOpenTable;

/**
*	User open table
*/
typedef struct user_opentable{
	int32_t point[USER_ALLOW_OPEN_COUNT];	//pointer, point to system open table(25 files/user)
} UserOpenTable;

/**
*	Active inode
*/
typedef struct{
	inode activeinode[SYSTEM_ALLOW_OPEN_COUNT];	//inode table
} ActiveNode;

/**
*	Tree node
*/
typedef struct TNode{
	dir data;				//current directory
	struct TNode *lchild,*rchild;	//left child: child file£¬right child: file in current directory
} FTree,* FTreepoint;

/**
*	Path Information
*/
typedef struct{
	char data[PATH_LENGTH];
} path;

/**
*	File's path
*/
typedef struct{
	path *base;
	path *top;
	int32_t stacksize;
} SqStack;