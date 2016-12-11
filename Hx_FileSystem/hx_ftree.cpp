/*
* Copyright (c) 2016, Randolph Han, Zhejiang University of Technology, China
* All rights reserved.
* Project name：Simple File System
* Programmer：Randolph Han
* Finish：2016.12.??
*
*/
#include "head.h"

extern FTreepoint L_Ftree;
extern dir file_dir[512];       //目录块
extern inode file_inode[512];   //inode节点区
void CreatBiTree1(FTreepoint &T, int& num); //递归方法构建文件树
void InitfileTree(FTreepoint &T);    //该函数的作用是初始化的时候根据dir区构建文件树
int find_inode_from_name(char name[10]);     //根据名字找到inode号
void Tree_to_dir(int &num, dir file_dir[], FTreepoint &T);  //将树转化为目录区存储
int find_num_of_file();     //求当前系统中有多少文件

void clear_dir(dir file_dir[]); //将file_dir清空



int find_inode_from_name(char name[10])     //根据名字找到inode号
{
	for (int i = 0;i < 512;i++)
	{
		if (strcmp(name, file_dir[i].file_name) == 0)
		{
			return file_dir[i].dir_inode;
		}

	}
	return -1;
}

int find_num_of_file()         //求当前系统中有多少文件
{
	int num = 0;
	for (int i = 0;i<512;i++)
	{
		if (file_dir[i].dir_inode >= 0)
		{
			num++;
		}
	}
	return num;
}
void InitfileTree(FTreepoint &T)    //该函数的作用是初始化的时候根据dir区构建文件树
{
	int a = 0;
	T = (FTree *)malloc(sizeof(FTree));//根节点没有在存储区开辟空间，默认加载在内存中，右子树为空
	strcpy(T->data.file_name, "root");
	T->lchild = T->rchild = NULL;
	if (find_num_of_file()>0)
	{
		CreatBiTree1(T->lchild, a);   //0代表从dir区第几个开始
	}
}

void CreatBiTree1(FTreepoint &T, int& num) //递归方法,按照先序遍历顺序构建文件树
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
	if (strcmp(file_inode[find_inode_from_name(file_dir[num].file_name)].dir_name, T->data.file_name) == 0)   //当前文件的子文件
	{
		CreatBiTree1(T->lchild, num);
	}
	if (strcmp(file_inode[find_inode_from_name(file_dir[num].file_name)].dir_name, file_inode[find_inode_from_name(T->data.file_name)].dir_name) == 0) //和当前文件的父文件一样
	{
		CreatBiTree1(T->rchild, num);
	}
}

void Tree_to_dir(int &num, dir file_dir[], FTreepoint &T)   //将树转化为目录区存储
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

int path_tnode(SqStack S, FTreepoint T, FTreepoint &p)        //根据当前路径得到对应的根节点
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
				if (strcmp((*a).data, t1->data.file_name) == 0)  //存在同名文件
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


void add_file_tree(FTreepoint &T, SqStack s, char newname[], int newinode)  //s为当前路径
{

}

void del_file_tree(FTreepoint &T, SqStack s, char oldname[], int &oldinode) //s为当前路径
{

}

void rename_file_tree(FTreepoint &T, SqStack s, char oldname[], char newname[]) //s为当前路径
{

}

void clear_dir(dir file_dir[])  //将file_dir清空
{
	for (int i = 0;i<512;i++)     // 根目录区信息初始化
	{
		strcpy(file_dir[i].file_name, "");   //文件名
		file_dir[i].dir_inode = -1;                       //文件节点号
	}
}

void cul_num(FTreepoint &T, int &n_file, int &n_dir)  //根据文件树计算文件个数和文件夹个数
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