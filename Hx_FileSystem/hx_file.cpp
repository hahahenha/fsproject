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
extern FILE *ff;					//disk
extern int32_t path_tnode(SqStack S, FTreepoint T, FTreepoint &p);		//find the root node of inode
extern char buffer[FILE_BUFFER];					//file content buffer
extern int32_t find_super();
extern void free_disk(int32_t a);
extern FTreepoint L_Ftree;                 //file tree

extern super_block hx_superblock;   //super block
extern inode file_inode[INODES_COUNT];		//inode
extern dir file_dir[DIR_COUNT];			//directory

extern  UserOpenTable user_open_table[USER_ALLOW_OPEN_COUNT];	//user open table
extern SystemOpenTable sys_open_table[SYSTEM_ALLOW_OPEN_COUNT];	//system open table
extern ActiveNode active_inode_table;		//active inode table

//Read information
void ReadFromFile();
void WriteToFile();
void ReadUsers(FILE *fp);
void SaveUsers();

int find_free_inode();

//Read from file  
void ReadFromFile(){
	FILE *fp = fopen(DISK_NAME, "rb");
	int32_t i;
	fseek(fp, 0, SEEK_SET);
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
	fclose(fp);
}
//write to file
void WriteToFile()
{
	fseek(ff, 0, SEEK_SET);
	//open file with write binary stream mode
	int32_t i;
	//write super block
	fwrite(&hx_superblock, sizeof(super_block), 1, ff);
	//write inode
	for (i = 0;i<INODES_COUNT;i++){
		fwrite(&file_inode[i], sizeof(inode), 1, ff);
	}
	//write directories
	for (i = 0;i<DIR_COUNT;i++)
	{
		fwrite(&file_dir[i], sizeof(dir), 1, ff);
	}

}

void r_u(int32_t a, char* word, FILE* fp) {
	int32_t b = file_inode[a].file_length, i, j;
	int32_t num = 0;
	if (b < DATA_COUNT - 2) {
		char cbuf[DATA_BLOCK_SIZE];
		for (i = 0;i < b;i++) {
			fseek(fp, PHY_DATA_START + file_inode[a].file_address[i] * DATA_BLOCK_SIZE, SEEK_SET);
			fread(cbuf, sizeof(cbuf), 1, fp);
			for (int t = 0; t < DATA_BLOCK_SIZE; t++) {
				if (cbuf[t] == 0) break;
				word[num++] = cbuf[t];
			}
		}
	}
	else {
		char cbuf[DATA_BLOCK_SIZE] = { 0 };
		for (i = 0;i < DATA_COUNT - 3;i++) {
			fseek(fp, PHY_DATA_START + file_inode[a].file_address[i] * DATA_BLOCK_SIZE, SEEK_SET);
			fread(cbuf, sizeof(cbuf), 1, fp);
			for (int t = 0; t < DATA_BLOCK_SIZE; t++) {
				if (cbuf[t] == 0) break;
				word[num++] = cbuf[t];
			}
		}
	}
	if (b >= DATA_COUNT - 2) {
		char cbuf[DATA_BLOCK_SIZE] = { 0 };
		int32_t f1[INDIRECT_NUM] = { 0 };
		fseek(fp, PHY_DATA_START + file_inode[a].file_address[DATA_COUNT - 3] * DATA_BLOCK_SIZE, SEEK_SET);
		fread(f1, sizeof(f1), 1, fp);
		for (i = 0;i < INDIRECT_NUM;i++) {
			fseek(fp, PHY_DATA_START + f1[i] * DATA_BLOCK_SIZE, SEEK_SET);
			fread(cbuf, sizeof(cbuf), 1, fp);
			for (int t = 0; t < DATA_BLOCK_SIZE; t++) {
				if (cbuf[t] == 0) break;
				word[num++] = cbuf[t];
			}
		}
	}
	if (b >= DATA_COUNT - 1) {
		char cbuf[DATA_BLOCK_SIZE] = { 0 };
		//first level indirect addressing 
		int32_t f1[INDIRECT_NUM] = { 0 }, i, j;
		fseek(fp, PHY_DATA_START + file_inode[a].file_address[DATA_COUNT - 2] * DATA_BLOCK_SIZE, SEEK_SET);
		fread(f1, sizeof(f1), 1, fp);
		//second level indirect addressing
		int32_t f2[INDIRECT_NUM] = { 0 };
		bool flag = 0;
		for (i = 0; i < INDIRECT_NUM; i++) {
			fseek(fp, PHY_DATA_START + f1[i] * DATA_BLOCK_SIZE, SEEK_SET);
			fread(f2, sizeof(f2), 1, fp);
			for (j = 0;j < INDIRECT_NUM;j++) {
				fseek(fp, PHY_DATA_START + f2[j] * DATA_BLOCK_SIZE, SEEK_SET);
				fread(cbuf, sizeof(cbuf), 1, fp);
				for (int t = 0; t < DATA_BLOCK_SIZE; t++) {
					if (cbuf[t] == 0) {
						flag = 1;
						break;
					}
					word[num++] = cbuf[t];
				}
				if (flag) break;
			}
			if (flag) break;
		}
	}
	if (b >= DATA_COUNT) {
		char cbuf[DATA_BLOCK_SIZE] = { 0 };
		//first level indirect addressing 
		int32_t f1[INDIRECT_NUM] = { 0 }, i, j, k;
		fseek(fp, PHY_DATA_START + file_inode[a].file_address[DATA_COUNT - 1] * DATA_BLOCK_SIZE, SEEK_SET);
		fread(f1, sizeof(f1), 1, fp);
		//second level indirect addressing
		int32_t f2[INDIRECT_NUM] = { 0 }, f3[INDIRECT_NUM] = { 0 };
		bool flag = 0;
		for (i = 0; i < INDIRECT_NUM; i++) {
			fseek(fp, PHY_DATA_START + f1[i] * DATA_BLOCK_SIZE, SEEK_SET);
			fread(f2, sizeof(f2), 1, fp);
			for (j = 0;j < INDIRECT_NUM;j++) {
				fseek(fp, PHY_DATA_START + f2[j] * DATA_BLOCK_SIZE, SEEK_SET);
				fread(f3, sizeof(f3), 1, fp);
				for (k = 0;k < INDIRECT_NUM;k++) {
					fseek(fp, PHY_DATA_START + f3[k] * DATA_BLOCK_SIZE, SEEK_SET);
					fread(cbuf, sizeof(cbuf), 1, fp);
					for (int t = 0; t < DATA_BLOCK_SIZE; t++) {
						if (cbuf[t] == 0) {
							flag = 1;
							break;
						}
						word[num++] = cbuf[t];
					}
					if (flag) break;
				}
				if (flag) break;
			}
			if (flag) break;
		}
	}			
	return;
}
//read users information
void ReadUsers(FILE* fp) {
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
				r_u(a, word, fp);
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
					L_user[z].level = 0;
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
				file_inode[a].file_address[i] = hx_superblock.special_stack.free[find_super()].b_number;
				fseek(ff, PHY_DATA_START + hx_superblock.special_stack.free[find_super()].b_number*DATA_BLOCK_SIZE, SEEK_SET);
				char cbuf[DATA_BLOCK_SIZE] = { 0 };
				for (int32_t j = 0;j<DATA_BLOCK_SIZE;j++) {
					cbuf[j] = buffer[i * DATA_BLOCK_SIZE + j];
				}
				fwrite(cbuf, sizeof(cbuf), 1, ff);
				hx_superblock.special_stack.free[find_super()].flag = 1;
				hx_superblock.special_stack.free_num--;
				file_inode[a].file_length++;
			}
		}
		else {
			for (int32_t i = 0;i<num / DATA_BLOCK_SIZE + 1;i++) {
				file_inode[a].file_address[i] = hx_superblock.special_stack.free[find_super()].b_number;
				fseek(ff, PHY_DATA_START + file_inode[a].file_address[i] * DATA_BLOCK_SIZE, SEEK_SET);
				char cbuf[DATA_BLOCK_SIZE] = { 0 };
				for (int32_t j = 0;j<DATA_BLOCK_SIZE;j++) {
					if (i * DATA_BLOCK_SIZE + j > num) break;
					cbuf[j] = buffer[i * DATA_BLOCK_SIZE + j];
				}
				fwrite(cbuf, sizeof(cbuf), 1, ff);
				hx_superblock.special_stack.free[find_super()].flag = 1;
				hx_superblock.special_stack.free_num--;
				file_inode[a].file_length++;
			}
		}
	}
	else {
		for (int32_t i = 0;i<DATA_COUNT - 3;i++) {
			file_inode[a].file_address[i] = hx_superblock.special_stack.free[find_super()].b_number;
			fseek(ff, PHY_DATA_START + file_inode[a].file_address[i] * DATA_BLOCK_SIZE, SEEK_SET);
			char cbuf[DATA_BLOCK_SIZE] = { 0 };
			for (int32_t j = 0;j<DATA_BLOCK_SIZE;j++) {
				cbuf[j] = buffer[i * DATA_BLOCK_SIZE + j];
			}
			fwrite(cbuf, sizeof(cbuf), 1, ff);
			hx_superblock.special_stack.free[find_super()].flag = 1;
			hx_superblock.special_stack.free_num--;
			file_inode[a].file_length++;
		}
	}
	bool flag = 0;
	if (num>(DATA_COUNT - 3)*DATA_BLOCK_SIZE) {
		file_inode[a].file_address[DATA_COUNT - 3] = hx_superblock.special_stack.free[find_super()].b_number;
		file_inode[a].file_length++;
		int32_t Inodenum[INDIRECT_NUM] = { 0 };
		for (int32_t i = 0; i < INDIRECT_NUM; i++) {
			hx_superblock.special_stack.free[find_super()].flag = 1;
			hx_superblock.special_stack.free_num--;
			Inodenum[i] = hx_superblock.special_stack.free[find_super()].b_number;
		}
		hx_superblock.special_stack.free[find_super()].flag = 1;
		hx_superblock.special_stack.free_num--;
		int32_t i, j;
		fseek(ff, PHY_DATA_START + file_inode[a].file_address[DATA_COUNT - 3] * DATA_BLOCK_SIZE, SEEK_SET);
		fwrite(Inodenum, sizeof(Inodenum), 1, ff);
		//save
		for (j = 0; j < INDIRECT_NUM; j++) {
			char cbuf[DATA_BLOCK_SIZE] = { 0 };
			for (i = (DATA_COUNT - 3 + j)*DATA_BLOCK_SIZE; i < (DATA_COUNT - 2 + j) * DATA_BLOCK_SIZE; i++) {
				if (i > num) {
					cbuf[i - (DATA_COUNT - 3 + j)*DATA_BLOCK_SIZE] = 0;
					flag = 1;
					break;
				}
				cbuf[i - (DATA_COUNT - 3 + j)*DATA_BLOCK_SIZE] = buffer[i];
			}
			fseek(ff, PHY_DATA_START + Inodenum[j] * DATA_BLOCK_SIZE, SEEK_SET);
			fwrite(cbuf, sizeof(cbuf), 1, ff);
			if (flag) break;
		}
	}
	if (num>(DATA_COUNT + FIRST_INDIRECT_NUM - 3)*DATA_BLOCK_SIZE) {
		file_inode[a].file_address[DATA_COUNT - 2] = hx_superblock.special_stack.free[find_super()].b_number;
		file_inode[a].file_length++;
		int32_t Inodenum[INDIRECT_NUM] = { 0 };
		for (int32_t i = 0; i < INDIRECT_NUM; i++) {
			hx_superblock.special_stack.free[find_super()].flag = 1;
			hx_superblock.special_stack.free_num--;
			Inodenum[i] = hx_superblock.special_stack.free[find_super()].b_number;
		}
		hx_superblock.special_stack.free[find_super()].flag = 1;
		hx_superblock.special_stack.free_num--;
		fseek(ff, PHY_DATA_START + file_inode[a].file_address[DATA_COUNT - 2] * DATA_BLOCK_SIZE, SEEK_SET);
		fwrite(Inodenum, sizeof(Inodenum), 1, ff);
		int32_t i, j, k;
		//second
		int32_t f2[INDIRECT_NUM] = { 0 };
		for (k = 0; k < INDIRECT_NUM; k++) {
			for (i = 0; i < INDIRECT_NUM; i++) {
				f2[i] = hx_superblock.special_stack.free[find_super()].b_number;
				hx_superblock.special_stack.free[find_super()].flag = 1;
				hx_superblock.special_stack.free_num--;
			}
			fseek(ff, PHY_DATA_START + Inodenum[k] * DATA_BLOCK_SIZE, SEEK_SET);
			fwrite(f2, sizeof(f2), 1, ff);
			//save
			for (j = 0; j < INDIRECT_NUM; j++) {
				char cbuf[DATA_BLOCK_SIZE] = { 0 };
				for (i = (DATA_COUNT - 3 + FIRST_INDIRECT_NUM + k * INDIRECT_NUM + j)*DATA_BLOCK_SIZE; i < (DATA_COUNT + FIRST_INDIRECT_NUM + k * INDIRECT_NUM - 2 + j) * DATA_BLOCK_SIZE; i++) {
					if (i > num) {
						cbuf[i - (DATA_COUNT + FIRST_INDIRECT_NUM + k * INDIRECT_NUM - 3 + j)*DATA_BLOCK_SIZE] = 0;
						flag = 1;
						break;
					}
					cbuf[i - (DATA_COUNT + FIRST_INDIRECT_NUM + k * INDIRECT_NUM - 3 + j)*DATA_BLOCK_SIZE] = buffer[i];
				}
				fseek(ff, PHY_DATA_START + f2[j] * DATA_BLOCK_SIZE, SEEK_SET);
				fwrite(cbuf, sizeof(cbuf), 1, ff);
				if (flag) break;
			}
		}
	}
	if (num>(DATA_COUNT + FIRST_INDIRECT_NUM + SECOND_INDIRECT_NUM - 3)*DATA_BLOCK_SIZE) {
		file_inode[a].file_address[DATA_COUNT - 1] = hx_superblock.special_stack.free[find_super()].b_number;
		file_inode[a].file_length++;
		int32_t Inodenum[INDIRECT_NUM] = { 0 };
		for (int32_t i = 0; i < INDIRECT_NUM; i++) {
			hx_superblock.special_stack.free[find_super()].flag = 1;
			hx_superblock.special_stack.free_num--;
			Inodenum[i] = hx_superblock.special_stack.free[find_super()].b_number;
		}
		hx_superblock.special_stack.free[find_super()].flag = 1;
		hx_superblock.special_stack.free_num--;
		fseek(ff, PHY_DATA_START + file_inode[a].file_address[DATA_COUNT - 1] * DATA_BLOCK_SIZE, SEEK_SET);
		fwrite(Inodenum, sizeof(Inodenum), 1, ff);
		int32_t i, j, k, p, q;
		//second
		int32_t f2[INDIRECT_NUM] = { 0 }, f3[INDIRECT_NUM];
		for (k = 0; k < INDIRECT_NUM; k++) {
			for (i = 0; i < INDIRECT_NUM; i++) {
				f2[i] = hx_superblock.special_stack.free[find_super()].b_number;
				hx_superblock.special_stack.free[find_super()].flag = 1;
				hx_superblock.special_stack.free_num--;
			}
			fseek(ff, PHY_DATA_START + Inodenum[k] * DATA_BLOCK_SIZE, SEEK_SET);
			fwrite(f2, sizeof(f2), 1, ff);
			for (p = 0; p < INDIRECT_NUM; p++) {
				for (i = 0; i < INDIRECT_NUM; i++) {
					f3[i] = hx_superblock.special_stack.free[find_super()].b_number;
					hx_superblock.special_stack.free[find_super()].flag = 1;
					hx_superblock.special_stack.free_num--;
				}
				if (flag) continue;
				fseek(ff, PHY_DATA_START + f2[p] * DATA_BLOCK_SIZE, SEEK_SET);
				fwrite(f3, sizeof(f3), 1, ff);
				//save
				for (j = 0; j < INDIRECT_NUM; j++) {
					char cbuf[DATA_BLOCK_SIZE] = { 0 };
					for (i = (DATA_COUNT - 3 + FIRST_INDIRECT_NUM + SECOND_INDIRECT_NUM + k * INDIRECT_NUM * INDIRECT_NUM + p * INDIRECT_NUM + j)*DATA_BLOCK_SIZE; i < (DATA_COUNT + FIRST_INDIRECT_NUM + SECOND_INDIRECT_NUM + k * INDIRECT_NUM * INDIRECT_NUM + p * INDIRECT_NUM - 2 + j) * DATA_BLOCK_SIZE; i++) {
						if (i > num) {
							cbuf[i - (DATA_COUNT + FIRST_INDIRECT_NUM + SECOND_INDIRECT_NUM + k * INDIRECT_NUM * INDIRECT_NUM + p * INDIRECT_NUM - 3 + j)*DATA_BLOCK_SIZE] = 0;
							flag = 1;
							break;
						}
						cbuf[i - (DATA_COUNT + FIRST_INDIRECT_NUM + SECOND_INDIRECT_NUM + k * INDIRECT_NUM * INDIRECT_NUM + p * INDIRECT_NUM - 3 + j)*DATA_BLOCK_SIZE] = buffer[i];
					}
					fseek(ff, PHY_DATA_START + f3[j] * DATA_BLOCK_SIZE, SEEK_SET);
					fwrite(cbuf, sizeof(cbuf), 1, ff);
					if (flag) break;
				}
			}
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