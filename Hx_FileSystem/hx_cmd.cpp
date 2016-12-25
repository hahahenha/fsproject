/*
* Copyright (c) 2016, Randolph Han, Zhejiang University of Technology, China
* All rights reserved.
* Project name：Simple File System
* Programmer：Randolph Han
* Finish：2016.12.11-2016.12.25
*
*/
#include "head.h"

//Global variable
extern usernote cur_user;			//current user
extern SqStack cur_dir;     		//current directory
extern Sys_cmd cmd[COM_NUM];		//25 commands
extern usernote L_user[USER_COUNT];	//users array
extern int f_inode;					//current active inode number

extern super_block hx_superblock;   //super block
extern inode file_inode[INODES_COUNT];	//inode
extern dir file_dir[DIR_COUNT];			//directory
extern physicalBlock phy[PHY_DATA_SIZE];//data

extern char buffer[FILE_BUFFER];					//file content buffer

extern UserOpenTable user_open_table[USER_ALLOW_OPEN_COUNT];	//user open table
extern SystemOpenTable sys_open_table[SYSTEM_ALLOW_OPEN_COUNT];	//system open table
extern ActiveNode active_inode_table;		//active inode table

extern FTreepoint L_Ftree;                 //file tree

//File operating
void ReadFromFile(FILE *fp);	//read file
void WriteToFile(FILE *fp);		//write file
void ReadUsers();		//get users
void SaveUsers();		//save users

								//Init function
void InitSystem(FILE *fp);		//Init system
void InitDisks();				//Init disk
void InitUsers();				//Init users
void InitTable();				//Init table
void InitCommand();				//Init command
void shell(FILE *fp);			//Init shell

//cmd
int login();					//login function
void help();					//help function
void show_curdir();				//show current directory
void go_dir(char tmp[]);		//jump to a directory
void back_dir();				//back to last directory
void create_dir(char tmp[]);	//create a directory
void del_dir(char tmp[]);		//delete a directory
void create_file(char tmp[]);	//create a file
void open_file(char tmp[]);		//open a file
void read_file(char tmp[]);		//read a file
void write_file(char tmp[]);	//write a file
void close_file(char tmp[]);	//close a file
void delete_file(char tmp[]);	//delete a file
void copy_file(char tmp[]);		//copy a file
void show_info();				//show system information
void logout(FILE *fp);			//logout
int change_user(FILE *fp, char tmp[]);	//change user
void change_mode(char tmp[]);	//chang file mode
void change_owner(char filename[]);//change file owner
void change_group(char filename[]);//change file group
void manage_user();				//user management
void h_link(char tmp[]);		//hard link
void s_link(char tmp[]);		//soft link
void rename(char tmp[]);		//rename a file

//path
int InitStack(SqStack &S);		//Init path
int pop(SqStack &S, char e[]);	//pop path
int push(SqStack &S, char e[]);	//push path
int Gettop(SqStack S, char e[]);//get the top path
int find_path(SqStack S);		//get absolute path

//file tree
void InitfileTree(FTreepoint &T);    //Init file tree

void Tree_to_dir(int &num, dir file_dir[], FTreepoint &T);	//save tree to dir
int find_num_of_file();     //sum of files

int path_tnode(SqStack S, FTreepoint T, FTreepoint &p);		//find the root node of inode
void cul_num(FTreepoint &T, int &n_file, int &n_dir);		//count files and directories

int find_free_inode();				//find free inode
void clear_dir(dir file_dir[]);		//clear file_dir

int cul_num_inodetable();			//count the number of active inode table
int cul_num_usetalbe(int i);		//the open files' number user opened, i:userid
int cul_num_systable();				//system open file number
void InsertUserTable(int a, int i); //add a file to user open table
void DelUserTable(int a, int i);	//delete a file from user open table

void change_pwd();					//change password

void help(){
	printf("System command:\n");
	printf("\t01.Exit system....................................(exit)\n");
	printf("\t02.Show help information..........................(help)\n");
	printf("\t03.Show current directory..........................(pwd)\n");
	printf("\t04.File list of current directory...................(ls)\n");
	printf("\t05.Enter the specified directory..........(cd + dirname)\n");
	printf("\t06.Return last directory..........................(cd..)\n");
	printf("\t07.Create a new directory..............(mkdir + dirname)\n");
	printf("\t08.Delete the directory................(rmdir + dirname)\n");
	printf("\t09.Create a new file....................(create + fname)\n");
	printf("\t10.Open a file............................(open + fname)\n");
	printf("\t11.Read the file...........................(cat + fname)\n");
	printf("\t12.Write the file........................(write + fname)\n");
	printf("\t13.Copy a file..............................(cp + fname)\n");
	printf("\t14.Close a file..........................(close + fname)\n");
	printf("\t15.Delete a file............................(rm + fname)\n");
	printf("\t16.System information view........................(info)\n");
	printf("\t17.Close the current user.......................(logout)\n");
	printf("\t18.Change the current user...............(su + username)\n");
	printf("\t19.Change the mode of a file.............(chmod + fname)\n");
	printf("\t20.Change the user of a file.............(chown + fname)\n");
	printf("\t21.Change the group of a file............(chgrp + fname)\n");
	printf("\t22.Rename a file............................(mv + fname)\n");
	printf("\t23.hard link.............................(hlink + fname)\n");
	printf("\t24.soft link.............................(slink + fname)\n");
	printf("\t25.Change password..............................(passwd)\n");
	printf("\t26.User Management Menu..........................(Muser)\n");
}


//login
int login(){
	char username[2*USER_NAME_LENGTH];
	char password[2* USER_PASSWORD_LENGTH];
	int count;
	int i = 0;
	//Only have 5 times to access the system
	for (count = 0; count < TRY_NUM; count++)
	{
		printf("login name:");
		//get username
		scanf("%s", username);
		printf("password:");
		i = 0;
		//get password
		while ((password[i] = getch()) != '\r'){
			if (password[i] == '\b') {
				printf("\b \b");
				password[i--] = '\0';
				password[i] = '\0';
			}
			else {
				printf("*");
				i++;
			}
		}
		password[i] = '\0';
		printf("\n");
		for (i = 0;i<USER_COUNT;i++){
			//comfirm the username & password
			if (L_user[i].userid != -1)
				//correct
				if (((strcmp(username, L_user[i].username) == 0) && (strcmp(password, L_user[i].password) == 0))){
					//ser user information
					cur_user.userid = L_user[i].userid;
					strcpy(cur_user.username, username);
					cur_user.group = L_user[i].group;
					cur_user.level = L_user[i].level;
					printf("Login successed! Input 'help' to get more information.\n");
					return 1;
					break;
				}
		}
		//username or password error
		if (i = USER_COUNT)
			printf(E23);
	}
	//exit system
	if (count >= TRY_NUM){
		printf(E24);
		return 0;
	}
	return 0;
}

//show current directory
void show_curdir(){
	int i;
	//formta
	printf("\tname\ttype\tlength\tpermission\tuser\tgroup\ttime\n");
	FTreepoint p = NULL;
	path_tnode(cur_dir, L_Ftree, p);
	
	p = p->lchild;
	while (p != NULL){
		if (file_inode[p->data.dir_inode].file_style == 1){
			printf("\t%s ", p->data.file_name);
			printf("\t%d\t%d\t", file_inode[p->data.dir_inode].file_style, file_inode[p->data.dir_inode].file_length);
			for (i = 0;i<PERMISSIONS;i++){
				printf("%c", file_inode[p->data.dir_inode].file_mode[i] + 48);
			}
			printf("\t%d\t%d\t", file_inode[p->data.dir_inode].file_userid, file_inode[p->data.dir_inode].file_groupid);
			printf("%s\n", file_inode[p->data.dir_inode].time);
		}
		if (file_inode[p->data.dir_inode].file_style == 0){
			printf("\t%s ", p->data.file_name);
			printf("\t%d\t%d\t", file_inode[p->data.dir_inode].file_style, file_inode[p->data.dir_inode].file_length);
			for (i = 0;i<PERMISSIONS;i++){
				printf("%c", file_inode[p->data.dir_inode].file_mode[i] + 48);
			}
			printf("\t\t\t%s\n", file_inode[p->data.dir_inode].time);
		}
		p = p->rchild;
	}
}


//go to pointed directory
void go_dir(char filename[]){
	FTreepoint p = NULL;
	path_tnode(cur_dir, L_Ftree, p);
	if (p->lchild != NULL){
		p = p->lchild;
		while (p != NULL){
			//directory exists
			if ((file_inode[p->data.dir_inode].file_style == 0) && (strcmp(p->data.file_name, filename) == 0)){
				//change current directory
				push(cur_dir, filename);return;
			}
			p = p->rchild;
		}
	}
	printf(E8);
}


//back to last directory
void back_dir(){
	char c[STACK_SIZE];
	Gettop(cur_dir, c);
	if (strcmp(c, "root") == 0){
		printf(E9);
		return;
	}
	pop(cur_dir, c);
}

//create directory
void create_dir(char filename[]) {
	FTreepoint p = NULL, p2 = NULL;
	path_tnode(cur_dir, L_Ftree, p);
	//no file in the directory
	if (p->lchild == NULL){
		p->lchild = (FTreepoint)malloc(sizeof(FTree));
		p = p->lchild;
		p->lchild = p->rchild = NULL;
		strcpy(p->data.file_name, filename);
	}
	//not null
	else{
		p = p->lchild;
		while (p != NULL){
			if ((strcmp(p->data.file_name, filename) == 0) && (file_inode[p->data.dir_inode].file_style == 0)) {
				printf(E10);
				return;
			}
			if (p->rchild == NULL) break;
			p = p->rchild;
		}
		p->rchild = (FTreepoint)malloc(sizeof(FTree));
		p = p->rchild;
		p->lchild = p->rchild = NULL;
		strcpy(p->data.file_name, filename);
	}
	//get a new inode
	int a = find_free_inode();
	p->data.dir_inode = a;
	{
		char c[STACK_SIZE];Gettop(cur_dir, c);
		strcpy(file_inode[a].dir_name, c);           //file dir
		file_inode[a].file_style = 0;
		file_inode[a].file_icount = 0;
		file_inode[a].file_length = 0;
		file_inode[a].inode_number = a;
		for (int i = 0;i<PERMISSIONS;i++){           //default permission
			file_inode[a].file_mode[i] = 1;
		}
		file_inode[a].file_groupid = cur_user.group;
		file_inode[a].file_userid = cur_user.userid;
	}
	int i = 0;
	clear_dir(file_dir);
	Tree_to_dir(i, file_dir, L_Ftree->lchild);
	FILE *fp = new FILE();
	WriteToFile(fp);
}
//delete the content in directory
void clear_inode_del(FTreepoint T){
	if (T){
		file_inode[T->data.dir_inode].inode_number = -1;      //clear inode
		clear_inode_del(T->lchild);
		clear_inode_del(T->rchild);
	}
}
//delete directory
void del_dir(char filename[]){		
	FTreepoint p = NULL, p2 = NULL, p3;
	path_tnode(cur_dir, L_Ftree, p);
	int flag = -1;
	if (p->lchild == NULL)
	{
		printf(E11);
		return;
	}
	//One file
	if (p->lchild->rchild == NULL){
		path_tnode(cur_dir, L_Ftree, p);
		p2 = p->lchild;
		if ((strcmp(p2->data.file_name, filename) == 0) && (file_inode[p2->data.dir_inode].file_style == 0)){  //delete
			p->lchild = NULL;
		}
	}
	//multiple files
	else {
		p2 = p->lchild;
		if ((strcmp(p2->data.file_name, filename) == 0) && (file_inode[p2->data.dir_inode].file_style == 0)){  //delete
			p->lchild = p2->rchild;
		}
		else{
			p = p->lchild;
			while ((p != NULL) && (p->rchild != NULL)){
				flag = 0;
				//same name
				if ((strcmp(p->rchild->data.file_name, filename) == 0) && (file_inode[p->rchild->data.dir_inode].file_style == 0)){
					p2 = p->rchild;
					p->rchild = p2->rchild;flag = 1;
					break;
				}
				p = p->rchild;
			}
		}
	}
	//check permission
	if (flag == 0) { printf(E11); return; }

	if (cur_user.userid == file_inode[p2->data.dir_inode].file_userid){     //user
		if (file_inode[p2->data.dir_inode].file_mode[1] == 1){
			file_inode[p2->data.dir_inode].inode_number = -1;
			clear_inode_del(p2->lchild);
		}
		else printf(E12);

	}
	else if (cur_user.group == file_inode[p2->data.dir_inode].file_groupid){   //group
	
		if (file_inode[p2->data.dir_inode].file_mode[4] == 1)
		{
			file_inode[p2->data.dir_inode].inode_number = -1;
			clear_inode_del(p2->lchild);
		}
		else printf(E12);

	}
	else{																		//others
		if (file_inode[p2->data.dir_inode].file_mode[7] == 1){
			file_inode[p2->data.dir_inode].inode_number = -1;
			clear_inode_del(p2->lchild);
		}
		else printf(E12);
	}
	if (p2 == NULL) { printf(E11); return; }
	int i = 0;
	clear_dir(file_dir);
	Tree_to_dir(i, file_dir, L_Ftree->lchild);
	FILE *fp1 = new FILE();
	WriteToFile(fp1);
}


//create a file
void create_file(char filename[]){
	FTreepoint p = NULL, p2 = NULL;
	path_tnode(cur_dir, L_Ftree, p);
	if (p->lchild == NULL){      //null
		p->lchild = (FTreepoint)malloc(sizeof(FTree));
		p = p->lchild;
		p->lchild = p->rchild = NULL;
		strcpy(p->data.file_name, filename);
	}
	else{   //not null
		p = p->lchild;
		while (p != NULL){
			if ((strcmp(p->data.file_name, filename) == 0) && (file_inode[p->data.dir_inode].file_style == 1)){ 
				printf(E10);
				return;
			}
			if (p->rchild == NULL) break;
			p = p->rchild;
		}
		p->rchild = (FTreepoint)malloc(sizeof(FTree));
		p = p->rchild;
		p->lchild = p->rchild = NULL;
		strcpy(p->data.file_name, filename);
	}
	//get a new inode
	int a = find_free_inode();
	p->data.dir_inode = a;
	{
		char c[DIR_NAME_LENGTH];Gettop(cur_dir, c);
		strcpy(file_inode[a].dir_name, c);           //file dir
		file_inode[a].file_style = 1;
		file_inode[a].file_icount = 0;
		file_inode[a].file_length = 0;
		file_inode[a].inode_number = a;
		for (int i = 0;i < 9;i++) {						//default permission
			file_inode[a].file_mode[i] = 1;
			if(i > 0 && filename == "pw")
				file_inode[a].file_mode[i] = 0;
		}
		file_inode[a].file_groupid = cur_user.group;
		file_inode[a].file_userid = cur_user.userid;
		time_t t = time(0);
		strftime(file_inode[a].time, sizeof(file_inode[a].time), "%Y/%m/%d %X %A %jday of the current year %z", localtime(&t));
		//puts(file_inode[a].time);
	}
	int i = 0;
	clear_dir(file_dir);
	Tree_to_dir(i, file_dir, L_Ftree->lchild);
	FILE *fp = new FILE();
	WriteToFile(fp);
}

//free file/dir
void free_disk(int a)
{
	//direct addressing
	int i;
	for (i = 0;i<DATA_COUNT-3;i++){
		if (file_inode[a].file_address[i] == -1) break;
		else{
			hx_superblock.phydata[file_inode[a].file_address[i]] = 0;
		}
	}
	//first level addressing
	if (file_inode[a].file_address[DATA_COUNT-3] != -1){
		int f1[2];
		char b1[2][5];
		for (i = 0;i<5;i++){
			b1[0][i] = phy[file_inode[a].file_address[12]].p[i];
			b1[1][i] = phy[file_inode[a].file_address[12]].p[i + 5];
		}
		hx_superblock.phydata[file_inode[a].file_address[12]] = 0;
		for (i = 0;i<2;i++){
			f1[i] = atoi(b1[i]);
		}
		for (i = 0;i<2;i++){
			hx_superblock.phydata[f1[i]] = 0;
		}
	}
	//second level addressing
	if (file_inode[a].file_address[DATA_COUNT-2] != -1){
		int f1[2];
		char b1[2][5];
		for (i = 0;i<5;i++){
			b1[0][i] = phy[file_inode[a].file_address[13]].p[i];
			b1[1][i] = phy[file_inode[a].file_address[13]].p[i + 5];
		}
		hx_superblock.phydata[file_inode[a].file_address[13]] = 0;
		for (i = 0;i<2;i++){
			f1[i] = atoi(b1[i]);
		}
		for (i = 0;i<2;i++){
			hx_superblock.phydata[f1[i]] = 0;
		}
		int f2[4];
		char b2[4][5];
		for (i = 0;i<5;i++){
			b2[0][i] = phy[f1[0]].p[i];
			b2[1][i] = phy[f1[0]].p[i + 5];
			b2[2][i] = phy[f1[1]].p[i];
			b2[3][i] = phy[f1[1]].p[i + 5];
		}
		for (i = 0;i<4;i++){
			f2[i] = atoi(b2[i]);
		}

		for (i = 0;i<4;i++){
			hx_superblock.phydata[f2[i]] = 0;
		}
	}
	//third level addressing
	if (file_inode[a].file_address[DATA_COUNT-1] != -1){
		int f1[2], i;
		char b1[2][5];
		for (i = 0;i<5;i++){
			b1[0][i] = phy[file_inode[a].file_address[14]].p[i];
			b1[1][i] = phy[file_inode[a].file_address[14]].p[i + 5];
		}
		hx_superblock.phydata[file_inode[a].file_address[14]] = 0;
		for (i = 0;i<2;i++){
			f1[i] = atoi(b1[i]);
		}
		for (i = 0;i<2;i++){
			hx_superblock.phydata[f1[i]] = 0;
		}
		int f2[4];
		char b2[4][5];
		for (i = 0;i<5;i++){
			b2[0][i] = phy[f1[0]].p[i];
			b2[1][i] = phy[f1[0]].p[i + 5];
			b2[2][i] = phy[f1[1]].p[i];
			b2[3][i] = phy[f1[1]].p[i + 5];
		}
		for (i = 0;i<4;i++){
			f2[i] = atoi(b2[i]);
		}
		for (i = 0;i<4;i++){
			hx_superblock.phydata[f2[i]] = 0;
		}
		int f3[8];
		char b3[8][5];
		for (i = 0;i<5;i++){
			b3[0][i] = phy[f2[0]].p[i];
			b3[1][i] = phy[f2[0]].p[i + 5];
			b3[2][i] = phy[f2[1]].p[i];
			b3[3][i] = phy[f2[1]].p[i + 5];
			b3[4][i] = phy[f2[2]].p[i];
			b3[5][i] = phy[f2[2]].p[i + 5];
			b3[6][i] = phy[f2[3]].p[i];
			b3[7][i] = phy[f2[3]].p[i + 5];
		}
		for (i = 0;i<8;i++){
			f3[i] = atoi(b3[i]);
		}
		for (i = 0;i<8;i++){
			hx_superblock.phydata[f3[i]] = 0;
		}
	}
	for (i = 0;i<15;i++){
		file_inode[a].file_address[i] = -1;
	}
}

//delete file
void delete_file(char filename[]){
	FTreepoint p = NULL, p2 = NULL, p3;
	path_tnode(cur_dir, L_Ftree, p);
	int flag = -1;
	if (p->lchild == NULL){
		printf(E11);
		return;
	}
	if (p->lchild->rchild == NULL){   //one file only
		path_tnode(cur_dir, L_Ftree, p);
		p2 = p->lchild;
		//check wether if opened
		for (int j = 0;j<SYSTEM_ALLOW_OPEN_COUNT;j++){
			if (sys_open_table[j].f_inode == p2->data.dir_inode){
				printf(E13);return;
			}
		}
		if ((strcmp(p2->data.file_name, filename) == 0) && (file_inode[p2->data.dir_inode].file_style == 1)){  //delete the first file
			if (file_inode[p2->data.dir_inode].file_icount > 0) {
				file_inode[p2->data.dir_inode].file_icount--;
			}
			else if (file_inode[p2->data.dir_inode].file_length>0)
			{
				free_disk(p2->data.dir_inode);
			}
			p->lchild = NULL;
		}
	}
	else{    //multiple files
		p2 = p->lchild;
		if ((strcmp(p2->data.file_name, filename) == 0) && (file_inode[p2->data.dir_inode].file_style == 1)) {  //delete the first file
			//active?
			for (int j = 0;j<200;j++){
				if (sys_open_table[j].f_inode == p2->data.dir_inode){
					printf(E13);return;
				}
			}

			if (file_inode[p2->data.dir_inode].file_length>0){
				free_disk(p2->data.dir_inode);
			}
			p->lchild = p2->rchild;
		}
		else{
			p = p->lchild;
			while ((p != NULL) && (p->rchild != NULL)){
				flag = 0;
				if ((strcmp(p->rchild->data.file_name, filename) == 0) && (file_inode[p->rchild->data.dir_inode].file_style == 1)){  //same name
					p2 = p->rchild;
					for (int j = 0;j<SYSTEM_ALLOW_OPEN_COUNT;j++){
						if (sys_open_table[j].f_inode == p2->data.dir_inode){
							printf(E13);return;
						}
					}
					if (file_inode[p2->data.dir_inode].file_length>0){
						free_disk(p2->data.dir_inode);
					}
					p->rchild = p2->rchild;flag = 1;
					break;
				}
				p = p->rchild;
			}
		}
	}
	//check permission
	if (flag == 0) { printf(E11); return; }

	if (cur_user.userid == file_inode[p2->data.dir_inode].file_userid) {
		if (file_inode[p2->data.dir_inode].file_mode[1] == 1){
			file_inode[p2->data.dir_inode].inode_number = -1;
			clear_inode_del(p2->lchild);
		}
		else printf(E12);

	}
	else if (cur_user.group == file_inode[p2->data.dir_inode].file_groupid){
		if (file_inode[p2->data.dir_inode].file_mode[4] == 1)
		{
			file_inode[p2->data.dir_inode].inode_number = -1;
			clear_inode_del(p2->lchild);
		}
		else printf(E12);

	}
	else{
		if (file_inode[p2->data.dir_inode].file_mode[7] == 1){
			file_inode[p2->data.dir_inode].inode_number = -1;
			clear_inode_del(p2->lchild);
		}
		else printf(E12);

	}
	if (p2 == NULL) { printf(E11); return; }
	int i = 0;
	clear_dir(file_dir);
	Tree_to_dir(i, file_dir, L_Ftree->lchild);
	FILE *fp1 = new FILE();
	WriteToFile(fp1);
}

//open file
void open_file(char filename[]){
	int a;//Inode number
	FTreepoint p = NULL, p2 = NULL;
	path_tnode(cur_dir, L_Ftree, p);
	if (p->lchild == NULL){
		printf(E14);
	}
	else{
		p = p->lchild;
		while (p != NULL){
			if ((strcmp(p->data.file_name, filename) == 0) && (file_inode[p->data.dir_inode].file_style == 1)){  //has same name
				a = p->data.dir_inode;
				if (cur_user.userid == file_inode[a].file_userid){
					if (file_inode[a].file_mode[2] == 1){
						InsertUserTable(a, cur_user.userid);
					}
					else printf(E12);
				}
				else if (cur_user.group == file_inode[a].file_groupid){
					if (file_inode[a].file_mode[5] == 1){
						InsertUserTable(a, cur_user.userid);
					}
					else printf(E12);
				}
				else{
					if (file_inode[a].file_mode[8] == 1){
						InsertUserTable(a, cur_user.userid);
					}
					else printf(E12);
				}
				return;
			}
			p = p->rchild;
		}
		printf(E14);
	}
}

//close file
void close_file(char filename[]){
	int a;     //a代表代开文件的inode号
	FTreepoint p = NULL, p2 = NULL;
	path_tnode(cur_dir, L_Ftree, p);
	if (p->lchild == NULL){
		printf(E14);
	}
	else{
		p = p->lchild;
		while (p != NULL){
			if ((strcmp(p->data.file_name, filename) == 0) && (file_inode[p->data.dir_inode].file_style == 1)){
				a = p->data.dir_inode;

				if (cur_user.userid == file_inode[a].file_userid){
					if (file_inode[a].file_mode[2] == 1){
						DelUserTable(a, cur_user.userid);
					}
					else printf(E12);
				}
				else if (cur_user.group == file_inode[a].file_groupid){
					if (file_inode[a].file_mode[5] == 1){
						DelUserTable(a, cur_user.userid);
					}
					else printf(E12);
				}
				else{
					if (file_inode[a].file_mode[8] == 1){
						DelUserTable(a, cur_user.userid);
					}
					else printf(E12);
				}

				return;
			}
			p = p->rchild;
		}
		printf(E14);
	}
}

void r_f(int a){
	printf("File content：\n");
	int b = file_inode[a].file_length, i;
	if (b < DATA_COUNT-2){
		for (i = 0;i < b;i++){
			for (int j = 0;j < 10;j++){
				if (phy[file_inode[a].file_address[i]].p[j] == 0) return;
				printf("%c", phy[file_inode[a].file_address[i]].p[j]);
			}
		}
	}
	else{
		for (int i = 0;i < DATA_COUNT-3;i++){
			for (int j = 0;j < 10;j++){
				if (phy[file_inode[a].file_address[i]].p[j] == 0) return;
				printf("%c", phy[file_inode[a].file_address[i]].p[j]);
			}
		}
	}
	if (b >= DATA_COUNT-2){
		//direct addressing 
		int f1[2];
		char b1[5], b2[5];
		for (i = 0;i < 5;i++){
			b1[i] = phy[file_inode[a].file_address[12]].p[i];
			b2[i] = phy[file_inode[a].file_address[12]].p[i + 5];
		}
		f1[0] = atoi(b1);f1[1] = atoi(b2);
		for (i = 0;i < 2;i++){
			for (int j = 0;j < 10;j++){
				if (phy[f1[i]].p[j] == 0) return;
				printf("%c", phy[f1[i]].p[j]);
			}
		}
	}
	if (b >= DATA_COUNT-1){
		//first level indirect addressing 
		int f1[2], i, j;
		char b1[5], b_2[5];
		for (i = 0;i < 5;i++){
			b1[i] = phy[file_inode[a].file_address[13]].p[i];
			b_2[i] = phy[file_inode[a].file_address[13]].p[i + 5];
		}
		f1[0] = atoi(b1);f1[1] = atoi(b_2);

		//second level indirect addressing
		int f2[4];
		char b2[4][5];
		for (j = 0;j < 5;j++){
			b2[0][j] = phy[f1[0]].p[j];
			b2[1][j] = phy[f1[0]].p[j + 5];
			b2[2][j] = phy[f1[1]].p[j];
			b2[3][j] = phy[f1[1]].p[j + 5];
		}
		for (i = 0;i < 4;i++){
			f2[i] = atoi(b2[i]);
		}

		for (i = 0;i < 4;i++){
			for (j = 0;j < 10;j++){
				if (phy[f2[i]].p[j] == 0) return;
				printf("%c", phy[f2[i]].p[j]);
			}
		}
	}
	if (b >= DATA_COUNT){
		//first level indirect addressing
		int f1[2], i, j;
		char b1[5], b_2[5];
		for (i = 0;i < 5;i++){
			b1[i] = phy[file_inode[a].file_address[14]].p[i];
			b_2[i] = phy[file_inode[a].file_address[14]].p[i + 5];
		}
		f1[0] = atoi(b1);f1[1] = atoi(b_2);

		//second level indirect addressing 
		int f2[4];
		char b2[4][5];
		for (j = 0;j < 5;j++){
			b2[0][j] = phy[f1[0]].p[j];
			b2[1][j] = phy[f1[0]].p[j + 5];
			b2[2][j] = phy[f1[1]].p[j];
			b2[3][j] = phy[f1[1]].p[j + 5];
		}
		for (i = 0;i < 4;i++){
			f2[i] = atoi(b2[i]);
		}

		//third level indirect addressing 
		int f3[8];
		char b3[8][5];
		for (j = 0;j < 5;j++){
			b3[0][j] = phy[f2[0]].p[j];
			b3[1][j] = phy[f2[0]].p[j + 5];
			b3[2][j] = phy[f2[1]].p[j];
			b3[3][j] = phy[f2[1]].p[j + 5];
			b3[4][j] = phy[f2[2]].p[j];
			b3[5][j] = phy[f2[2]].p[j + 5];
			b3[6][j] = phy[f2[3]].p[j];
			b3[7][j] = phy[f2[3]].p[j + 5];
		}
		for (i = 0;i < 8;i++){
			f3[i] = atoi(b3[i]);
		}


		for (i = 0;i < 8;i++){
			for (j = 0;j < 10;j++){
				if (phy[f3[i]].p[j] == 0) return;
				printf("%c", phy[f3[i]].p[j]);
			}
		}
	}
}

//read file
void read_file(char filename[]){
	int a;     //inode number
	FTreepoint p = NULL, p2 = NULL;
	path_tnode(cur_dir, L_Ftree, p);
	if (p->lchild == NULL){
		printf(E14);
	}
	else{
		p = p->lchild;
		while (p != NULL){
			if ((strcmp(p->data.file_name, filename) == 0) && (file_inode[p->data.dir_inode].file_style == 1)){
				a = p->data.dir_inode;
				if (cur_user.userid == file_inode[a].file_userid){
					if (file_inode[a].file_mode[0] == 1){
						r_f(a);	printf("\n");
					}
					else printf(E12);
				}
				else if (cur_user.group == file_inode[a].file_groupid)   //same group
				{
					if (file_inode[a].file_mode[3] == 1)
					{
						r_f(a);printf("\n");
					}
					else printf(E12);
				}
				else
				{
					if (file_inode[a].file_mode[6] == 1)
					{
						r_f(a);printf("\n");
					}
					else printf(E12);
				}

				return;
			}
			p = p->rchild;
		}
		printf(E14);
	}
}

int find_super(){
	for (int i = 0;i<PHY_DATA_SIZE;i++){
		if (hx_superblock.phydata[i] == 0){
			return i;
		}
	}
	return -1;
}

//write file
void w_f(int a){  //a:inode number
	free_disk(a);   //clear
	char ch;
	int num = 0;
	printf("Please input the content，end with '$'！(单个文件不能超过%d个字符：（按照一个磁盘块%d字符来算）)\n", FILE_BUFFER, DATA_BLOCK_SIZE);
	//buffer
	while ((ch = getchar()) != '$'){
		buffer[num] = ch;
		num++;
	}
	buffer[num] = 0;
	printf("Total: %d！\n", num);

	file_inode[a].file_length = 0;
	if (num>FILE_BUFFER){
		printf("The input content out of the size!\n");
		return;
	}
	if (num <= 120 && num>0){
		if (num % 10 == 0){
			for (int i = 0;i<num / 10+1;i++){
				file_inode[a].file_address[i] = find_super();
				for (int j = 0;j<10;j++){
					phy[find_super()].p[j] = buffer[i * 10 + j];
				}
				hx_superblock.phydata[find_super()] = 1;
				file_inode[a].file_length++;
			}
		}
		else{
			for (int i = 0;i<num / 10 + 1;i++){
				file_inode[a].file_address[i] = find_super();
				for (int j = 0;j<10;j++){
					phy[find_super()].p[j] = buffer[i * 10 + j];
				}
				hx_superblock.phydata[find_super()] = 1;
				file_inode[a].file_length++;
			}
		}
	}
	else{
		for (int i = 0;i<12;i++){
			file_inode[a].file_address[i] = find_super();
			for (int j = 0;j<512;j++){
				phy[find_super()].p[j] = buffer[i * 10 + j];
			}
			hx_superblock.phydata[find_super()] = 1;
			file_inode[a].file_length++;
		}
	}
	if (num>120){   //简化处理，直接开辟两个存储空间
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
		for (i = 0;i<512;i++){
			phy[file_inode[a].file_address[12]].p[i] = buf[i];
			phy[file_inode[a].file_address[12]].p[i + 5] = buf1[i];
		}
		//开始存储内容
		for (i = 120;i<130;i++){
			if (i>num){
				break;
			}
			phy[first].p[i - 120] = buffer[i];
		}
		for (i = 130;i<140;i++){
			if (i>num){
				break;
			}
			phy[second].p[i - 130] = buffer[i];
		}
	}
	if (num>140){ 
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
		for (int i = 0;i<512;i++){
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
		for (i = 140;i<150;i++){
			if (i>num){
				break;
			}
			phy[f1].p[i - 140] = buffer[i];
		}
		for (i = 150;i<160;i++){
			if (i>num){
				break;
			}
			phy[f2].p[i - 150] = buffer[i];
		}
		for (i = 160;i<170;i++){
			if (i>num){
				break;
			}
			phy[f3].p[i - 160] = buffer[i];
		}
		for (i = 170;i<180;i++){
			if (i>num){
				break;
			}
			phy[f4].p[i - 170] = buffer[i];
		}
	}
	if (num>180){
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
		for (int i = 0;i<5;i++){
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
		for (i = 0;i<5;i++){
			phy[f1].p[i] = bb1[i];
			phy[f1].p[i + 5] = bb2[i];
			phy[f2].p[i] = bb3[i];
			phy[f2].p[i + 5] = bb4[i];
			phy[f3].p[i] = bb5[i];
			phy[f3].p[i + 5] = bb6[i];
			phy[f4].p[i] = bb7[i];
			phy[f4].p[i + 5] = bb8[i];
		}

		for (i = 180;i<190;i++){
			if (i>num){
				break;
			}
			phy[ff1].p[i - 180] = buffer[i];
		}
		for (i = 190;i<200;i++){
			if (i>num){
				break;
			}
			phy[ff2].p[i - 190] = buffer[i];
		}
		for (i = 200;i<210;i++){
			if (i>num){
				break;
			}
			phy[ff3].p[i - 200] = buffer[i];
		}
		for (i = 210;i<220;i++){
			if (i>num){
				break;
			}
			phy[ff4].p[i - 210] = buffer[i];
		}
		for (i = 220;i<230;i++){
			if (i>num){
				break;
			}
			phy[ff5].p[i - 220] = buffer[i];
		}
		for (i = 230;i<240;i++){
			if (i>num){
				break;
			}
			phy[ff6].p[i - 230] = buffer[i];
		}
		for (i = 240;i<250;i++){
			if (i>num){
				break;
			}
			phy[ff7].p[i - 240] = buffer[i];
		}
		for (i = 250;i<260;i++){
			if (i>num){
				break;
			}
			phy[ff8].p[i - 250] = buffer[i];
		}
	}
}

//写文件（单个文件不能超过2600个字符即：(12+2+4+8)*10=2600）（按照一个磁盘块10字符来算）一旦写不论写满与否均认为写满
void write_file(char filename[])
{
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
				//关闭文件操作，用户表，系统登记表和活动inode表都要操作
				a = p->data.dir_inode;
				if (cur_user.userid == file_inode[a].file_userid)     //属主
				{
					if (file_inode[a].file_mode[1] == 1)
					{
						w_f(a);
					}
					else printf(E12);
				}
				else if (cur_user.group == file_inode[a].file_groupid)   //同组
				{
					if (file_inode[a].file_mode[4] == 1)
					{
						w_f(a);
					}
					else printf(E12);
				}
				else
				{
					if (file_inode[a].file_mode[7] == 1)
					{
						w_f(a);
					}
					else printf(E12);
				}

				return;
			}
			p = p->rchild;
		}
		printf(E14);
	}
}

//show system information
void show_info()
{
	printf("\t*************************\n");

	printf("\tSystem Information:\n\n");
	int n_file, n_dir;
	n_dir = n_file = 0;
	int i, m, k = 0;
	for (i = 0;i<PHY_DATA_SIZE;i++)
	{
		if (hx_superblock.phydata[i] == 0)
			k++;
	}
	m = PHY_DATA_SIZE - k;
	printf("\tFree block:\t");
	printf("%d\n", k);
	printf("\tAlready used:\t");
	printf("%d\n", m);

	printf("\t*************************\n");
	printf("\tSystem open file information:\n\n");
	printf("\tCurrent user open table:\t");
	printf("%d\n", cul_num_usetalbe(cur_user.userid));
	printf("\tSystem open table:\t");
	printf("%d\n", cul_num_systable());
	printf("\tActive inode table:\t");
	printf("%d\n", cul_num_inodetable());
	printf("\t*************************\n");

	printf("\tUsed active Inode:\t");
	printf("%d\n", find_num_of_file());
	printf("\tReset active Inode\t");
	printf("%d\n", INODES_COUNT - find_num_of_file());

	cul_num(L_Ftree->lchild, n_file, n_dir);

	printf("\tUsed directory:\t");
	printf("%d\n", n_dir + n_file);
	printf("\tReset directory:\t");
	printf("%d\n", DIR_COUNT - (n_dir + n_file));
	printf("\tFolder:\t");
	printf("\t%d\n", n_dir);
	printf("\tFile:\t");
	printf("\t%d\n", n_file);

	printf("\t****************************\n");
	printf("\tCurrent user:\n\n");
	printf("\t\tUser ID:\t%d\n", cur_user.userid);
	printf("\t\tUser name:\t%s\n", cur_user.username);
	printf("\t\tUser group:\t%d\n", cur_user.group);
}

//logout
void logout(FILE *fp)
{
	//write system information to file
	WriteToFile(fp);
	system("cls");		//********************不符合实验要求函数！！！*********************************
	login();
}
//change user
int change_user(FILE *fp, char username[])
{
	char password[USER_PASSWORD_LENGTH];
	int count;
	int i = 0;
	int f = -1;
	for (i = 0;i<USER_COUNT;i++)
	{
		if (L_user[i].userid != -1)
			if (strcmp(username, L_user[i].username) == 0)
			{
				f = i;
				break;
			}
	}
	//can't find
	if (f == -1)
	{
		printf(E15);
		return -1;
	}
	//write to file
	WriteToFile(fp);
	system("cls");			//********************不符合实验要求函数！！！*********************************

	for (count = 0; count < TRY_NUM; count++)
	{
		printf("password:");
		i = 0;
		while ((password[i] = getch()) != '\r')
		{
			printf("*");
			i++;
		}
		password[i] = '\0';
		printf("\n");

		if (strcmp(password, L_user[f].password) == 0)
		{
			cur_user.userid = L_user[f].userid;
			strcpy(cur_user.username, username);
			cur_user.group = L_user[f].group;
			cur_user.level = L_user[f].level;
			printf("\nLogin successed\n");
			return 1;
			break;
		}
		else
		{
			printf(E16);
		}
	}
	if (count >= TRY_NUM)
	{
		printf(E17);
		return 0;
	}
	return 0;
}

//change file's mode
void change_mode(char filename[])
{
	int a;     //inode number
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
			//
			if ((strcmp(p->data.file_name, filename) == 0) && (file_inode[p->data.dir_inode].file_style == 1))
			{
				a = p->data.dir_inode;
				int i = 0;
				for (i = 0;i < SYSTEM_ALLOW_OPEN_COUNT;i++)
				{
					if (sys_open_table[i].f_inode == a)
					{
						break;
					}
				}
				if (i == SYSTEM_ALLOW_OPEN_COUNT)
				{
					printf(E6);
					return;
				}
				if (cur_user.userid == file_inode[a].file_userid)
				{
					printf("Please input 0&1 string(1 stands for having, 0 for not)\n");
					printf("Format：r w e r w e r w e\n");
					for (int j = 0;j < PERMISSIONS;j++)
					{
						scanf("%d", &(file_inode[a].file_mode[j]));
					}
					time_t t = time(0);
					strftime(file_inode[a].time, sizeof(file_inode[a].time), "%Y/%m/%d %X %A %jday of current year %z", localtime(&t));
					//puts(file_inode[a].time);
					return;
				}
				else printf(E18);
			}
			p = p->rchild;
		}
		printf(E14);
	}
}

//change file's owner
void change_owner(char filename[])
{
	int a;     //inode number
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
			//
			if ((strcmp(p->data.file_name, filename) == 0) && (file_inode[p->data.dir_inode].file_style == 1))
			{
				a = p->data.dir_inode;
				int i = 0;
				for (i = 0;i < SYSTEM_ALLOW_OPEN_COUNT;i++)
				{
					if (sys_open_table[i].f_inode == a)
					{
						break;
					}
				}
				if (i == SYSTEM_ALLOW_OPEN_COUNT)
				{
					printf(E6);
					return;
				}
				if (cur_user.userid == file_inode[a].file_userid)
				{
					printf("Please input user name:");
					char str[USER_NAME_LENGTH];
					scanf("%s", str);
					for (i = 0; i < USER_COUNT; i++) {
						if (strcmp(L_user[i].username, str) == 0) {
							break;
						}
					}
					if (i == USER_COUNT) {
						printf(E15);
						return;
					}
					file_inode[a].file_userid = L_user[i].userid;
					file_inode[a].file_groupid = L_user[i].group;
					time_t t = time(0);
					strftime(file_inode[a].time, sizeof(file_inode[a].time), "%Y/%m/%d %X %A %jday of current year %z", localtime(&t));
					//puts(file_inode[a].time);
					return;
				}
				else printf(E18);
			}
			p = p->rchild;
		}
		printf(E14);
	}
}

//change file's group
void change_group(char filename[])
{
	int a;     //inode number
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
			//
			if ((strcmp(p->data.file_name, filename) == 0) && (file_inode[p->data.dir_inode].file_style == 1))
			{
				a = p->data.dir_inode;
				int i = 0;
				for (i = 0;i < SYSTEM_ALLOW_OPEN_COUNT;i++)
				{
					if (sys_open_table[i].f_inode == a)
					{
						break;
					}
				}
				if (i == SYSTEM_ALLOW_OPEN_COUNT)
				{
					printf(E6);
					return;
				}
				if (cur_user.userid == file_inode[a].file_userid)
				{
					printf("Please input group id:");
					int gid;
					scanf("%d", &gid);
					if (gid < 0 || gid > 3) {
						printf(E22);
						return;
					}
					file_inode[a].file_groupid = gid;
					time_t t = time(0);
					strftime(file_inode[a].time, sizeof(file_inode[a].time), "%Y/%m/%d %X %A %jday of current year %z", localtime(&t));
					//puts(file_inode[a].time);
					return;
				}
				else printf(E18);
			}
			p = p->rchild;
		}
		printf(E14);
	}
}

//user management
void manage_user()
{
	//rights check
	if (cur_user.level != 1) 
	{
		printf(E19);
		SaveUsers();
		return;
	}
	printf("Welcome to user management!\n");
	int i, j;
	int f;
	char tempuser[2*USER_NAME_LENGTH];
	char temppass[2*USER_PASSWORD_LENGTH];
	int tempgroup;
	int tempuid;
	while (1)
	{
		printf("Select (1.view 2.insert 3.del  0.save&exit):\n");
		scanf("%d", &f);
		if (f == 0)
		{
			SaveUsers();
			break;
		}
		if (f == 1)
		{
			printf("User ID\tName\tGroup\tLevel\n");
			for (i = 0;i<USER_COUNT;i++)
			{
				if (L_user[i].userid != -1)
				{
					printf("%d\t%s\t%d\t%d\n", L_user[i].userid, L_user[i].username, L_user[i].group, L_user[i].level);
				}
			}
		}
		if (f == 2)
		{
			for (j = 0;j<USER_COUNT;j++)
			{
				if (L_user[j].userid == -1)
					break;
			}
			if (j == USER_COUNT)
			{
				printf("Can't insert!\n");
				continue;
			}
			printf("Please input user name:");
			scanf("%s", &tempuser);
			printf("Please input password:");
			j = 0;
			//get password
			while ((temppass[j] = getch()) != '\r') {
				if (temppass[j] == '\b') {
					printf("\b \b");
					temppass[j--] = '\0';
					temppass[j] = '\0';
				}
				else {
					printf("*");
					j++;
				}
			}
			temppass[j] = '\0';
			printf("\n");
			printf("Please input group(1,2,3):");
			scanf("%d", &tempgroup);
			if (tempgroup<1 || tempgroup>3)
			{
				printf("Group error! Input Denied!\n");
				continue;
			}
			L_user[j].userid = j;
			strcpy(L_user[j].username, tempuser);
			strcpy(L_user[j].password, temppass);
			L_user[j].group = tempgroup;
			L_user[j].level = 0;
		}
		if (f == 3)
		{
			printf("Please user Id deleted：");
			scanf("%d", &tempuid);
			if (tempuid<0 || tempuid>USER_COUNT-1)
			{
				printf(E15);
				continue;
			}
			if (tempuid == 0)
			{
				printf("Super administrator can't delete！\n");
				continue;
			}
			L_user[tempuid].userid = -1;
			strcpy(L_user[tempuid].username, "");
			strcpy(L_user[tempuid].password, "");
			L_user[tempuid].group = -1;
		}
	}
}

void str2stack(SqStack &s);
int findtreeinode(SqStack S, FTreepoint T, FTreepoint &p);

//h_link
void h_link(char filename[])
{
	SqStack s;
	FTreepoint p = NULL, p2 = NULL, p4;
	path_tnode(cur_dir, L_Ftree, p);
	int flag = -1;
	if (p->lchild == NULL)
	{
		printf(E20);
		return;
	}
	if (p->lchild->rchild == NULL)
	{
		path_tnode(cur_dir, L_Ftree, p);
		p2 = p->lchild;
		for (int j = 0;j<200;j++)
		{
			if (sys_open_table[j].f_inode == p2->data.dir_inode)
			{
				printf(E13);return;
			}
		}
		if ((strcmp(p2->data.file_name, filename) == 0) && (file_inode[p2->data.dir_inode].file_style == 1))
		{
			str2stack(s);
			findtreeinode(s, L_Ftree, p4);
			if (p4 == NULL) {
				printf(E14);
				return;
			}
			if (p4->lchild == NULL) {
				p4->lchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->lchild;
				p4->lchild = p4->rchild = NULL;
			}
			else {
				p4 = p4->lchild;
				while (p4->rchild != NULL) {
					p4 = p4->rchild;
				}
				p4->rchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->rchild;
				p4->lchild = p4->rchild = NULL;
			}
			p4->data.dir_inode = p2->data.dir_inode;
			strcpy(p4->data.file_name, p2->data.file_name);

			file_inode[p4->data.dir_inode].file_icount++;  //link count add 1

		}
	}
	else
	{
		p2 = p->lchild;
		if ((strcmp(p2->data.file_name, filename) == 0) && (file_inode[p2->data.dir_inode].file_style == 1))
		{
			for (int j = 0;j<200;j++)
			{
				if (sys_open_table[j].f_inode == p2->data.dir_inode)
				{
					printf(E13);return;
				}
			}

		}
		else
		{
			p = p->lchild;
			while ((p != NULL) && (p->rchild != NULL))
			{
				flag = 0;
				if ((strcmp(p->rchild->data.file_name, filename) == 0) && (file_inode[p->rchild->data.dir_inode].file_style == 1))
				{
					p2 = p->rchild;
					flag = 1;
					for (int j = 0;j<200;j++)
					{
						if (sys_open_table[j].f_inode == p2->data.dir_inode)
						{
							printf(E13);return;
						}
					}
					break;
				}
				p = p->rchild;
			}
		}
	}
	if (flag == 0) { printf(E20); return; }

	if (cur_user.userid == file_inode[p2->data.dir_inode].file_userid)
	{
		if (file_inode[p2->data.dir_inode].file_mode[1] == 1)
		{
			str2stack(s);
			findtreeinode(s, L_Ftree, p4);
			if (p4 == NULL) {
				printf(E14);
				return;
			}
			if (p4->lchild == NULL) {
				p4->lchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->lchild;
				p4->lchild = p4->rchild = NULL;
			}
			else {
				p4 = p4->lchild;
				while (p4->rchild != NULL) {
					p4 = p4->rchild;
				}
				p4->rchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->rchild;
				p4->lchild = p4->rchild = NULL;
			}
			p4->data.dir_inode = p2->data.dir_inode;
			strcpy(p4->data.file_name, p2->data.file_name);

			//file_inode[p2->data.dir_inode].inode_number = -1;		//inode number
			//clear_inode_del(p2);

			file_inode[p4->data.dir_inode].file_icount++;  //link count add 1

		}
		else {
			printf(E12);
			return;
		}
	}
	else if (cur_user.group == file_inode[p2->data.dir_inode].file_groupid)
	{
		if (file_inode[p2->data.dir_inode].file_mode[4] == 1)
		{
			str2stack(s);
			findtreeinode(s, L_Ftree, p4);
			if (p4 == NULL) {
				printf(E14);
				return;
			}
			if (p4->lchild == NULL) {
				p4->lchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->lchild;
				p4->lchild = p4->rchild = NULL;
			}
			else {
				p4 = p4->lchild;
				while (p4->rchild != NULL) {
					p4 = p4->rchild;
				}
				p4->rchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->rchild;
				p4->lchild = p4->rchild = NULL;
			}
			p4->data.dir_inode = p2->data.dir_inode;
			strcpy(p4->data.file_name, p2->data.file_name);

			//file_inode[p2->data.dir_inode].inode_number = -1;		//inode number
			//clear_inode_del(p2);

			file_inode[p4->data.dir_inode].file_icount++;  //link count add 1
		}
		else {
			printf(E12);
			return;
		}
	}
	else
	{
		if (file_inode[p2->data.dir_inode].file_mode[7] == 1)
		{
			str2stack(s);
			findtreeinode(s, L_Ftree, p4);
			if (p4 == NULL) {
				printf(E14);
				return;
			}
			if (p4->lchild == NULL) {
				p4->lchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->lchild;
				p4->lchild = p4->rchild = NULL;
			}
			else {
				p4 = p4->lchild;
				while (p4->rchild != NULL) {
					p4 = p4->rchild;
				}
				p4->rchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->rchild;
				p4->lchild = p4->rchild = NULL;
			}
			p4->data.dir_inode = p2->data.dir_inode;
			strcpy(p4->data.file_name, p2->data.file_name);

			//file_inode[p2->data.dir_inode].inode_number = -1;		//inode number
			//clear_inode_del(p2);

			file_inode[p4->data.dir_inode].file_icount++;  //link count add 1
		}
		else {
			printf(E12);
			return;
		}
	}
	if (p2 == NULL) { printf(E20); return; }
	int i = 0;
	clear_dir(file_dir);
	Tree_to_dir(i, file_dir, L_Ftree->lchild);
	FILE *fp1 = new FILE();
	WriteToFile(fp1);
}

//s_link
void s_link(char filename[])
{
	SqStack s;
	FTreepoint p = NULL, p2 = NULL, p3, p4;
	path_tnode(cur_dir, L_Ftree, p);
	int flag = -1;
	if (p->lchild == NULL)
	{
		printf(E20);
		return;
	}
	if (p->lchild->rchild == NULL)
	{
		path_tnode(cur_dir, L_Ftree, p);
		p2 = p->lchild;
		for (int j = 0;j<200;j++)
		{
			if (sys_open_table[j].f_inode == p2->data.dir_inode)
			{
				printf(E13);return;
			}
		}
		if ((strcmp(p2->data.file_name, filename) == 0) && (file_inode[p2->data.dir_inode].file_style == 1))
		{
			str2stack(s);
			findtreeinode(s, L_Ftree, p4);
			if (p4 == NULL) {
				printf(E14);
				return;
			}
			if (p4->lchild == NULL) {
				p4->lchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->lchild;
				p4->lchild = p4->rchild = NULL;
			}
			else {
				p4 = p4->lchild;
				while (p4->rchild != NULL) {
					p4 = p4->rchild;
				}
				p4->rchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->rchild;
				p4->lchild = p4->rchild = NULL;
			}
			int a = find_free_inode();
			p4->data.dir_inode = a;
			strcpy(p4->data.file_name, p2->data.file_name);

			file_inode[a].inode_number = file_inode[p2->data.dir_inode].inode_number;
			file_inode[a].file_style = 1;
			file_inode[a].file_length = file_inode[p2->data.dir_inode].file_length;
			strcpy(file_inode[a].file_mode, file_inode[p2->data.dir_inode].file_mode);
			file_inode[a].file_userid = file_inode[p2->data.dir_inode].file_userid;
			file_inode[a].file_groupid = file_inode[p2->data.dir_inode].file_groupid;
			for (int i = 0; i < DATA_COUNT; i++) {
				file_inode[a].file_address[i] = file_inode[p2->data.dir_inode].file_address[i];
			}
			file_inode[a].file_icount = file_inode[p2->data.dir_inode].file_icount + 1;
			strcpy(file_inode[a].dir_name, file_inode[p2->data.dir_inode].dir_name);
			strcpy(file_inode[a].time, file_inode[p2->data.dir_inode].time);
		}
	}
	else
	{
		p2 = p->lchild;
		if ((strcmp(p2->data.file_name, filename) == 0) && (file_inode[p2->data.dir_inode].file_style == 1))
		{
			for (int j = 0;j<200;j++)
			{
				if (sys_open_table[j].f_inode == p2->data.dir_inode)
				{
					printf(E13);return;
				}
			}

		}
		else
		{
			p = p->lchild;
			while ((p != NULL) && (p->rchild != NULL))
			{
				flag = 0;
				if ((strcmp(p->rchild->data.file_name, filename) == 0) && (file_inode[p->rchild->data.dir_inode].file_style == 1))
				{
					p2 = p->rchild;
					flag = 1;
					for (int j = 0;j<200;j++)
					{
						if (sys_open_table[j].f_inode == p2->data.dir_inode)
						{
							printf(E13);return;
						}
					}
					break;
				}
				p = p->rchild;
			}
		}
	}
	if (flag == 0) { printf(E20); return; }

	if (cur_user.userid == file_inode[p2->data.dir_inode].file_userid)
	{
		if (file_inode[p2->data.dir_inode].file_mode[1] == 1)
		{
			str2stack(s);
			findtreeinode(s, L_Ftree, p4);
			if (p4 == NULL) {
				printf(E14);
				return;
			}
			if (p4->lchild == NULL) {
				p4->lchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->lchild;
				p4->lchild = p4->rchild = NULL;
			}
			else {
				p4 = p4->lchild;
				while (p4->rchild != NULL) {
					p4 = p4->rchild;
				}
				p4->rchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->rchild;
				p4->lchild = p4->rchild = NULL;
			}
			int a = find_free_inode();
			p4->data.dir_inode = a;
			strcpy(p4->data.file_name, p2->data.file_name);

			file_inode[a].inode_number = file_inode[p2->data.dir_inode].inode_number;
			file_inode[a].file_style = file_inode[p2->data.dir_inode].file_style;
			file_inode[a].file_length = file_inode[p2->data.dir_inode].file_length;
			strcpy(file_inode[a].file_mode, file_inode[p2->data.dir_inode].file_mode);
			file_inode[a].file_userid = file_inode[p2->data.dir_inode].file_userid;
			file_inode[a].file_groupid = file_inode[p2->data.dir_inode].file_groupid;
			for (int i = 0; i < DATA_COUNT; i++) {
				file_inode[a].file_address[i] = file_inode[p2->data.dir_inode].file_address[i];
			}
			file_inode[a].file_icount = file_inode[p2->data.dir_inode].file_icount + 1;
			strcpy(file_inode[a].dir_name, file_inode[p2->data.dir_inode].dir_name);
			strcpy(file_inode[a].time, file_inode[p2->data.dir_inode].time);

		}
		else {
			printf(E12);
			return;
		}
	}
	else if (cur_user.group == file_inode[p2->data.dir_inode].file_groupid)
	{
		if (file_inode[p2->data.dir_inode].file_mode[4] == 1)
		{
			str2stack(s);
			findtreeinode(s, L_Ftree, p4);
			if (p4 == NULL) {
				printf(E14);
				return;
			}
			if (p4->lchild == NULL) {
				p4->lchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->lchild;
				p4->lchild = p4->rchild = NULL;
			}
			else {
				p4 = p4->lchild;
				while (p4->rchild != NULL) {
					p4 = p4->rchild;
				}
				p4->rchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->rchild;
				p4->lchild = p4->rchild = NULL;
			}
			int a = find_free_inode();
			p4->data.dir_inode = a;
			strcpy(p4->data.file_name, p2->data.file_name);

			file_inode[a].inode_number = file_inode[p2->data.dir_inode].inode_number;
			file_inode[a].file_style = 1;
			file_inode[a].file_length = file_inode[p2->data.dir_inode].file_length;
			strcpy(file_inode[a].file_mode, file_inode[p2->data.dir_inode].file_mode);
			file_inode[a].file_userid = file_inode[p2->data.dir_inode].file_userid;
			file_inode[a].file_groupid = file_inode[p2->data.dir_inode].file_groupid;
			for (int i = 0; i < DATA_COUNT; i++) {
				file_inode[a].file_address[i] = file_inode[p2->data.dir_inode].file_address[i];
			}
			file_inode[a].file_icount = file_inode[p2->data.dir_inode].file_icount + 1;
			strcpy(file_inode[a].dir_name, file_inode[p2->data.dir_inode].dir_name);
			strcpy(file_inode[a].time, file_inode[p2->data.dir_inode].time);
		}
		else {
			printf(E12);
			return;
		}
	}
	else
	{
		if (file_inode[p2->data.dir_inode].file_mode[7] == 1)
		{
			str2stack(s);
			findtreeinode(s, L_Ftree, p4);
			if (p4 == NULL) {
				printf(E14);
				return;
			}
			if (p4->lchild == NULL) {
				p4->lchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->lchild;
				p4->lchild = p4->rchild = NULL;
			}
			else {
				p4 = p4->lchild;
				while (p4->rchild != NULL) {
					p4 = p4->rchild;
				}
				p4->rchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->rchild;
				p4->lchild = p4->rchild = NULL;
			}
			int a = find_free_inode();
			p4->data.dir_inode = a;
			strcpy(p4->data.file_name, p2->data.file_name);

			file_inode[a].inode_number = file_inode[p2->data.dir_inode].inode_number;
			file_inode[a].file_style = 1;
			file_inode[a].file_length = file_inode[p2->data.dir_inode].file_length;
			strcpy(file_inode[a].file_mode, file_inode[p2->data.dir_inode].file_mode);
			file_inode[a].file_userid = file_inode[p2->data.dir_inode].file_userid;
			file_inode[a].file_groupid = file_inode[p2->data.dir_inode].file_groupid;
			for (int i = 0; i < DATA_COUNT; i++) {
				file_inode[a].file_address[i] = file_inode[p2->data.dir_inode].file_address[i];
			}
			file_inode[a].file_icount = file_inode[p2->data.dir_inode].file_icount + 1;
			strcpy(file_inode[a].dir_name, file_inode[p2->data.dir_inode].dir_name);
			strcpy(file_inode[a].time, file_inode[p2->data.dir_inode].time);
		}
		else {
			printf(E12);
			return;
		}
	}
	if (p2 == NULL) { printf(E20); return; }
	int i = 0;
	clear_dir(file_dir);
	Tree_to_dir(i, file_dir, L_Ftree->lchild);
	FILE *fp1 = new FILE();
	WriteToFile(fp1);
}

//do file in array
void do_file(char buff[])
{
	SqStack s;
	InitStack(s);

	char ch, c1[10];
	int num = 0, num1 = 0;
	while (buff[num] != '#')
	{
		if (buff[num] != '/')
		{
			c1[num1] = buff[num];
			num1++;
		}
		else
		{
			c1[num1] = 0;
			push(s, c1);
			num1 = 0;
		}
		num++;
	}
	FTreepoint p;
	findtreeinode(s, L_Ftree, p);
	r_f(p->data.dir_inode);
}

//rename
void rename(char filename[])
{

	char newfname[FILE_NAME_LENGTH];
	FTreepoint p = NULL, p2 = NULL, p3;
	path_tnode(cur_dir, L_Ftree, p);
	if (p->lchild == NULL)
	{
		printf("File not existed\n");
	}
	else
	{
		p2 = p->lchild;
		while (p2 != NULL)
		{
			if ((strcmp(p2->data.file_name, filename) == 0) && (file_inode[p2->data.dir_inode].file_style == 1))
			{
				printf("Please input a new file name:");
				scanf("%s", newfname);
				if (strcmp(newfname, filename) == 0) { printf("Please do not input the same name^-^!\n");return; }
				p3 = p->lchild;
				while (p3 != NULL)
				{
					if ((strcmp(p3->data.file_name, newfname) == 0) && (file_inode[p3->data.dir_inode].file_style == 1))
					{
						printf("This name has already used^-^!\n");return;
					}
					p3 = p3->rchild;
				}
				strcpy(p2->data.file_name, newfname);
				break;
			}
			p2 = p2->rchild;
		}
	}
	int i = 0;
	clear_dir(file_dir);
	Tree_to_dir(i, file_dir, L_Ftree->lchild);
	FILE *fp = new FILE();
	WriteToFile(fp);
}

void change_pwd() {
	printf("Please input old password:");
	char password[2 * USER_PASSWORD_LENGTH];
	int i = 0;
	//get password
	while ((password[i] = getch()) != '\r') {
		if (password[i] == '\b') {
			printf("\b \b");
			password[i--] = '\0';
			password[i] = '\0';
		}
		else {
			printf("*");
			i++;
		}
	}
	password[i] = '\0';
	printf("\n");
	for (i = 0;i<USER_COUNT;i++) {
		//comfirm the username & password
			if (L_user[i].userid == cur_user.userid) {
				if (strcmp(password, L_user[i].password) == 0) {
					printf("Please input new password:");
					int j = 0;
					//get password
					while ((password[j] = getch()) != '\r') {
						if (password[j] == '\b') {
							printf("\b \b");
							password[j--] = '\0';
							password[j] = '\0';
						}
						else {
							printf("*");
							j++;
						}
					}
					password[j] = '\0';
					printf("\n");
					strcpy(L_user[i].password, password);
					SaveUsers();
					return;
				}
			}
	}
	//username or password error
	if (i = USER_COUNT)
		printf(E23);
	return;
}

void c_f(int a, char* buffer) {
	int b = file_inode[a].file_length, i, cnt = 0;
	if (b < DATA_COUNT - 2) {
		for (i = 0;i < b;i++) {
			for (int j = 0;j < 10;j++) {
				if (phy[file_inode[a].file_address[i]].p[j] == 0) return;
				buffer[cnt++] = phy[file_inode[a].file_address[i]].p[j];
			}
		}
	}
	else {
		for (int i = 0;i < DATA_COUNT - 3;i++) {
			for (int j = 0;j < 10;j++) {
				if (phy[file_inode[a].file_address[i]].p[j] == 0) return;
				buffer[cnt++] = phy[file_inode[a].file_address[i]].p[j];
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
				if (phy[f1[i]].p[j] == 0) return;
				buffer[cnt++] = phy[f1[i]].p[j];
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
				if (phy[f2[i]].p[j] == 0) return;
				buffer[cnt++] = phy[f2[i]].p[j];
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
				if (phy[f3[i]].p[j] == 0) return;
				buffer[cnt++] = phy[f3[i]].p[j];
			}
		}
	}
}

void wc_f(int a, char* buffer) {  //a:inode number
	free_disk(a);   //clear
	char ch;
	int num = 0;
	while (buffer[num] != '\0') num++;

	file_inode[a].file_length = 0;
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

void copy_file(char filename[]) {
	char buffer[FILE_BUFFER] = { 0 };
	SqStack s;
	FTreepoint p = NULL, p2 = NULL, p4;
	path_tnode(cur_dir, L_Ftree, p);
	int flag = -1;
	if (p->lchild == NULL)
	{
		printf(E20);
		return;
	}
	if (p->lchild->rchild == NULL)
	{
		path_tnode(cur_dir, L_Ftree, p);
		p2 = p->lchild;
		for (int j = 0;j<200;j++)
		{
			if (sys_open_table[j].f_inode == p2->data.dir_inode)
			{
				printf(E13);return;
			}
		}
		if ((strcmp(p2->data.file_name, filename) == 0) && (file_inode[p2->data.dir_inode].file_style == 1))
		{
			c_f(p2->data.dir_inode, buffer);
			str2stack(s);
			findtreeinode(s, L_Ftree, p4);
			if (p4 == NULL) {
				printf(E14);
				return;
			}
			if (p4->lchild == NULL) {
				p4->lchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->lchild;
				p4->lchild = p4->rchild = NULL;
			}
			else {
				p4 = p4->lchild;
				while (p4->rchild != NULL) {
					p4 = p4->rchild;
				}
				p4->rchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->rchild;
				p4->lchild = p4->rchild = NULL;
			}
			int a = find_free_inode();
			p4->data.dir_inode = a;
			strcpy(p4->data.file_name, p2->data.file_name);

			file_inode[a].inode_number = file_inode[p2->data.dir_inode].inode_number;
			file_inode[a].file_style = 1;
			file_inode[a].file_length = file_inode[p2->data.dir_inode].file_length;
			strcpy(file_inode[a].file_mode, file_inode[p2->data.dir_inode].file_mode);
			file_inode[a].file_userid = file_inode[p2->data.dir_inode].file_userid;
			file_inode[a].file_groupid = file_inode[p2->data.dir_inode].file_groupid;
			file_inode[a].file_icount = file_inode[p2->data.dir_inode].file_icount + 1;
			strcpy(file_inode[a].dir_name, file_inode[p2->data.dir_inode].dir_name);
			time_t t = time(0);
			strftime(file_inode[a].time, sizeof(file_inode[a].time), "%Y/%m/%d %X %A %jday of the current year %z", localtime(&t));
			wc_f(a, buffer);
		}
	}
	else
	{
		p2 = p->lchild;
		if ((strcmp(p2->data.file_name, filename) == 0) && (file_inode[p2->data.dir_inode].file_style == 1))
		{
			for (int j = 0;j<200;j++)
			{
				if (sys_open_table[j].f_inode == p2->data.dir_inode)
				{
					printf(E13);return;
				}
			}

		}
		else
		{
			p = p->lchild;
			while ((p != NULL) && (p->rchild != NULL))
			{
				flag = 0;
				if ((strcmp(p->rchild->data.file_name, filename) == 0) && (file_inode[p->rchild->data.dir_inode].file_style == 1))
				{
					p2 = p->rchild;
					flag = 1;
					for (int j = 0;j<200;j++)
					{
						if (sys_open_table[j].f_inode == p2->data.dir_inode)
						{
							printf(E13);return;
						}
					}
					break;
				}
				p = p->rchild;
			}
		}
	}
	if (flag == 0) { printf(E20); return; }

	if (cur_user.userid == file_inode[p2->data.dir_inode].file_userid)
	{
		if (file_inode[p2->data.dir_inode].file_mode[1] == 1)
		{
			c_f(p2->data.dir_inode, buffer);
			str2stack(s);
			findtreeinode(s, L_Ftree, p4);
			if (p4 == NULL) {
				printf(E14);
				return;
			}
			if (p4->lchild == NULL) {
				p4->lchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->lchild;
				p4->lchild = p4->rchild = NULL;
			}
			else {
				p4 = p4->lchild;
				while (p4->rchild != NULL) {
					p4 = p4->rchild;
				}
				p4->rchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->rchild;
				p4->lchild = p4->rchild = NULL;
			}
			int a = find_free_inode();
			p4->data.dir_inode = a;
			strcpy(p4->data.file_name, p2->data.file_name);

			file_inode[a].inode_number = file_inode[p2->data.dir_inode].inode_number;
			file_inode[a].file_style = file_inode[p2->data.dir_inode].file_style;
			file_inode[a].file_length = file_inode[p2->data.dir_inode].file_length;
			strcpy(file_inode[a].file_mode, file_inode[p2->data.dir_inode].file_mode);
			file_inode[a].file_userid = file_inode[p2->data.dir_inode].file_userid;
			file_inode[a].file_groupid = file_inode[p2->data.dir_inode].file_groupid;
			file_inode[a].file_icount = file_inode[p2->data.dir_inode].file_icount + 1;
			strcpy(file_inode[a].dir_name, file_inode[p2->data.dir_inode].dir_name);
			time_t t = time(0);
			strftime(file_inode[a].time, sizeof(file_inode[a].time), "%Y/%m/%d %X %A %jday of the current year %z", localtime(&t));
			wc_f(a, buffer);
		}
		else {
			printf(E12);
			return;
		}
	}
	else if (cur_user.group == file_inode[p2->data.dir_inode].file_groupid)
	{
		if (file_inode[p2->data.dir_inode].file_mode[4] == 1)
		{
			c_f(p2->data.dir_inode, buffer);
			str2stack(s);
			findtreeinode(s, L_Ftree, p4);
			if (p4 == NULL) {
				printf(E14);
				return;
			}
			if (p4->lchild == NULL) {
				p4->lchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->lchild;
				p4->lchild = p4->rchild = NULL;
			}
			else {
				p4 = p4->lchild;
				while (p4->rchild != NULL) {
					p4 = p4->rchild;
				}
				p4->rchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->rchild;
				p4->lchild = p4->rchild = NULL;
			}
			int a = find_free_inode();
			p4->data.dir_inode = a;
			strcpy(p4->data.file_name, p2->data.file_name);

			file_inode[a].inode_number = file_inode[p2->data.dir_inode].inode_number;
			file_inode[a].file_style = 1;
			file_inode[a].file_length = file_inode[p2->data.dir_inode].file_length;
			strcpy(file_inode[a].file_mode, file_inode[p2->data.dir_inode].file_mode);
			file_inode[a].file_userid = file_inode[p2->data.dir_inode].file_userid;
			file_inode[a].file_groupid = file_inode[p2->data.dir_inode].file_groupid;
			file_inode[a].file_icount = file_inode[p2->data.dir_inode].file_icount + 1;
			strcpy(file_inode[a].dir_name, file_inode[p2->data.dir_inode].dir_name);
			time_t t = time(0);
			strftime(file_inode[a].time, sizeof(file_inode[a].time), "%Y/%m/%d %X %A %jday of the current year %z", localtime(&t));
			wc_f(a, buffer);
		}
		else {
			printf(E12);
			return;
		}
	}
	else
	{
		if (file_inode[p2->data.dir_inode].file_mode[7] == 1)
		{
			c_f(p2->data.dir_inode, buffer);
			str2stack(s);
			findtreeinode(s, L_Ftree, p4);
			if (p4 == NULL) {
				printf(E14);
				return;
			}
			if (p4->lchild == NULL) {
				p4->lchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->lchild;
				p4->lchild = p4->rchild = NULL;
			}
			else {
				p4 = p4->lchild;
				while (p4->rchild != NULL) {
					p4 = p4->rchild;
				}
				p4->rchild = (FTreepoint)malloc(sizeof(FTree));
				p4 = p4->rchild;
				p4->lchild = p4->rchild = NULL;
			}
			int a = find_free_inode();
			p4->data.dir_inode = a;
			strcpy(p4->data.file_name, p2->data.file_name);

			file_inode[a].inode_number = file_inode[p2->data.dir_inode].inode_number;
			file_inode[a].file_style = 1;
			file_inode[a].file_length = file_inode[p2->data.dir_inode].file_length;
			strcpy(file_inode[a].file_mode, file_inode[p2->data.dir_inode].file_mode);
			file_inode[a].file_userid = file_inode[p2->data.dir_inode].file_userid;
			file_inode[a].file_groupid = file_inode[p2->data.dir_inode].file_groupid;
			file_inode[a].file_icount = file_inode[p2->data.dir_inode].file_icount + 1;
			strcpy(file_inode[a].dir_name, file_inode[p2->data.dir_inode].dir_name);
			time_t t = time(0);
			strftime(file_inode[a].time, sizeof(file_inode[a].time), "%Y/%m/%d %X %A %jday of the current year %z", localtime(&t));
			wc_f(a, buffer);
		}
		else {
			printf(E12);
			return;
		}
	}
	if (p2 == NULL) { printf(E20); return; }
	int i = 0;
	clear_dir(file_dir);
	Tree_to_dir(i, file_dir, L_Ftree->lchild);
	FILE *fp1 = new FILE();
	WriteToFile(fp1);
}