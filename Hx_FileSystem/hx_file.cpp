/*
* Copyright (c) 2016, Randolph Han, Zhejiang University of Technology, China
* All rights reserved.
* Project name£ºSimple File System
* Programmer£ºRandolph Han
* Finish£º2016.12.12
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

//Read information
void ReadFromFile(FILE *fp);
void WriteToFile(FILE *fp);
void ReadUsers(FILE *fps);
void SaveUsers(FILE *fps);

//Read from file  
void ReadFromFile(FILE *fp){
	int i;
	//open file with binary stream
	fp = fopen("filesystem", "rb");
	//read super block
	fread(&hx_superblock, sizeof(super_block), 1, fp);
	//read inode
	for (i = 0;i<512;i++){
		fread(&file_inode[i], sizeof(inode), 1, fp);
	}
	//read directories
	for (i = 0;i<512;i++)
	{
		fread(&file_dir[i], sizeof(dir), 1, fp);
	}
	//read data
	for (i = 0;i<20500;i++)
	{
		fread(&phy[i], sizeof(physicalBlock), 1, fp);
	}
	//close file
	fclose(fp);
}


//write to file
void WriteToFile(FILE *fp)
{
	//open file with write binary stream mode
	int i;
	fp = fopen("filesystem", "wb");
	//write super block
	fwrite(&hx_superblock, sizeof(super_block), 1, fp);
	//write inode
	for (i = 0;i<512;i++){
		fwrite(&file_inode[i], sizeof(inode), 1, fp);
	}
	//write directories
	for (i = 0;i<512;i++)
	{
		fwrite(&file_dir[i], sizeof(dir), 1, fp);
	}
	//write data
	for (i = 0;i<20500;i++)
	{
		fwrite(&phy[i], sizeof(physicalBlock), 1, fp);
	}
	fclose(fp);
}


//read users information
void ReadUsers(FILE *fps){
	//open file with binary stream
	int i;
	fps = fopen("users.txt", "rb");
	for (i = 0;i<10;i++){
		fread(&L_user[i], sizeof(usernote), 1, fps);
	}
	fclose(fps);
}


//write users information
void SaveUsers(FILE *fps){
	//open file with write binary stream mode
	int i;
	fps = fopen("users.txt", "wb");
	for (i = 0;i<8;i++){
		fwrite(&L_user[i], sizeof(usernote), 1, fps);
	}
	fclose(fps);
}
