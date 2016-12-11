/*
* Copyright (c) 2016, Randolph Han, Zhejiang University of Technology, China
* All rights reserved.
* Project name��Simple File System
* Programmer��Randolph Han
* Finish��2016.12.??
*
*/
#include "head.h"

extern FTreepoint L_Ftree;
extern dir file_dir[512];       //Ŀ¼��
extern inode file_inode[512];   //inode�ڵ���
void CreatBiTree1(FTreepoint &T, int& num); //�ݹ鷽�������ļ���
void InitfileTree(FTreepoint &T);    //�ú����������ǳ�ʼ����ʱ�����dir�������ļ���
int find_inode_from_name(char name[10]);     //���������ҵ�inode��
void Tree_to_dir(int &num, dir file_dir[], FTreepoint &T);  //����ת��ΪĿ¼���洢
int find_num_of_file();     //��ǰϵͳ���ж����ļ�

void clear_dir(dir file_dir[]); //��file_dir���



int find_inode_from_name(char name[10])     //���������ҵ�inode��
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

int find_num_of_file()         //��ǰϵͳ���ж����ļ�
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
void InitfileTree(FTreepoint &T)    //�ú����������ǳ�ʼ����ʱ�����dir�������ļ���
{
	int a = 0;
	T = (FTree *)malloc(sizeof(FTree));//���ڵ�û���ڴ洢�����ٿռ䣬Ĭ�ϼ������ڴ��У�������Ϊ��
	strcpy(T->data.file_name, "root");
	T->lchild = T->rchild = NULL;
	if (find_num_of_file()>0)
	{
		CreatBiTree1(T->lchild, a);   //0�����dir���ڼ�����ʼ
	}
}

void CreatBiTree1(FTreepoint &T, int& num) //�ݹ鷽��,�����������˳�򹹽��ļ���
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
	if (strcmp(file_inode[find_inode_from_name(file_dir[num].file_name)].dir_name, T->data.file_name) == 0)   //��ǰ�ļ������ļ�
	{
		CreatBiTree1(T->lchild, num);
	}
	if (strcmp(file_inode[find_inode_from_name(file_dir[num].file_name)].dir_name, file_inode[find_inode_from_name(T->data.file_name)].dir_name) == 0) //�͵�ǰ�ļ��ĸ��ļ�һ��
	{
		CreatBiTree1(T->rchild, num);
	}
}

void Tree_to_dir(int &num, dir file_dir[], FTreepoint &T)   //����ת��ΪĿ¼���洢
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

int path_tnode(SqStack S, FTreepoint T, FTreepoint &p)        //���ݵ�ǰ·���õ���Ӧ�ĸ��ڵ�
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
				if (strcmp((*a).data, t1->data.file_name) == 0)  //����ͬ���ļ�
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


void add_file_tree(FTreepoint &T, SqStack s, char newname[], int newinode)  //sΪ��ǰ·��
{

}

void del_file_tree(FTreepoint &T, SqStack s, char oldname[], int &oldinode) //sΪ��ǰ·��
{

}

void rename_file_tree(FTreepoint &T, SqStack s, char oldname[], char newname[]) //sΪ��ǰ·��
{

}

void clear_dir(dir file_dir[])  //��file_dir���
{
	for (int i = 0;i<512;i++)     // ��Ŀ¼����Ϣ��ʼ��
	{
		strcpy(file_dir[i].file_name, "");   //�ļ���
		file_dir[i].dir_inode = -1;                       //�ļ��ڵ��
	}
}

void cul_num(FTreepoint &T, int &n_file, int &n_dir)  //�����ļ��������ļ��������ļ��и���
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