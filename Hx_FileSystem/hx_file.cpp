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
extern int32_t f_inode;					//current active inode number

extern int32_t path_tnode(SqStack S, FTreepoint T, FTreepoint &p);		//find the root node of inode
extern char buffer[FILE_BUFFER];					//file content buffer
extern int32_t find_super();
extern void free_disk(int32_t a);
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
	int32_t i;
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
	int32_t i;
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

void r_u(int32_t a, char* word) {
	int32_t b = file_inode[a].file_length, i, j;
	int32_t num = 0;
	if (b < DATA_COUNT - 2) {
		for (i = 0;i < b;i++) {
			for (j = 0;j < DATA_BLOCK_SIZE;j++) {
				if (phy[file_inode[a].file_address[i]].p[j] == 0) return;
				word[num++] = phy[file_inode[a].file_address[i]].p[j];
			}
		}
	}
	else {
		for (i = 0;i < DATA_COUNT - 3;i++) {
			for (j = 0;j < DATA_BLOCK_SIZE;j++) {
				if (phy[file_inode[a].file_address[i]].p[j] == 0) return;
				word[num++] = phy[file_inode[a].file_address[i]].p[j];
			}
		}
	}
	if (b >= DATA_COUNT - 2) {
		int32_t f1[FIRST_INDIRECT_NUM];
		char b1[FIRST_INDIRECT_NUM][DATA_BLOCK_SIZE / FIRST_INDIRECT_NUM];
		for (i = 0;i < DATA_BLOCK_SIZE / FIRST_INDIRECT_NUM;i++) {
			for (j = 0; j < FIRST_INDIRECT_NUM; j++)
				b1[j][i] = phy[file_inode[a].file_address[DATA_COUNT - 3]].p[i + (DATA_BLOCK_SIZE / FIRST_INDIRECT_NUM)*(j%FIRST_INDIRECT_NUM)];
		}
		for (i = 0; i < FIRST_INDIRECT_NUM; i++) {
			f1[i] = atoi(b1[i]);
		}
		for (i = 0;i < FIRST_INDIRECT_NUM;i++) {
			for (int32_t j = 0;j < DATA_BLOCK_SIZE;j++) {
				if (phy[f1[i]].p[j] == 0) return;
				word[num++] = phy[f1[i]].p[j];
			}
		}
	}
	if (b >= DATA_COUNT - 1) {
		//first level indirect addressing 
		int32_t f1[FIRST_INDIRECT_NUM], i, j;
		char b1[FIRST_INDIRECT_NUM][DATA_BLOCK_SIZE / FIRST_INDIRECT_NUM];
		for (i = 0;i < DATA_BLOCK_SIZE / FIRST_INDIRECT_NUM;i++) {
			for (j = 0; j < FIRST_INDIRECT_NUM; j++)
				b1[j][i] = phy[file_inode[a].file_address[DATA_COUNT - 2]].p[i + (DATA_BLOCK_SIZE / FIRST_INDIRECT_NUM)*(j%FIRST_INDIRECT_NUM)];
		}
		for (i = 0; i < FIRST_INDIRECT_NUM; i++) {
			f1[i] = atoi(b1[i]);
		}

		//second level indirect addressing
		int32_t f2[SECOND_INDIRECT_NUM];
		char b2[SECOND_INDIRECT_NUM][DATA_BLOCK_SIZE / (SECOND_INDIRECT_NUM / FIRST_INDIRECT_NUM)];
		for (j = 0;j < DATA_BLOCK_SIZE / (SECOND_INDIRECT_NUM / FIRST_INDIRECT_NUM);j++) {
			for (i = 0; i < SECOND_INDIRECT_NUM; i++) {
				b2[i][j] = phy[f1[i / (SECOND_INDIRECT_NUM / FIRST_INDIRECT_NUM)]].p[j + DATA_BLOCK_SIZE / (SECOND_INDIRECT_NUM / FIRST_INDIRECT_NUM) * (i % (SECOND_INDIRECT_NUM / FIRST_INDIRECT_NUM))];
			}
		}
		for (i = 0;i < SECOND_INDIRECT_NUM;i++) {
			f2[i] = atoi(b2[i]);
		}

		for (i = 0;i < SECOND_INDIRECT_NUM;i++) {
			for (j = 0;j < DATA_BLOCK_SIZE;j++) {
				if (phy[f2[i]].p[j] == 0) return;
				word[num++] = phy[f2[i]].p[j];
			}
		}
	}
	if (b >= DATA_COUNT) {
		//first level indirect addressing
		int32_t f1[FIRST_INDIRECT_NUM], i, j;
		char b1[FIRST_INDIRECT_NUM][DATA_BLOCK_SIZE / FIRST_INDIRECT_NUM];
		for (i = 0;i < DATA_BLOCK_SIZE / FIRST_INDIRECT_NUM;i++) {
			for (j = 0; j < FIRST_INDIRECT_NUM; j++) {
				b1[j][i] = phy[file_inode[a].file_address[DATA_COUNT - 1]].p[i + DATA_BLOCK_SIZE / FIRST_INDIRECT_NUM*(j%FIRST_INDIRECT_NUM)];
			}
		}
		for (i = 0; i < FIRST_INDIRECT_NUM; i++) {
			f1[i] = atoi(b1[i]);
		}

		//second level indirect addressing 
		int32_t f2[SECOND_INDIRECT_NUM];
		char b2[SECOND_INDIRECT_NUM][DATA_BLOCK_SIZE / (SECOND_INDIRECT_NUM / FIRST_INDIRECT_NUM)];
		for (j = 0;j < DATA_BLOCK_SIZE / (SECOND_INDIRECT_NUM / FIRST_INDIRECT_NUM);j++) {
			for (i = 0; i < SECOND_INDIRECT_NUM; i++) {
				b2[i][j] = phy[f1[i / (SECOND_INDIRECT_NUM / FIRST_INDIRECT_NUM)]].p[j + DATA_BLOCK_SIZE / (SECOND_INDIRECT_NUM / FIRST_INDIRECT_NUM) * (i % (SECOND_INDIRECT_NUM / FIRST_INDIRECT_NUM))];
			}
		}
		for (i = 0;i < SECOND_INDIRECT_NUM;i++) {
			f2[i] = atoi(b2[i]);
		}

		//third level indirect addressing 
		int32_t f3[THIRD_INDIRECT_NUM];
		char b3[THIRD_INDIRECT_NUM][DATA_BLOCK_SIZE / (THIRD_INDIRECT_NUM / SECOND_INDIRECT_NUM)];
		for (j = 0;j < DATA_BLOCK_SIZE / (THIRD_INDIRECT_NUM / SECOND_INDIRECT_NUM);j++) {
			for (i = 0; i < THIRD_INDIRECT_NUM; i++) {
				b3[i][j] = phy[f2[i / (THIRD_INDIRECT_NUM / SECOND_INDIRECT_NUM)]].p[j + DATA_BLOCK_SIZE / (THIRD_INDIRECT_NUM / SECOND_INDIRECT_NUM) * (i % (THIRD_INDIRECT_NUM / SECOND_INDIRECT_NUM))];
			}
		}
		for (i = 0;i < THIRD_INDIRECT_NUM;i++) {
			f3[i] = atoi(b3[i]);
		}


		for (i = 0;i < THIRD_INDIRECT_NUM;i++) {
			for (j = 0;j < DATA_BLOCK_SIZE;j++) {
				if (phy[f3[i]].p[j] == 0) return;
				word[num++] = phy[f3[i]].p[j];
			}
		}
	}
						
	return;
}

//read users information
void ReadUsers() {
	char filename[] = "pw";
	char word[FILE_BUFFER] = { 0 };
	int32_t a;     //inode number
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
				r_u(a, word);
				int32_t z;
				for (z = 0; z < USER_COUNT; z++) {
					if (word[z * 31] == 0) break;
					L_user[z].userid = word[z * 31] - '0';
					for (int32_t p = 0; p < USER_NAME_LENGTH; p++) {
						if (word[z * 31 + p + 1] == 1)
							L_user[z].username[p] = 0;
						else
							L_user[z].username[p] = word[z * 31 + p + 1];
					}

					for (int32_t p = 0; p < USER_PASSWORD_LENGTH; p++) {
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
					for (int32_t p = 0; p < USER_NAME_LENGTH; p++) {
						L_user[z].username[p] = 0;
						L_user[z].password[p] = 0;
					}
					L_user[z].group = -1;
					L_user[z].level = -1;
					z++;
				}
				return;
			}
			p = p->rchild;
		}
		printf(E14);
	}
}

//write file
void w_u(int32_t a) {  //a:inode number
	free_disk(a);   //clear
	char ch;
	int32_t num = 0;
	for (int32_t i = 0; i < USER_COUNT; i++) {
		if (L_user[i].userid == -1) break;
		buffer[num++] = L_user[i].userid + '0';
		for (int32_t j = 0; j < USER_NAME_LENGTH; j++) {
			if (L_user[i].username[j] == 0)
				buffer[num++] = 1;
			else
				buffer[num++] = L_user[i].username[j];
		}
		for (int32_t j = 0; j < USER_PASSWORD_LENGTH; j++) {
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
	if (num > FILE_BUFFER) {
		printf("The input content out of the size!\n");
		return;
	}
	if (num <= (DATA_COUNT - 3)*DATA_BLOCK_SIZE && num > 0) {
		if (num % DATA_BLOCK_SIZE == 0) {
			for (int32_t i = 0;i < num / DATA_BLOCK_SIZE;i++) {
				file_inode[a].file_address[i] = find_super();
				for (int32_t j = 0;j<DATA_BLOCK_SIZE;j++) {
					phy[find_super()].p[j] = buffer[i * DATA_BLOCK_SIZE + j];
				}
				hx_superblock.phydata[find_super()] = 1;
				file_inode[a].file_length++;
			}
		}
		else {
			for (int32_t i = 0;i<num / DATA_BLOCK_SIZE + 1;i++) {
				file_inode[a].file_address[i] = find_super();
				for (int32_t j = 0;j<DATA_BLOCK_SIZE;j++) {
					phy[find_super()].p[j] = buffer[i * DATA_BLOCK_SIZE + j];
				}
				hx_superblock.phydata[find_super()] = 1;
				file_inode[a].file_length++;
			}
		}
	}
	else {
		for (int32_t i = 0;i<DATA_COUNT - 3;i++) {
			file_inode[a].file_address[i] = find_super();
			for (int32_t j = 0;j<DATA_BLOCK_SIZE;j++) {
				phy[find_super()].p[j] = buffer[i * DATA_BLOCK_SIZE + j];
			}
			hx_superblock.phydata[find_super()] = 1;
			file_inode[a].file_length++;
		}
	}
	bool flag = 0;
	if (num>(DATA_COUNT - 3)*DATA_BLOCK_SIZE) {
		file_inode[a].file_address[DATA_COUNT - 3] = find_super();
		file_inode[a].file_length++;
		int32_t Inodenum[FIRST_INDIRECT_NUM];
		for (int32_t i = 0; i < FIRST_INDIRECT_NUM; i++) {
			hx_superblock.phydata[find_super()] = 1;
			Inodenum[i] = find_super();
		}
		hx_superblock.phydata[find_super()] = 1;
		char buf[FIRST_INDIRECT_NUM][DATA_BLOCK_SIZE / FIRST_INDIRECT_NUM] = { 0 };
		for (int32_t i = 0; i < FIRST_INDIRECT_NUM; i++) {
			itoa(Inodenum[i], buf[i], DATA_BLOCK_SIZE);
		}
		int32_t i, j;
		for (i = 0;i < DATA_BLOCK_SIZE; i++) {
			for (j = 0; j < FIRST_INDIRECT_NUM; j++) {
				phy[file_inode[a].file_address[DATA_COUNT - 3]].p[i + (DATA_BLOCK_SIZE / FIRST_INDIRECT_NUM) * (j%FIRST_INDIRECT_NUM)] = buf[j][i];
			}
		}
		//save
		for (j = 0; j < FIRST_INDIRECT_NUM; j++) {
			for (i = (DATA_COUNT - 3 + j)*DATA_BLOCK_SIZE; i < (DATA_COUNT - 2 + j) * DATA_BLOCK_SIZE; i++) {
				if (i > num) {
					flag = 1;
					break;
				}
				phy[Inodenum[j]].p[i - (DATA_COUNT - 3 + j)*DATA_BLOCK_SIZE] = buffer[i];
			}
			if (flag) break;
		}
	}
	if (num>(DATA_COUNT + FIRST_INDIRECT_NUM - 3)*DATA_BLOCK_SIZE) {
		file_inode[a].file_address[DATA_COUNT - 2] = find_super();
		file_inode[a].file_length++;
		int32_t Inodenum[FIRST_INDIRECT_NUM];
		for (int32_t i = 0; i < FIRST_INDIRECT_NUM; i++) {
			hx_superblock.phydata[find_super()] = 1;
			Inodenum[i] = find_super();
		}
		hx_superblock.phydata[find_super()] = 1;
		char buf[FIRST_INDIRECT_NUM][DATA_BLOCK_SIZE / FIRST_INDIRECT_NUM] = { 0 };
		for (int32_t i = 0; i < FIRST_INDIRECT_NUM; i++) {
			itoa(Inodenum[i], buf[i], DATA_BLOCK_SIZE);
		}
		for (int32_t i = 0;i < DATA_BLOCK_SIZE; i++) {
			for (int32_t j = 0; j < FIRST_INDIRECT_NUM; j++) {
				phy[file_inode[a].file_address[DATA_COUNT - 2]].p[i + (DATA_BLOCK_SIZE / FIRST_INDIRECT_NUM) * (j%FIRST_INDIRECT_NUM)] = buf[j][i];
			}
		}
		//first
		int32_t i, j;
		for (j = 0; j < FIRST_INDIRECT_NUM; j++) {
			for (i = (DATA_COUNT - 3 + j)* DATA_BLOCK_SIZE; i < (DATA_COUNT + j - 2) * DATA_BLOCK_SIZE; i++) {
				phy[Inodenum[j]].p[i - (DATA_COUNT - 3 + j)*DATA_BLOCK_SIZE] = buffer[i];
			}
		}

		int32_t f[SECOND_INDIRECT_NUM];
		for (i = 0; i < SECOND_INDIRECT_NUM; i++) {
			f[i] = find_super();
			hx_superblock.phydata[f[i]] = 1;
		}
		char b[SECOND_INDIRECT_NUM][DATA_BLOCK_SIZE / FIRST_INDIRECT_NUM] = { 0 };
		for (i = 0; i < SECOND_INDIRECT_NUM; i++) {
			itoa(f[i], b[i], DATA_BLOCK_SIZE);
		}
		//second
		for (i = 0;i<DATA_BLOCK_SIZE / (SECOND_INDIRECT_NUM / FIRST_INDIRECT_NUM);i++) {
			for (j = 0; j < SECOND_INDIRECT_NUM; j++) {
				phy[Inodenum[j / (SECOND_INDIRECT_NUM / FIRST_INDIRECT_NUM)]].p[i + (j % (SECOND_INDIRECT_NUM / FIRST_INDIRECT_NUM))*DATA_BLOCK_SIZE / (SECOND_INDIRECT_NUM / FIRST_INDIRECT_NUM)] = b[j][i];
			}
		}
		//save
		for (j = 0; j < SECOND_INDIRECT_NUM; j++) {
			for (i = (DATA_COUNT + FIRST_INDIRECT_NUM + j - 3)*DATA_BLOCK_SIZE; i < (DATA_COUNT + FIRST_INDIRECT_NUM + j - 2)*DATA_BLOCK_SIZE;i++) {
				if (i > num) {
					flag = 1;
					break;
				}
				phy[f[j]].p[i - (DATA_COUNT + FIRST_INDIRECT_NUM + j - 3)*DATA_BLOCK_SIZE] = buffer[i];
			}
			if (flag) break;
		}
	}
	if (num>(DATA_COUNT + FIRST_INDIRECT_NUM + SECOND_INDIRECT_NUM - 3)*DATA_BLOCK_SIZE) {
		file_inode[a].file_address[DATA_COUNT - 1] = find_super();
		file_inode[a].file_length++;

		int32_t Inodenum[FIRST_INDIRECT_NUM];
		for (int32_t i = 0; i < FIRST_INDIRECT_NUM; i++) {
			hx_superblock.phydata[find_super()] = 1;
			Inodenum[i] = find_super();
		}
		hx_superblock.phydata[find_super()] = 1;
		char buf[FIRST_INDIRECT_NUM][DATA_BLOCK_SIZE / FIRST_INDIRECT_NUM] = { 0 };
		for (int32_t i = 0; i < FIRST_INDIRECT_NUM; i++) {
			itoa(Inodenum[i], buf[i], DATA_BLOCK_SIZE);
		}
		//first
		int32_t i, j;
		for (j = 0; j < FIRST_INDIRECT_NUM; j++) {
			for (i = 0; i < DATA_BLOCK_SIZE / FIRST_INDIRECT_NUM; i++) {
				phy[file_inode[a].file_address[DATA_COUNT - 1]].p[i + (DATA_BLOCK_SIZE / FIRST_INDIRECT_NUM) * (j % FIRST_INDIRECT_NUM)] = buf[j][i];
			}
		}
		//second
		int32_t f[SECOND_INDIRECT_NUM];
		for (i = 0; i < SECOND_INDIRECT_NUM; i++) {
			f[i] = find_super();
			hx_superblock.phydata[f[i]] = 1;
		}
		char b[SECOND_INDIRECT_NUM][DATA_BLOCK_SIZE / FIRST_INDIRECT_NUM] = { 0 };
		for (i = 0; i < SECOND_INDIRECT_NUM; i++) {
			itoa(f[i], b[i], DATA_BLOCK_SIZE);
		}
		for (i = 0;i<DATA_BLOCK_SIZE / (SECOND_INDIRECT_NUM / FIRST_INDIRECT_NUM);i++) {
			for (j = 0; j < SECOND_INDIRECT_NUM; j++) {
				phy[Inodenum[j / (SECOND_INDIRECT_NUM / FIRST_INDIRECT_NUM)]].p[i + (j % (SECOND_INDIRECT_NUM / FIRST_INDIRECT_NUM))*DATA_BLOCK_SIZE / (SECOND_INDIRECT_NUM / FIRST_INDIRECT_NUM)] = b[j][i];
			}
		}
		//third
		int32_t ff[THIRD_INDIRECT_NUM];
		for (i = 0; i < THIRD_INDIRECT_NUM; i++) {
			ff[i] = find_super();
			hx_superblock.phydata[ff[i]] = 1;
		}
		char bb[THIRD_INDIRECT_NUM][DATA_BLOCK_SIZE / (THIRD_INDIRECT_NUM / SECOND_INDIRECT_NUM)] = { 0 };
		for (i = 0; i < THIRD_INDIRECT_NUM; i++) {
			itoa(ff[i], bb[i], DATA_BLOCK_SIZE);
		}
		for (i = 0;i < DATA_BLOCK_SIZE / (THIRD_INDIRECT_NUM / SECOND_INDIRECT_NUM);i++) {
			for (j = 0; j < THIRD_INDIRECT_NUM; j++) {
				phy[f[j / (THIRD_INDIRECT_NUM / SECOND_INDIRECT_NUM)]].p[i + DATA_BLOCK_SIZE / (THIRD_INDIRECT_NUM / SECOND_INDIRECT_NUM)*(j % (THIRD_INDIRECT_NUM / SECOND_INDIRECT_NUM))] = bb[j][i];
			}
		}
		//save
		for (j = 0; j < THIRD_INDIRECT_NUM; j++) {
			for (i = (DATA_COUNT + FIRST_INDIRECT_NUM + SECOND_INDIRECT_NUM + j - 3)*DATA_BLOCK_SIZE;i < (DATA_COUNT + FIRST_INDIRECT_NUM + SECOND_INDIRECT_NUM + j - 2)*DATA_BLOCK_SIZE;i++) {
				if (i > num) {
					flag = 1;
					break;
				}
				phy[ff[j]].p[i - (DATA_COUNT + FIRST_INDIRECT_NUM + SECOND_INDIRECT_NUM + j - 3)*DATA_BLOCK_SIZE] = buffer[i];
			}
			if (flag) break;
		}
	}
}

//write user information
void SaveUsers()
{
	char filename[] = "pw";
	int32_t a;
	FTreepoint p = NULL, p2 = NULL;
	path_tnode(cur_dir, L_Ftree, p);
	if (p->lchild == NULL)
	{
		printf(E14);
	}
	else
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