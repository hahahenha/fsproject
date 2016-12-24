/*
* Copyright (c) 2016, Randolph Han, Zhejiang University of Technology, China
* All rights reserved.
* Project name：Simple File System
* Programmer：Randolph Han
* Finish：2016.12.12
*
*/
#include "head.h"

//Global variable
extern usernote cur_user;			//current user
extern SqStack cur_dir;     		//current directory
extern Sys_cmd cmd[COM_NUM];				//23 commands
extern usernote L_user[USER_COUNT];			//users array
extern int f_inode;					//current active inode number

extern int path_tnode(SqStack S, FTreepoint T, FTreepoint &p);		//find the root node of inode
extern char buffer[FILE_BUFFER];					//file content buffer
extern int find_super();
extern void free_disk(int a);
extern FTreepoint L_Ftree;                 //file tree

extern super_block hx_superblock;   //super block
extern inode file_inode[INODES_COUNT];		//inode
extern dir file_dir[DIR_COUNT];			//directory
extern physicalBlock phy[PHY_DATA_SIZE];	//data

extern  UserOpenTable user_open_table[USER_ALLOW_OPEN_COUNT];	//user open table
extern SystemOpenTable sys_open_table[SYSTEM_ALLOW_OPEN_COUNT];	//system open table
extern ActiveNode active_inode_table;		//active inode table

//Read information
void ReadFromFile(FILE *fp);
void WriteToFile(FILE *fp);
void ReadUsers();
void SaveUsers();

//Read from file  
void ReadFromFile(FILE *fp){
	int i;
	//open file with binary stream
	fp = fopen(DISK_NAME, "rb");
	//read super block
	fread(&hx_superblock, sizeof(super_block), 1, fp);
	//read inode
	for (i = 0;i<INODES_COUNT;i++){
		fread(&file_inode[i], sizeof(inode), 1, fp);
	}
	//read directories
	for (i = 0;i<DIR_COUNT;i++)
	{
		fread(&file_dir[i], sizeof(dir), 1, fp);
	}
	//read data
	for (i = 0;i<PHY_DATA_SIZE;i++)
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
	fp = fopen(DISK_NAME, "wb");
	//write super block
	fwrite(&hx_superblock, sizeof(super_block), 1, fp);
	//write inode
	for (i = 0;i<INODES_COUNT;i++){
		fwrite(&file_inode[i], sizeof(inode), 1, fp);
	}
	//write directories
	for (i = 0;i<DIR_COUNT;i++)
	{
		fwrite(&file_dir[i], sizeof(dir), 1, fp);
	}
	//write data
	for (i = 0;i<PHY_DATA_SIZE;i++)
	{
		fwrite(&phy[i], sizeof(physicalBlock), 1, fp);
	}
	fclose(fp);
}

char* r_u(int a) {
	char word[400];
	int b = file_inode[a].file_length, i;
	int num = 0;
	if (b < DATA_COUNT - 2) {
		for (i = 0;i < b;i++) {
			for (int j = 0;j < 10;j++) {
				if (phy[file_inode[a].file_address[i]].p[j] == 0) return word;
				word[num++] = phy[file_inode[a].file_address[i]].p[j];
			}
		}
	}
	else {
		for (int i = 0;i < DATA_COUNT - 3;i++) {
			for (int j = 0;j < 10;j++) {
				if (phy[file_inode[a].file_address[i]].p[j] == 0) return word;
				word[num++] = phy[file_inode[a].file_address[i]].p[j];
			}
		}
	}
	if (b >= DATA_COUNT - 2) {
		//direct addressing 
		int f1[2];
		char b1[5], b2[5];
		for (i = 0;i < 5;i++) {
			b1[i] = phy[file_inode[a].file_address[12]].p[i];
			b2[i] = phy[file_inode[a].file_address[12]].p[i + 5];
		}
		f1[0] = atoi(b1);f1[1] = atoi(b2);
		for (i = 0;i < 2;i++) {
			for (int j = 0;j < 10;j++) {
				if (phy[f1[i]].p[j] == 0) return word;
				word[num++] = phy[file_inode[a].file_address[i]].p[j];
			}
		}
	}
	if (b >= DATA_COUNT - 1) {
		//first level indirect addressing 
		int f1[2], i, j;
		char b1[5], b_2[5];
		for (i = 0;i < 5;i++) {
			b1[i] = phy[file_inode[a].file_address[13]].p[i];
			b_2[i] = phy[file_inode[a].file_address[13]].p[i + 5];
		}
		f1[0] = atoi(b1);f1[1] = atoi(b_2);

		//second level indirect addressing
		int f2[4];
		char b2[4][5];
		for (j = 0;j < 5;j++) {
			b2[0][j] = phy[f1[0]].p[j];
			b2[1][j] = phy[f1[0]].p[j + 5];
			b2[2][j] = phy[f1[1]].p[j];
			b2[3][j] = phy[f1[1]].p[j + 5];
		}
		for (i = 0;i < 4;i++) {
			f2[i] = atoi(b2[i]);
		}

		for (i = 0;i < 4;i++) {
			for (j = 0;j < 10;j++) {
				if (phy[f2[i]].p[j] == 0) return word;
				word[num++] = phy[file_inode[a].file_address[i]].p[j];
			}
		}
	}
	if (b >= DATA_COUNT) {
		//first level indirect addressing
		int f1[2], i, j;
		char b1[5], b_2[5];
		for (i = 0;i < 5;i++) {
			b1[i] = phy[file_inode[a].file_address[14]].p[i];
			b_2[i] = phy[file_inode[a].file_address[14]].p[i + 5];
		}
		f1[0] = atoi(b1);f1[1] = atoi(b_2);

		//second level indirect addressing 
		int f2[4];
		char b2[4][5];
		for (j = 0;j < 5;j++) {
			b2[0][j] = phy[f1[0]].p[j];
			b2[1][j] = phy[f1[0]].p[j + 5];
			b2[2][j] = phy[f1[1]].p[j];
			b2[3][j] = phy[f1[1]].p[j + 5];
		}
		for (i = 0;i < 4;i++) {
			f2[i] = atoi(b2[i]);
		}

		//third level indirect addressing 
		int f3[8];
		char b3[8][5];
		for (j = 0;j < 5;j++) {
			b3[0][j] = phy[f2[0]].p[j];
			b3[1][j] = phy[f2[0]].p[j + 5];
			b3[2][j] = phy[f2[1]].p[j];
			b3[3][j] = phy[f2[1]].p[j + 5];
			b3[4][j] = phy[f2[2]].p[j];
			b3[5][j] = phy[f2[2]].p[j + 5];
			b3[6][j] = phy[f2[3]].p[j];
			b3[7][j] = phy[f2[3]].p[j + 5];
		}
		for (i = 0;i < 8;i++) {
			f3[i] = atoi(b3[i]);
		}


		for (i = 0;i < 8;i++) {
			for (j = 0;j < 10;j++) {
				if (phy[f3[i]].p[j] == 0) return word;
				word[num++] = phy[file_inode[a].file_address[i]].p[j];
			}
		}
	}
}

//read users information
void ReadUsers() {
	char filename[] = "pw";
	int a;     //inode number
	FTreepoint p = NULL, p2 = NULL;
	path_tnode(cur_dir, L_Ftree, p);
	if (p->lchild == NULL) {
		printf(E14);
	}
	else {
		p = p->lchild;
		while (p != NULL) {
			if ((strcmp(p->data.file_name, filename) == 0) && (file_inode[p->data.dir_inode].file_style == 1)) {
				a = p->data.dir_inode;
				char* word = r_u(a);
				int z;
				for (z = 0; z < USER_COUNT; z++) {
					if (word[z * 31] == 0) break;
					L_user[z].userid = word[z * 31] - '0';
					for (int p = 0; p < USER_NAME_LENGTH; p++) {
						if (word[z * 31 + p + 1] == 1)
							L_user[z].username[p] = 0;
						else
							L_user[z].username[p] = word[z * 31 + p + 1];
					}

					for (int p = 0; p < USER_PASSWORD_LENGTH; p++) {
						if (word[z * 31 + p + 1 + USER_NAME_LENGTH] == 1)
							L_user[z].password[p] = 0;
						else
							L_user[z].password[p] = word[z * 31 + p + 1 + USER_NAME_LENGTH];
					}
					L_user[z].group = word[z * 31 + 1 + USER_NAME_LENGTH + USER_PASSWORD_LENGTH] - '0';
					L_user[z].level = word[z * 31 + 1 + USER_NAME_LENGTH + USER_PASSWORD_LENGTH + 1] - '0';
				}
				while (z < USER_COUNT) {
					L_user[z].userid = -1;
					for (int p = 0; p < USER_NAME_LENGTH; p++) {
						L_user[z].username[p] = 0;
						L_user[z].password[p] = 0;
					}
					L_user[z].group = -1;
					L_user[z].level = -1;
				}
				return;
			}
			p = p->rchild;
		}
		printf(E14);
	}
}

//write file
void w_u(int a) {  //a:inode number
	free_disk(a);   //clear
	char ch;
	int num = 0;
	for (int i = 0; i < USER_COUNT; i++) {
		if (L_user[i].userid == -1) break;
		buffer[num++] = L_user[i].userid + '0';
		for (int j = 0; j < USER_NAME_LENGTH; j++) {
			if (L_user[i].username[j] == 0)
				buffer[num++] = 1;
			else
				buffer[num++] = L_user[i].username[j];
		}
		for (int j = 0; j < USER_PASSWORD_LENGTH; j++) {
			if (L_user[i].password[j] == 0)
				buffer[num++] = 1;
			else
				buffer[num++] = L_user[i].password[j];
		}
		buffer[num++] = L_user[i].group + '0';
		buffer[num++] = L_user[i].level + '0';
	}
	buffer[num] = 0;

	file_inode[a].file_length = 0;
	if (num>FILE_BUFFER) {
		printf("The input content out of the size!\n");
		return;
	}
	if (num <= 120 && num>0) {
		if (num % 10 == 0) {
			for (int i = 0;i<num / 10 + 1;i++) {
				file_inode[a].file_address[i] = find_super();
				for (int j = 0;j<10;j++) {
					phy[find_super()].p[j] = buffer[i * 10 + j];
				}
				hx_superblock.phydata[find_super()] = 1;
				file_inode[a].file_length++;
			}
		}
		else {
			for (int i = 0;i<num / 10 + 1;i++) {
				file_inode[a].file_address[i] = find_super();
				for (int j = 0;j<10;j++) {
					phy[find_super()].p[j] = buffer[i * 10 + j];
				}
				hx_superblock.phydata[find_super()] = 1;
				file_inode[a].file_length++;
			}
		}
	}
	else {
		for (int i = 0;i<12;i++) {
			file_inode[a].file_address[i] = find_super();
			for (int j = 0;j<512;j++) {
				phy[find_super()].p[j] = buffer[i * 10 + j];
			}
			hx_superblock.phydata[find_super()] = 1;
			file_inode[a].file_length++;
		}
	}
	if (num>120) {   //简化处理，直接开辟两个存储空间
		file_inode[a].file_address[12] = find_super();
		file_inode[a].file_length++;
		hx_superblock.phydata[find_super()] = 1;
		int first = find_super();
		hx_superblock.phydata[find_super()] = 1;
		int second = find_super();
		hx_superblock.phydata[find_super()] = 1;
		char buf[5] = { 0 }, buf1[5] = { 0 };
		itoa(first, buf, 512);
		itoa(second, buf1, 512);
		int i;
		for (i = 0;i<512;i++) {
			phy[file_inode[a].file_address[12]].p[i] = buf[i];
			phy[file_inode[a].file_address[12]].p[i + 5] = buf1[i];
		}
		//开始存储内容
		for (i = 120;i<130;i++) {
			if (i>num) {
				break;
			}
			phy[first].p[i - 120] = buffer[i];
		}
		for (i = 130;i<140;i++) {
			if (i>num) {
				break;
			}
			phy[second].p[i - 130] = buffer[i];
		}
	}
	if (num>140) {
		file_inode[a].file_address[13] = find_super();
		file_inode[a].file_length++;
		hx_superblock.phydata[find_super()] = 1;
		int first = find_super();
		hx_superblock.phydata[find_super()] = 1;
		int second = find_super();
		hx_superblock.phydata[find_super()] = 1;
		char buf[5] = { 0 }, buf1[5] = { 0 };
		itoa(first, buf, 512);
		itoa(second, buf1, 512);
		//一次间接寻址
		for (int i = 0;i<512;i++) {
			phy[file_inode[a].file_address[13]].p[i] = buf[i];
			phy[file_inode[a].file_address[13]].p[i + 5] = buf1[i];
		}

		int f1, f2, f3, f4, i;
		f1 = find_super();hx_superblock.phydata[f1] = 1;
		f2 = find_super();hx_superblock.phydata[f2] = 1;
		f3 = find_super();hx_superblock.phydata[f3] = 1;
		f4 = find_super();hx_superblock.phydata[f4] = 1;
		char b1[5] = { 0 }, b2[5] = { 0 }, b3[5] = { 0 }, b4[5] = { 0 };
		itoa(f1, b1, 512);itoa(f2, b2, 512);itoa(f3, b3, 512);itoa(f4, b4, 512);
		//二次间接寻址
		for (i = 0;i<5;i++)
		{
			phy[first].p[i] = b1[i];
			phy[first].p[i + 5] = b2[i];
			phy[second].p[i] = b3[i];
			phy[second].p[i + 5] = b4[i];
		}
		//开始存储内容
		for (i = 140;i<150;i++) {
			if (i>num) {
				break;
			}
			phy[f1].p[i - 140] = buffer[i];
		}
		for (i = 150;i<160;i++) {
			if (i>num) {
				break;
			}
			phy[f2].p[i - 150] = buffer[i];
		}
		for (i = 160;i<170;i++) {
			if (i>num) {
				break;
			}
			phy[f3].p[i - 160] = buffer[i];
		}
		for (i = 170;i<180;i++) {
			if (i>num) {
				break;
			}
			phy[f4].p[i - 170] = buffer[i];
		}
	}
	if (num>180) {
		file_inode[a].file_address[14] = find_super();
		file_inode[a].file_length++;
		hx_superblock.phydata[find_super()] = 1;
		int first = find_super();
		hx_superblock.phydata[find_super()] = 1;
		int second = find_super();
		hx_superblock.phydata[find_super()] = 1;
		char buf[5] = { 0 }, buf1[5] = { 0 };
		itoa(first, buf, 512);
		itoa(second, buf1, 512);
		for (int i = 0;i<5;i++) {
			phy[file_inode[a].file_address[14]].p[i] = buf[i];
			phy[file_inode[a].file_address[14]].p[i + 5] = buf1[i];
		}

		int f1, f2, f3, f4, i;
		f1 = find_super();hx_superblock.phydata[f1] = 1;
		f2 = find_super();hx_superblock.phydata[f2] = 1;
		f3 = find_super();hx_superblock.phydata[f3] = 1;
		f4 = find_super();hx_superblock.phydata[f4] = 1;
		char b1[5] = { 0 }, b2[5] = { 0 }, b3[5] = { 0 }, b4[5] = { 0 };
		itoa(f1, b1, 512);itoa(f2, b2, 512);itoa(f3, b3, 512);itoa(f4, b4, 512);

		for (i = 0;i<5;i++)
		{
			phy[first].p[i] = b1[i];
			phy[first].p[i + 5] = b2[i];
			phy[second].p[i] = b3[i];
			phy[second].p[i + 5] = b4[i];
		}

		int ff1, ff2, ff3, ff4, ff5, ff6, ff7, ff8;
		ff1 = find_super();hx_superblock.phydata[ff1] = 1;
		ff2 = find_super();hx_superblock.phydata[ff2] = 1;
		ff3 = find_super();hx_superblock.phydata[ff3] = 1;
		ff4 = find_super();hx_superblock.phydata[ff4] = 1;
		ff5 = find_super();hx_superblock.phydata[ff5] = 1;
		ff6 = find_super();hx_superblock.phydata[ff6] = 1;
		ff7 = find_super();hx_superblock.phydata[ff7] = 1;
		ff8 = find_super();hx_superblock.phydata[ff8] = 1;
		char bb1[5] = { 0 }, bb2[5] = { 0 }, bb3[5] = { 0 }, bb4[5] = { 0 }, bb5[5] = { 0 }, bb6[5] = { 0 }, bb7[5] = { 0 }, bb8[5] = { 0 };
		itoa(ff1, bb1, 512);itoa(ff2, bb2, 512);itoa(ff3, bb3, 512);itoa(ff4, bb4, 512);
		itoa(ff5, bb5, 512);itoa(ff6, bb6, 512);itoa(ff7, bb7, 512);itoa(ff8, bb8, 512);
		for (i = 0;i<5;i++) {
			phy[f1].p[i] = bb1[i];
			phy[f1].p[i + 5] = bb2[i];
			phy[f2].p[i] = bb3[i];
			phy[f2].p[i + 5] = bb4[i];
			phy[f3].p[i] = bb5[i];
			phy[f3].p[i + 5] = bb6[i];
			phy[f4].p[i] = bb7[i];
			phy[f4].p[i + 5] = bb8[i];
		}

		for (i = 180;i<190;i++) {
			if (i>num) {
				break;
			}
			phy[ff1].p[i - 180] = buffer[i];
		}
		for (i = 190;i<200;i++) {
			if (i>num) {
				break;
			}
			phy[ff2].p[i - 190] = buffer[i];
		}
		for (i = 200;i<210;i++) {
			if (i>num) {
				break;
			}
			phy[ff3].p[i - 200] = buffer[i];
		}
		for (i = 210;i<220;i++) {
			if (i>num) {
				break;
			}
			phy[ff4].p[i - 210] = buffer[i];
		}
		for (i = 220;i<230;i++) {
			if (i>num) {
				break;
			}
			phy[ff5].p[i - 220] = buffer[i];
		}
		for (i = 230;i<240;i++) {
			if (i>num) {
				break;
			}
			phy[ff6].p[i - 230] = buffer[i];
		}
		for (i = 240;i<250;i++) {
			if (i>num) {
				break;
			}
			phy[ff7].p[i - 240] = buffer[i];
		}
		for (i = 250;i<260;i++) {
			if (i>num) {
				break;
			}
			phy[ff8].p[i - 250] = buffer[i];
		}
	}
}

//write user information
void SaveUsers()
{
	char filename[] = "pw";
	int a;     //a代表代开文件的inode号
	FTreepoint p = NULL, p2 = NULL;
	path_tnode(cur_dir, L_Ftree, p);
	if (p->lchild == NULL)      //为空
	{
		printf(E14);
	}
	else   //非空
	{
		p = p->lchild;
		while (p != NULL)
		{
			if ((strcmp(p->data.file_name, filename) == 0) && (file_inode[p->data.dir_inode].file_style == 1))  //存在同名文件
			{
				a = p->data.dir_inode;
				w_u(a);

				return;
			}
			p = p->rchild;
		}
		printf(E14);
	}
}