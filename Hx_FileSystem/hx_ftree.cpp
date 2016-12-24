/*
* Copyright (c) 2016, Randolph Han, Zhejiang University of Technology, China
* All rights reserved.
* Project name：Simple File System
* Programmer：Randolph Han
* Finish：2016.12.18
*
*/
#include "head.h"

extern FTreepoint L_Ftree;
extern dir file_dir[DIR_COUNT];				//dir block
extern inode file_inode[INODES_COUNT];		//inode
void CreatBiTree1(FTreepoint &T, int& num); //create tree
void InitfileTree(FTreepoint &T);			//create tree(from dir)
int find_inode_from_name(char name[USER_NAME_LENGTH]);		//find inode by file name
void Tree_to_dir(int &num, dir file_dir[], FTreepoint &T);  //save tree
int find_num_of_file();			//culate the sum of files

void clear_dir(dir file_dir[]); //clear file_dir

//find inode by file name
int find_inode_from_name(char name[USER_NAME_LENGTH])
{
	for (int i = 0;i < DIR_COUNT;i++)
	{
		if (strcmp(name, file_dir[i].file_name) == 0)
		{
			return file_dir[i].dir_inode;
		}

	}
	return -1;
}

//culate the sum of files
int find_num_of_file()
{
	int num = 0;
	for (int i = 0;i<DIR_COUNT;i++)
	{
		if (file_dir[i].dir_inode >= 0)
		{
			num++;
		}
	}
	return num;
}


void InitfileTree(FTreepoint &T)
{
	int a = 0;
	T = (FTree *)malloc(sizeof(FTree));
	strcpy(T->data.file_name, "root");
	T->lchild = T->rchild = NULL;
	if (find_num_of_file()>0)
	{
		CreatBiTree1(T->lchild, a);
	}
}

void CreatBiTree1(FTreepoint &T, int& num)
{
	if (num >= find_num_of_file())
	{
		return;
	}

	T = (FTree *)malloc(sizeof(FTree));
	T->lchild = T->rchild = NULL;
	T->data.dir_inode = file_dir[num].dir_inode;
	strcpy(T->data.file_name, file_dir[num].file_name);
	num++;
	if (strcmp(file_inode[find_inode_from_name(file_dir[num].file_name)].dir_name, T->data.file_name) == 0)
	{
		CreatBiTree1(T->lchild, num);
	}
	if (strcmp(file_inode[find_inode_from_name(file_dir[num].file_name)].dir_name, file_inode[find_inode_from_name(T->data.file_name)].dir_name) == 0) //和当前文件的父文件一样
	{
		CreatBiTree1(T->rchild, num);
	}
}

//save tree to disk
void Tree_to_dir(int &num, dir file_dir[], FTreepoint &T)
{
	if (T)
	{
		file_dir[num].dir_inode = T->data.dir_inode;
		strcpy(file_dir[num].file_name, T->data.file_name);
		num++;
		Tree_to_dir(num, file_dir, T->lchild);
		Tree_to_dir(num, file_dir, T->rchild);
	}
}

//find root
int path_tnode(SqStack S, FTreepoint T, FTreepoint &p)
{
	FTreepoint t1;
	t1 = T;
	path *a;
	if (S.base == S.top) return 0;
	a = S.base;
	while (a != S.top)
	{
		if (strcmp((*a).data, t1->data.file_name) == 0)
		{
			a++;
			if (a == S.top) break;
			t1 = t1->lchild;

		}
		else
		{
			while (t1 != NULL)
			{
				if (strcmp((*a).data, t1->data.file_name) == 0)
				{
					break;
				}
				t1 = t1->rchild;
			}
		}
	}
	p = t1;
	return 1;
}


void add_file_tree(FTreepoint &T, SqStack s, char newname[], int newinode)
{

}

void del_file_tree(FTreepoint &T, SqStack s, char oldname[], int &oldinode)
{

}

void rename_file_tree(FTreepoint &T, SqStack s, char oldname[], char newname[])
{

}

//clear dir
void clear_dir(dir file_dir[])
{
	for (int i = 0;i<DIR_COUNT;i++)
	{
		strcpy(file_dir[i].file_name, "");
		file_dir[i].dir_inode = -1;
	}
}

//culate file number
void cul_num(FTreepoint &T, int &n_file, int &n_dir)
{
	if (T)
	{
		if (file_inode[T->data.dir_inode].file_style == 0)
		{
			n_dir++;
		}
		if (file_inode[T->data.dir_inode].file_style == 1)
		{
			n_file++;
		}
		cul_num(T->lchild, n_file, n_dir);
		cul_num(T->rchild, n_file, n_dir);
	}
}