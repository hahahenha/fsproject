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

#include "settings.h"
#include "expections.h"

using namespace std;

/**
*	Users Information
*/
typedef struct{
	char username[14];	//Username
	char password[14];	//password
	char level;			//level£¬root: 1 & common user: 0
	int userid;			//user's id(start from 0)
	int group;			//group number
} usernote,* userpoint;

/**
*	Commands Information
*/
typedef struct{ 
	char com[10];	
} Sys_cmd,* Sys_cmd_point; 

/**
*	Block Imformation
*/
typedef struct{
	int b_number;	//block number
	int flag;		//whether occupied£¬1 stands for occupied¡¢0 for not
} block;

/**
*	Block Group Information(50 blocks/group)
*/
typedef struct{ 
	int bg_number;      //block group number
	int free_num;		//number of free blocks
	int next;			//the next free block number of the next group
	block free[50];		//block address
} block_group;

/**
*	Super Block Imformation
*/
typedef struct{ 
  	block_group special_stack;  //special stack
  	block_group memory[820];	//information of all blocks,20M = 20500B£¬512Byte/block,820 group
	char phydata[20500];		//allocation of datas
	char inode_info[512];		//allocation of inodes
	char dir_info[512];			//allocation of directories
	int number_inode;           //number of free inode blocks
	int number_dir;             //number of free dir blocks
	int number_data;            //number of free data blocks
} super_block;

/**
*	Inode Information
*/
typedef struct{ 
	int inode_number;		//inode number
	int file_style;		//file type, 0 stand for a directory, 1 stand for a file 
	int file_length;		//the length of the file
	char file_mode[9];		//permission  
	int file_userid;		//user id
	int file_groupid;		//group id
	int file_address[15];	//addressing, 3 level indirect addressing
	int file_icount;		//link, directory doesn't have hard link, but file does
	char dir_name[14];		//parent directory
} inode;

/**
*	Directory/File Information 
*/
typedef struct{ 
	char file_name[14];		//file name
	int  dir_inode;			//inode number(start from 0)
} dir;

/**
*	Data block Information(512B)
*/
typedef struct{
	char p[512];	
} physicalBlock;	

/**
*	System open table Information
*/
typedef struct{
	int  f_inode;		//pointer, point to active inode
	int f_count;       //shared?
} SystemOpenTable;

/**
*	User open table
*/
typedef struct user_opentable{
	int point[25];	//pointer, point to system open table(25 files/user)
} UserOpenTable;

/**
*	Active inode
*/
typedef struct{
	inode activeinode[200];	//inode table
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
	char data[14];
} path;

/**
*	File's path
*/
typedef struct{
	path *base;
	path *top;
	int stacksize;
} SqStack;