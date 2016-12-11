/*
* Copyright (c) 2016, Randolph Han, Zhejiang University of Technology, China
* All rights reserved.
* Project name��Simple File System
* Programmer��Randolph Han
* Finish��2016.12.??
*
*/
#include "head.h"

//ȫ�ֱ���
extern usernote cur_user;				//��ǰ�û�
										//extern char cur_dir[15];     	//��ǰĿ¼ 
extern SqStack cur_dir;
extern Sys_cmd cmd[23];              //�洢23��ϵͳ��ָ��
extern usernote L_user[10];            //�û��������ʾ�Ƚϼ򵥣���Ȼ���еĺô��ǲ�����������ϵ����ƣ������ǵ�ϵͳ�ڿ���ʱӦ��������洢������������ӽӽ���ʵ���
extern int f_inode;				    	//��ǰ��ڵ�λ��

extern super_block hx_superblock;   //������
extern inode file_inode[512];   //inode�ڵ���
extern dir file_dir[512];       //Ŀ¼��
extern physicalBlock phy[20500];         //������


extern  UserOpenTable user_open_table[10];       //�û��򿪱�
extern SystemOpenTable sys_open_table[200];          //ϵͳ�򿪱�
extern ActiveNode active_inode_table;           //�inode��
												//��д��Ϣ�ĺ���
void ReadFromFile(FILE *fp);
void WriteToFile(FILE *fp);
void ReadUsers(FILE *fps);
void SaveUsers(FILE *fps);


//���ļ�����ϵͳ��Ϣ  
void ReadFromFile(FILE *fp)
{
	int i;
	//�ö�����������ʽ��ϵͳ��Ϣ�ļ�
	fp = fopen("filesystem", "rb");
	//�����������д�ŵ���Ϣ,��һ���������СΪ��׼,���ļ����ж���һ��.
	fread(&hx_superblock, sizeof(super_block), 1, fp);
	//���ļ��ж����ܹ�512��inode��Ϣ
	for (i = 0;i<512;i++)
	{
		fread(&file_inode[i], sizeof(inode), 1, fp);
	}
	//���ļ�ϵͳ�ж���512��Ŀ¼����Ϣ
	for (i = 0;i<512;i++)
	{
		fread(&file_dir[i], sizeof(dir), 1, fp);
	}
	//�����ݿ���Ϣ20500
	for (i = 0;i<20500;i++)
	{
		fread(&phy[i], sizeof(physicalBlock), 1, fp);
	}
	//�ر��ļ���
	fclose(fp);
}


// ����Ϣд���ļ���
void WriteToFile(FILE *fp)
{
	//���ļ���,��д�������� �ķ�ʽ
	int i;
	fp = fopen("filesystem", "wb");
	//�Գ�����Ĵ�СΪ��׼,���ļ���д��һ����������
	fwrite(&hx_superblock, sizeof(super_block), 1, fp);
	//�Խڵ���ϢΪ��λ,д��512���ڵ���Ϣ
	for (i = 0;i<512;i++)
	{
		fwrite(&file_inode[i], sizeof(inode), 1, fp);
	}
	//��Ŀ¼��Ĵ�СΪ��׼,���ļ���д��512����������
	for (i = 0;i<512;i++)
	{
		fwrite(&file_dir[i], sizeof(dir), 1, fp);
	}
	//���ļ����ݵ�λΪ��׼,���20500����λ.
	for (i = 0;i<20500;i++)
	{
		fwrite(&phy[i], sizeof(physicalBlock), 1, fp);
	}
	fclose(fp);
}


// ���ļ������û���Ϣ
void ReadUsers(FILE *fps)
{
	//�Զ����������ķ�ʽ���û���Ϣ�ļ�
	int i;
	fps = fopen("users.txt", "rb");
	//һ�û������ݽṹ��СΪ��λ,��ȡ�ļ�ϵͳ���û���Ϣ
	for (i = 0;i<10;i++)
	{
		fread(&L_user[i], sizeof(usernote), 1, fps);
	}
	fclose(fps);
}


// ���û���Ϣд���ļ�
void SaveUsers(FILE *fps)
{
	//����û���Ϣ��ͬ,�˴���д���������ķ�ʽ���ļ�,д��
	int i;
	fps = fopen("users.txt", "wb");
	//ͨ��ѭ��д��������
	for (i = 0;i<8;i++)
	{
		fwrite(&L_user[i], sizeof(usernote), 1, fps);
	}
	fclose(fps);
}
