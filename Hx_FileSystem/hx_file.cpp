/*
* Copyright (c) 2016, Randolph Han, Zhejiang University of Technology, China
* All rights reserved.
* Project name：Simple File System
* Programmer：Randolph Han
* Finish：2016.12.??
*
*/
#include "head.h"

//全局变量
extern usernote cur_user;				//当前用户
										//extern char cur_dir[15];     	//当前目录 
extern SqStack cur_dir;
extern Sys_cmd cmd[23];              //存储23条系统的指令
extern usernote L_user[10];            //用户用数组表示比较简单，虽然队列的好处是不会存在数量上的限制，但考虑到系统在控制时应该是整块存储所以用数组更加接近真实情况
extern int f_inode;				    	//当前活动节点位置

extern super_block hx_superblock;   //超级块
extern inode file_inode[512];   //inode节点区
extern dir file_dir[512];       //目录块
extern physicalBlock phy[20500];         //数据区


extern  UserOpenTable user_open_table[10];       //用户打开表
extern SystemOpenTable sys_open_table[200];          //系统打开表
extern ActiveNode active_inode_table;           //活动inode表
												//读写信息的函数
void ReadFromFile(FILE *fp);
void WriteToFile(FILE *fp);
void ReadUsers(FILE *fps);
void SaveUsers(FILE *fps);


//从文件读出系统信息  
void ReadFromFile(FILE *fp)
{
	int i;
	//用二进制流的形式打开系统信息文件
	fp = fopen("filesystem", "rb");
	//读出超级块中存放的信息,以一个超级块大小为标准,从文件流中读出一个.
	fread(&hx_superblock, sizeof(super_block), 1, fp);
	//从文件中读出总共512个inode信息
	for (i = 0;i<512;i++)
	{
		fread(&file_inode[i], sizeof(inode), 1, fp);
	}
	//从文件系统中读出512个目录项信息
	for (i = 0;i<512;i++)
	{
		fread(&file_dir[i], sizeof(dir), 1, fp);
	}
	//读数据块信息20500
	for (i = 0;i<20500;i++)
	{
		fread(&phy[i], sizeof(physicalBlock), 1, fp);
	}
	//关闭文件流
	fclose(fp);
}


// 将信息写入文件中
void WriteToFile(FILE *fp)
{
	//打开文件流,以写二进制流 的方式
	int i;
	fp = fopen("filesystem", "wb");
	//以超级块的大小为标准,向文件中写入一个二进制流
	fwrite(&hx_superblock, sizeof(super_block), 1, fp);
	//以节点信息为单位,写入512个节点信息
	for (i = 0;i<512;i++)
	{
		fwrite(&file_inode[i], sizeof(inode), 1, fp);
	}
	//以目录项的大小为标准,向文件中写入512个二进制流
	for (i = 0;i<512;i++)
	{
		fwrite(&file_dir[i], sizeof(dir), 1, fp);
	}
	//以文件数据单位为标准,存放20500个单位.
	for (i = 0;i<20500;i++)
	{
		fwrite(&phy[i], sizeof(physicalBlock), 1, fp);
	}
	fclose(fp);
}


// 从文件读出用户信息
void ReadUsers(FILE *fps)
{
	//以读二进制流的方式打开用户信息文件
	int i;
	fps = fopen("users.txt", "rb");
	//一用户的数据结构大小为单位,读取文件系统的用户信息
	for (i = 0;i<10;i++)
	{
		fread(&L_user[i], sizeof(usernote), 1, fps);
	}
	fclose(fps);
}


// 将用户信息写入文件
void SaveUsers(FILE *fps)
{
	//与读用户信息相同,此处以写二进制流的方式打开文件,写入
	int i;
	fps = fopen("users.txt", "wb");
	//通过循环写二进制流
	for (i = 0;i<8;i++)
	{
		fwrite(&L_user[i], sizeof(usernote), 1, fps);
	}
	fclose(fps);
}
