/*
* Copyright (c) 2016, Randolph Han, Zhejiang University of Technology, China
* All rights reserved.
* Project name：Simple File System
* Programmer：Randolph Han
* Finish：2016.12.11-2016.12.??
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

extern char buffer[13312];					//file content buffer

extern  UserOpenTable user_open_table[10];	//user open table
extern SystemOpenTable sys_open_table[200];	//system open table
extern ActiveNode active_inode_table;		//active inode table

extern FTreepoint L_Ftree;                 //file tree

//File operating
void ReadFromFile(FILE *fp);	//read file
void WriteToFile(FILE *fp);		//write file
void ReadUsers(FILE *fps);		//get users
void SaveUsers(FILE *fps);		//save users

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
void show_info();				//show system information
void logout(FILE *fp);			//logout
int change_user(FILE *fp, char tmp[]);	//change user
void change_mode(char tmp[]);	//chang file mode
void manage_user();				//user management
void h_link(char tmp[]);		//hard link
void s_link(char tmp[]);		//soft link
void rename(char tmp[]);		//rename a file
void relogin();					//multiple users

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

void help(){
	printf("System command:\n");
	printf("\t01.退出系统.......................................(exit)\n");
	printf("\t02.显示帮助命令...................................(help)\n");
	printf("\t03.查看当前目录文件列表.............................(ls)\n");
	printf("\t04.进入当前目录下的指定目录................(cd + 目录名)\n");
	printf("\t05.返回上一级目录.................................(cd..)\n");
	printf("\t06.创建目录.............................(mkdir + 目录名)\n");
	printf("\t07.删除目录.............................(rmdir + 目录名)\n");
	printf("\t08.创建文件............................(create + 文件名)\n");
	printf("\t09.打开文件..............................(open + 文件名)\n");
	printf("\t10.文件的读..............................(read + 文件名)\n");
	printf("\t11.文件的写.............................(write + 文件名)\n");
	printf("\t12.关闭文件.............................(close + 文件名)\n");
	printf("\t13.删除文件................................(rm + 文件名)\n");
	printf("\t14.查看系统信息...................................(info)\n");
	printf("\t15.注销（重启）.................................(logout)\n");
	printf("\t16.切换用户................................(su + 用户名)\n");
	printf("\t17.更改文件访问权限.....................(chmod + 文件名)\n");
	printf("\t18.管理用户(显示管理菜单)........................(Muser)\n");
	printf("\t19.硬链接...............................(hlink + 文件名)\n");
	printf("\t20.软链接...............................(slink + 文件名)\n");
	printf("\t21.重命名..................................(mv + 文件名)\n");
}


//login
int login(){
	char username[20];
	char password[20];
	int count;
	int i = 0;
	//Only have 5 times to access the system
	for (count = 0; count < 5; count++)
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
		for (i = 0;i<10;i++){
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
		if (i = 14)
			printf("\nUsername or password error!\n");
	}
	//exit system
	if (count >= 3){
		printf("\nYou can't try any more, system will be shutdown...\n");
		return 0;
	}
	return 0;
}

//show current directory
void show_curdir(){
	int i;
	//formta
	printf("\t\tname\ttype\tlength\tpermission\t  user\tgroup\n");
	FTreepoint p = NULL;
	path_tnode(cur_dir, L_Ftree, p);
	p = p->lchild;
	while (p != NULL){
		if (file_inode[p->data.dir_inode].file_style == 1){
			printf("\t\t%s ", p->data.file_name);
			printf("\t%d\t%d\t", file_inode[p->data.dir_inode].file_style, file_inode[p->data.dir_inode].file_length);
			for (i = 0;i<9;i++){
				printf("%c", file_inode[p->data.dir_inode].file_mode[i] + 48);
			}
			printf("\t%d\t%d\n", file_inode[p->data.dir_inode].file_userid, file_inode[p->data.dir_inode].file_groupid);

		}
		if (file_inode[p->data.dir_inode].file_style == 0){
			printf("\t\t%s ", p->data.file_name);
			printf("\t%d\t%d\t", file_inode[p->data.dir_inode].file_style, file_inode[p->data.dir_inode].file_length);
			for (i = 0;i<9;i++){
				printf("%c", file_inode[p->data.dir_inode].file_mode[i] + 48);
			}
			printf("\n");
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
	printf("The directory doesn't exist\n");
}


//back to last directory
void back_dir(){
	char c[10];
	Gettop(cur_dir, c);
	if (strcmp(c, "root") == 0){
		printf("Already at root dir！\n");
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
				printf("Already exist a same name dir！Create failed！\n");
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
		char c[10];Gettop(cur_dir, c);
		strcpy(file_inode[a].dir_name, c);           //file dir
		file_inode[a].file_style = 0;
		file_inode[a].file_icount = 0;
		file_inode[a].file_length = 0;
		file_inode[a].inode_number = a;
		for (int i = 0;i<9;i++){           //default permission
			file_inode[a].file_mode[i] = 1;
		}
		file_inode[a].file_groupid = cur_user.group;
		file_inode[a].file_userid = cur_user.userid;
	}
	int i = 0;
	clear_dir(file_dir);
	Tree_to_dir(i, file_dir, L_Ftree->lchild);
	FILE *fp;
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
		printf("Input dirname error! Please check out!\n");
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
	if (flag == 0) { printf("Input dirname error! Please check out!\n"); return; }

	if (cur_user.userid == file_inode[p2->data.dir_inode].file_userid){     //user
		if (file_inode[p2->data.dir_inode].file_mode[1] == 1){
			file_inode[p2->data.dir_inode].inode_number = -1;
			clear_inode_del(p2->lchild);
		}
		else printf("Access Denied! (chmod + filename)\n");

	}
	else if (cur_user.group == file_inode[p2->data.dir_inode].file_groupid){   //group
	
		if (file_inode[p2->data.dir_inode].file_mode[4] == 1)
		{
			file_inode[p2->data.dir_inode].inode_number = -1;
			clear_inode_del(p2->lchild);
		}
		else printf("Access Denied! (chmod + filename)\n");

	}
	else{																		//others
		if (file_inode[p2->data.dir_inode].file_mode[7] == 1){
			file_inode[p2->data.dir_inode].inode_number = -1;
			clear_inode_del(p2->lchild);
		}
		else printf("Access Denied! (chmod + filename)\n");
	}
	if (p2 == NULL) { printf("Input dirname error! Please check out!\n"); return; }
	int i = 0;
	clear_dir(file_dir);
	Tree_to_dir(i, file_dir, L_Ftree->lchild);
	FILE *fp1;
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
				printf("Already exist a same name file！Create failed！\n");
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
		char c[10];Gettop(cur_dir, c);
		strcpy(file_inode[a].dir_name, c);           //file dir
		file_inode[a].file_style = 1;
		file_inode[a].file_icount = 0;
		file_inode[a].file_length = 0;
		file_inode[a].inode_number = a;
		for (int i = 0;i < 9;i++) {						//default permission
			file_inode[a].file_mode[i] = 1;
		}
		file_inode[a].file_groupid = cur_user.group;
		file_inode[a].file_userid = cur_user.userid;
	}
	int i = 0;
	clear_dir(file_dir);
	Tree_to_dir(i, file_dir, L_Ftree->lchild);
	FILE *fp;
	WriteToFile(fp);
}

//free file/dir
void free_disk(int a)
{
	//direct addressing
	int i;
	for (i = 0;i<12;i++){
		if (file_inode[a].file_address[i] == -1) break;
		else{
			hx_superblock.phydata[file_inode[a].file_address[i]] = 0;
		}
	}
	//first level addressing
	if (file_inode[a].file_address[12] != -1){
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
	if (file_inode[a].file_address[13] != -1){
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
	if (file_inode[a].file_address[14] != -1){
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
		printf("Input filename error! Please check out!\n");
		return;
	}
	if (p->lchild->rchild == NULL){   //one file only
		path_tnode(cur_dir, L_Ftree, p);
		p2 = p->lchild;
		//check wether if opened
		for (int j = 0;j<200;j++){
			if (sys_open_table[j].f_inode == p2->data.dir_inode){
				printf("The file has already opened, please close it before delete！\n");return;
			}
		}
		if ((strcmp(p2->data.file_name, filename) == 0) && (file_inode[p2->data.dir_inode].file_style == 1)){  //delete the first file
		
			if (file_inode[p2->data.dir_inode].file_length>0)
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
					printf("The file has already opened, please close it before delete!\n");return;
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
					for (int j = 0;j<200;j++){
						if (sys_open_table[j].f_inode == p2->data.dir_inode){
							printf("The file has already opened, please close it before delete!\n");return;
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
	if (flag == 0) { printf("Input filename error! Please check out!\n"); return; }

	if (cur_user.userid == file_inode[p2->data.dir_inode].file_userid) {
		if (file_inode[p2->data.dir_inode].file_mode[1] == 1){
			file_inode[p2->data.dir_inode].inode_number = -1;
			clear_inode_del(p2->lchild);
		}
		else printf("Access Denied! (chmod + filename)\n");

	}
	else if (cur_user.group == file_inode[p2->data.dir_inode].file_groupid){
		if (file_inode[p2->data.dir_inode].file_mode[4] == 1)
		{
			file_inode[p2->data.dir_inode].inode_number = -1;
			clear_inode_del(p2->lchild);
		}
		else printf("Access Denied! (chmod + filename)\n");

	}
	else{
		if (file_inode[p2->data.dir_inode].file_mode[7] == 1){
			file_inode[p2->data.dir_inode].inode_number = -1;
			clear_inode_del(p2->lchild);
		}
		else printf("Access Denied! (chmod + filename)\n");

	}
	if (p2 == NULL) { printf("Input filename error! Please check out!\n"); return; }
	int i = 0;
	clear_dir(file_dir);
	Tree_to_dir(i, file_dir, L_Ftree->lchild);
	FILE *fp1;
	WriteToFile(fp1);
}

//open file
void open_file(char filename[]){
	int a;     //a代表代开文件的inode号
	FTreepoint p = NULL, p2 = NULL;
	path_tnode(cur_dir, L_Ftree, p);
	if (p->lchild == NULL){
		printf("File not exist！\n");
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
					else printf("Access Denied! (chmod + filename)\n");
				}
				else if (cur_user.group == file_inode[a].file_groupid){
					if (file_inode[a].file_mode[5] == 1){
						InsertUserTable(a, cur_user.userid);
					}
					else printf("Access Denied! (chmod + filename)\n");
				}
				else{
					if (file_inode[a].file_mode[8] == 1){
						InsertUserTable(a, cur_user.userid);
					}
					else printf("Access Denied! (chmod + filename)\n");
				}
				return;
			}
			p = p->rchild;
		}
		printf("File not exist!\n");
	}
}

//close file
void close_file(char filename[]){
	int a;     //a代表代开文件的inode号
	FTreepoint p = NULL, p2 = NULL;
	path_tnode(cur_dir, L_Ftree, p);
	if (p->lchild == NULL){
		printf("File not exist!\n");
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
					else printf("Access Denied! (chmod + filename)\n");
				}
				else if (cur_user.group == file_inode[a].file_groupid){
					if (file_inode[a].file_mode[5] == 1){
						DelUserTable(a, cur_user.userid);
					}
					else printf("Access Denied! (chmod + filename)\n");
				}
				else{
					if (file_inode[a].file_mode[8] == 1){
						DelUserTable(a, cur_user.userid);
					}
					else printf("Access Denied! (chmod + filename)\n");
				}

				return;
			}
			p = p->rchild;
		}
		printf("File not exist!\n");
	}
}

void r_f(int a){
	printf("File content：\n");
	int b = file_inode[a].file_length, i;
	if (b < 13){
		for (i = 0;i < b;i++){
			for (int j = 0;j < 10;j++){
				if (phy[file_inode[a].file_address[i]].p[j] == 0) return;
				printf("%c", phy[file_inode[a].file_address[i]].p[j]);
			}
		}
	}
	else{
		for (int i = 0;i < 12;i++){
			for (int j = 0;j < 10;j++){
				if (phy[file_inode[a].file_address[i]].p[j] == 0) return;
				printf("%c", phy[file_inode[a].file_address[i]].p[j]);
			}
		}
	}
	if (b >= 13){
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
	if (b >= 14){
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
	if (b >= 15){
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
		printf("File not exist!\n");
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
					else printf("Access Denied! (chmod + filename)\n");
				}
				else if (cur_user.group == file_inode[a].file_groupid)   //同组
				{
					if (file_inode[a].file_mode[3] == 1)
					{
						r_f(a);printf("\n");
					}
					else printf("Access Denied! (chmod + filename)\n");
				}
				else
				{
					if (file_inode[a].file_mode[6] == 1)
					{
						r_f(a);printf("\n");
					}
					else printf("Access Denied! (chmod + filename)\n");
				}

				return;
			}
			p = p->rchild;
		}
		printf("File not exist!\n");
	}
}

int find_super(){
	for (int i = 0;i<20500;i++){
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
	printf("Please input the content，end with '$'！(单个文件不能超过13312个字符即：(12+2+4+8)*512=13312）（按照一个磁盘块512字符来算）)\n");
	//buffer
	while ((ch = getchar()) != '$'){
		buffer[num] = ch;
		num++;
	}
	buffer[num] = 0;
	printf("Total: %d！\n", num);

	file_inode[a].file_length = 0;
	if (num>13312){
		printf("The input content out of the size!\n");
		return;
	}
	if (num <= 120 && num>0){
		if (num % 10 == 0){
			for (int i = 0;i<num / 10;i++){
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
	if (num>12*512){   //简化处理，直接开辟两个存储空间
		file_inode[a].file_address[12] = find_super();
		file_inode[a].file_length++;
		hx_superblock.phydata[find_super()] = 1;
		int first = find_super();
		hx_superblock.phydata[find_super()] = 1;
		int second = find_super();
		hx_superblock.phydata[find_super()] = 1;
		char buf[256] = { 0 }, buf1[256] = { 0 };
		itoa(first, buf, 512);
		itoa(second, buf1, 512);
		int i;
		for (i = 0;i<512;i++){
			phy[file_inode[a].file_address[12]].p[i] = buf[i];
			phy[file_inode[a].file_address[12]].p[i + 256] = buf1[i];
		}
		//开始存储内容
		for (i = 12*512;i<13*512;i++){
			if (i>num){
				break;
			}
			phy[first].p[i - 120] = buffer[i];
		}
		for (i = 13*512;i<14*512;i++){
			if (i>num){
				break;
			}
			phy[second].p[i - 13*512] = buffer[i];
		}
	}
	if (num>14*512){ 
		file_inode[a].file_address[13] = find_super();
		file_inode[a].file_length++;
		hx_superblock.phydata[find_super()] = 1;
		int first = find_super();
		hx_superblock.phydata[find_super()] = 1;
		int second = find_super();
		hx_superblock.phydata[find_super()] = 1;
		char buf[256] = { 0 }, buf1[256] = { 0 };
		itoa(first, buf, 512);
		itoa(second, buf1, 512);
		//一次间接寻址
		for (int i = 0;i<512;i++){
			phy[file_inode[a].file_address[13]].p[i] = buf[i];
			phy[file_inode[a].file_address[13]].p[i + 256] = buf1[i];
		}

		int f1, f2, f3, f4, i;
		f1 = find_super();hx_superblock.phydata[f1] = 1;
		f2 = find_super();hx_superblock.phydata[f2] = 1;
		f3 = find_super();hx_superblock.phydata[f3] = 1;
		f4 = find_super();hx_superblock.phydata[f4] = 1;
		char b1[256] = { 0 }, b2[256] = { 0 }, b3[256] = { 0 }, b4[256] = { 0 };
		itoa(f1, b1, 512);itoa(f2, b2, 512);itoa(f3, b3, 512);itoa(f4, b4, 512);
		//二次间接寻址
		for (i = 0;i<256;i++)
		{
			phy[first].p[i] = b1[i];
			phy[first].p[i + 256] = b2[i];
			phy[second].p[i] = b3[i];
			phy[second].p[i + 256] = b4[i];
		}
		//开始存储内容
		for (i = 14*512;i<15*512;i++){
			if (i>num){
				break;
			}
			phy[f1].p[i - 14*512] = buffer[i];
		}
		for (i = 15*512;i<16*512;i++){
			if (i>num){
				break;
			}
			phy[f2].p[i - 15*512] = buffer[i];
		}
		for (i = 16*512;i<17*512;i++){
			if (i>num){
				break;
			}
			phy[f3].p[i - 16*512] = buffer[i];
		}
		for (i = 17*512;i<18*512;i++){
			if (i>num){
				break;
			}
			phy[f4].p[i - 17*512] = buffer[i];
		}
	}
	if (num>18*512){
		file_inode[a].file_address[14] = find_super();
		file_inode[a].file_length++;
		hx_superblock.phydata[find_super()] = 1;
		int first = find_super();
		hx_superblock.phydata[find_super()] = 1;
		int second = find_super();
		hx_superblock.phydata[find_super()] = 1;
		char buf[256] = { 0 }, buf1[256] = { 0 };
		itoa(first, buf, 512);
		itoa(second, buf1, 512);
		for (int i = 0;i<256;i++){
			phy[file_inode[a].file_address[14]].p[i] = buf[i];
			phy[file_inode[a].file_address[14]].p[i + 256] = buf1[i];
		}

		int f1, f2, f3, f4, i;
		f1 = find_super();hx_superblock.phydata[f1] = 1;
		f2 = find_super();hx_superblock.phydata[f2] = 1;
		f3 = find_super();hx_superblock.phydata[f3] = 1;
		f4 = find_super();hx_superblock.phydata[f4] = 1;
		char b1[256] = { 0 }, b2[256] = { 0 }, b3[256] = { 0 }, b4[256] = { 0 };
		itoa(f1, b1, 512);itoa(f2, b2, 512);itoa(f3, b3, 512);itoa(f4, b4, 512);

		for (i = 0;i<256;i++)
		{
			phy[first].p[i] = b1[i];
			phy[first].p[i + 256] = b2[i];
			phy[second].p[i] = b3[i];
			phy[second].p[i + 256] = b4[i];
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
		char bb1[256] = { 0 }, bb2[256] = { 0 }, bb3[256] = { 0 }, bb4[256] = { 0 }, bb5[256] = { 0 }, bb6[256] = { 0 }, bb7[256] = { 0 }, bb8[256] = { 0 };
		itoa(ff1, bb1, 512);itoa(ff2, bb2, 512);itoa(ff3, bb3, 512);itoa(ff4, bb4, 512);
		itoa(ff5, bb5, 512);itoa(ff6, bb6, 512);itoa(ff7, bb7, 512);itoa(ff8, bb8, 512);
		for (i = 0;i<256;i++){
			phy[f1].p[i] = bb1[i];
			phy[f1].p[i + 256] = bb2[i];
			phy[f2].p[i] = bb3[i];
			phy[f2].p[i + 256] = bb4[i];
			phy[f3].p[i] = bb5[i];
			phy[f3].p[i + 256] = bb6[i];
			phy[f4].p[i] = bb7[i];
			phy[f4].p[i + 256] = bb8[i];
		}

		for (i = 18*512;i<19*512;i++){
			if (i>num){
				break;
			}
			phy[ff1].p[i - 18*512] = buffer[i];
		}
		for (i = 19*512;i<20*512;i++){
			if (i>num){
				break;
			}
			phy[ff2].p[i - 19*512] = buffer[i];
		}
		for (i = 20*512;i<21*512;i++){
			if (i>num){
				break;
			}
			phy[ff3].p[i - 20*512] = buffer[i];
		}
		for (i = 21*512;i<22*512;i++){
			if (i>num){
				break;
			}
			phy[ff4].p[i - 21*512] = buffer[i];
		}
		for (i = 22*512;i<23*512;i++){
			if (i>num){
				break;
			}
			phy[ff5].p[i - 22*512] = buffer[i];
		}
		for (i = 23*512;i<24*512;i++){
			if (i>num){
				break;
			}
			phy[ff6].p[i - 23*512] = buffer[i];
		}
		for (i = 24*512;i<25*512;i++){
			if (i>num){
				break;
			}
			phy[ff7].p[i - 24*512] = buffer[i];
		}
		for (i = 25*512;i<26*512;i++){
			if (i>num){
				break;
			}
			phy[ff8].p[i - 25*512] = buffer[i];
		}
	}
}

//写文件（单个文件不能超过2600个字符即：(12+2+4+8)*512=2600）（按照一个磁盘块10字符来算）一旦写不论写满与否均认为写满
void write_file(char filename[])
{
	int a;     //a代表代开文件的inode号
	FTreepoint p = NULL, p2 = NULL;
	path_tnode(cur_dir, L_Ftree, p);
	if (p->lchild == NULL)      //为空
	{
		printf("不存在该文件！\n");
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
					else printf("您不具备权限，请联系管理员修改权限！（chmod + 文件名）\n");
				}
				else if (cur_user.group == file_inode[a].file_groupid)   //同组
				{
					if (file_inode[a].file_mode[4] == 1)
					{
						w_f(a);
					}
					else printf("您不具备权限，请联系管理员修改权限！（chmod + 文件名）\n");
				}
				else
				{
					if (file_inode[a].file_mode[7] == 1)
					{
						w_f(a);
					}
					else printf("您不具备权限，请联系管理员修改权限！（chmod + 文件名）\n");
				}

				return;
			}
			p = p->rchild;
		}
		printf("不存在该文件！\n");
	}
}

//查看系统信息
void show_info()
{

	//打印系统当前信息
	printf("\t*************************\n");

	printf("\t当前系统信息:\n\n");
	int n_file, n_dir;   //统计文件个数和文件夹个数
	n_dir = n_file = 0;
	int i, m, k = 0;
	for (i = 0;i<20500;i++)
	{
		if (hx_superblock.phydata[i] == 0)
			k++;
	}
	m = 20500 - k;
	printf("\t空闲的盘块数是：\t");
	printf("%d\n", k);
	printf("\t使用的盘块数是：\t");
	printf("%d\n", m);
	//打印用户打开表状态

	printf("\t*************************\n");
	printf("\t系统表项信息为：\n\n");
	printf("\t当前用户打开表项个数为：\t");
	printf("%d\n", cul_num_usetalbe(cur_user.userid));
	printf("\t系统打开表项个数为：\t");
	printf("%d\n", cul_num_systable());
	printf("\t活动inode表项个数为：\t");
	printf("%d\n", cul_num_inodetable());
	printf("\t*************************\n");

	printf("\t活动节点已使用数：\t");
	printf("%d\n", find_num_of_file());
	printf("\t活动节点剩余个数：\t");
	printf("%d\n", 512 - find_num_of_file());

	cul_num(L_Ftree->lchild, n_file, n_dir);

	printf("\t目录节点已使用数：\t");
	printf("%d\n", n_dir + n_file);
	printf("\t文件夹个数为：\t");
	printf("\t%d\n", n_dir);
	printf("\t文件个数为：\t");
	printf("\t%d\n", n_file);
	printf("\t目录节点剩余个数：\t");
	printf("%d\n", 512 - (n_dir + n_file));

	//打印当前用户信息
	printf("\t****************************\n");
	printf("\t当前用户信息：\n\n");
	printf("\t\t用户ID：\t%d\n", cur_user.userid);
	printf("\t\t用户名：\t%s\n", cur_user.username);
	printf("\t\t用户组：\t%d\n", cur_user.group);
}

//注销
void logout(FILE *fp)
{
	//将系统信息写入文件
	WriteToFile(fp);
	//清屏
	system("cls");
	login();
}
//切换用户
int change_user(FILE *fp, char username[])
{
	char password[10];
	int count;
	int i = 0;
	int f = -1;
	//在已存在的用户中,查找要切换的用户
	for (i = 0;i<10;i++)
	{
		if (L_user[i].userid != -1)
			if (strcmp(username, L_user[i].username) == 0)
			{
				f = i;
				break;
			}
	}
	//查找不到
	if (f == -1)
	{
		printf("无此用户!\n");
		return -1;
	}
	//将目前的信息写入文件,防止刚刚操作丢失
	WriteToFile(fp);
	system("cls");
	//三次机会输入用户名密码
	for (count = 0; count < 3; count++)
	{
		printf("password:");
		i = 0;
		//获取输入密码，同时隐藏输入字符
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
			printf("\n登陆成功!\n");
			return 1;
			break;
		}
		else
		{
			printf("密码错误!\n");
		}
	}
	//登陆失败
	if (count >= 3)
	{
		printf("\n登陆失败!\n");
		return 0;
	}
	return 0;
}
//修改文件操作权限,只有文件的属主能够有权限修改文件权限
void change_mode(char filename[])
{
	int a;     //a代表代开文件的inode号
	FTreepoint p = NULL, p2 = NULL;
	path_tnode(cur_dir, L_Ftree, p);
	if (p->lchild == NULL)      //为空
	{
		printf("不存在该文件！\n");
	}
	else   //非空
	{
		p = p->lchild;
		while (p != NULL)
		{
			//
			if ((strcmp(p->data.file_name, filename) == 0) && (file_inode[p->data.dir_inode].file_style == 1))  //存在同名文件
			{
				a = p->data.dir_inode;
				//先判断文件是否打开
				int i = 0;
				for (i = 0;i<200;i++)
				{
					if (sys_open_table[i].f_inode == a)
					{
						break;
					}
				}
				if (i == 200)
				{
					printf("系统文件表项中无该文件，请先打开该文件或者创建该文件！\n");
					return;
				}
				if (cur_user.userid == file_inode[a].file_userid)     //属主
				{
					printf("请输入0、1串（相应的位为一代表有相应的权限，0代表没有）\n");
					printf("格式：r w e r w e r w e\n");
					//	scanf("%s",file_inode[a].file_mode);
					for (int j = 0;j<9;j++)
					{
						//		fflush(stdin);
						scanf("%d", &(file_inode[a].file_mode[j]));
					}
					return;
				}
				else printf("您不是文件的属主，不具备权限，请联系管理员修改权限！\n");
			}
			p = p->rchild;
		}
		printf("不存在该文件！\n");
	}
}

//用户管理模块
void manage_user()
{
	FILE *fp;
	fp = fopen("users.txt", "wb");
	//权限查找
	if (cur_user.level != 1)    //管理员才有权限进行管理
	{
		printf("对不起，你没有权限进行用户管理！\n");
		SaveUsers(fp);
		return;
	}
	printf("欢迎进入用户管理系统!\n");
	int i, j;
	int f;
	char tempuser[10];
	char temppass[10];
	int tempgroup;
	int tempuid;
	while (1)
	{
		printf("请选择操作(1.查看 2.插入 3.删除  0.保存退出):\n");
		scanf("%d", &f);
		//将文件信息保存,退出
		if (f == 0)
		{
			SaveUsers(fp);
			break;
		}
		//循环显示用户信息
		if (f == 1)
		{
			printf("用户ID\t用户名\t用户组\t用户等级\n");
			for (i = 0;i<10;i++)
			{
				if (L_user[i].userid != -1)
				{
					printf("%d\t%s\t%d\t%d\n", L_user[i].userid, L_user[i].username, L_user[i].group, L_user[i].level);
				}
			}
		}
		//插入用户信息
		if (f == 2)
		{
			//查找第一个空用户
			for (j = 0;j<10;j++)
			{
				if (L_user[j].userid == -1)
					break;
			}
			if (j == 10)
			{
				printf("用户已满，不能插入!\n");
				continue;
			}
			//读取用户输入信息
			printf("请输入用户名：");
			scanf("%s", &tempuser);
			printf("请输入密码：");
			scanf("%s", &temppass);
			printf("请输入组别（1或2或3）：");
			scanf("%d", &tempgroup);
			//验证用户输入信息正确与否
			if (tempgroup<1 || tempgroup>3)
			{
				printf("用户组错误，不能插入!\n");
				continue;
			}
			//将用户信息添加
			L_user[j].userid = j;
			strcpy(L_user[j].username, tempuser);
			strcpy(L_user[j].password, temppass);
			L_user[j].group = tempgroup;
			L_user[j].level = 0;
		}
		//删除用户信息
		if (f == 3)
		{
			printf("请输入要删除的用户Id：");
			scanf("%d", &tempuid);
			//验证用户输入的id是否正确
			if (tempuid<0 || tempuid>9)
			{
				printf("此用户不存在！\n");
				continue;
			}
			//超级管理员不可删除
			if (tempuid == 0)
			{
				printf("超级管理员不可删除！\n");
				continue;
			}
			//保存用户的各种信息
			L_user[tempuid].userid = -1;
			strcpy(L_user[tempuid].username, "");
			strcpy(L_user[tempuid].password, "");
			L_user[tempuid].group = -1;
		}
	}
}

void str2stack(SqStack &s);     //输入文件路径，转化成对应的栈路径存储
int findtreeinode(SqStack S, FTreepoint T, FTreepoint &p);     //根据路径找到对应节点

															   //硬链接函数
void h_link(char filename[])
{
	SqStack s;
	FTreepoint p = NULL, p2 = NULL, p3, p4;
	path_tnode(cur_dir, L_Ftree, p);
	int flag = -1;
	if (p->lchild == NULL)
	{
		printf("输入文件名有误，请检查是否该文件存在!\n");
		return;
	}
	if (p->lchild->rchild == NULL)   //说明当前只有一个文件
	{
		path_tnode(cur_dir, L_Ftree, p);
		p2 = p->lchild;
		for (int j = 0;j<200;j++)
		{
			if (sys_open_table[j].f_inode == p2->data.dir_inode)
			{
				printf("该文件已经打开，请关闭之后再进行连接操作！\n");return;
			}
		}
		if ((strcmp(p2->data.file_name, filename) == 0) && (file_inode[p2->data.dir_inode].file_style == 1))  //删除第一个文件
		{
			//连接操作

		}
	}
	else    //有多个文件
	{
		p2 = p->lchild;
		if ((strcmp(p2->data.file_name, filename) == 0) && (file_inode[p2->data.dir_inode].file_style == 1))  //删除第一个文件
		{
			//检查系统打开表是否有这个文件，有则不能删除
			for (int j = 0;j<200;j++)
			{
				if (sys_open_table[j].f_inode == p2->data.dir_inode)
				{
					printf("该文件已经打开，请关闭之后再进行连接操作！\n");return;
				}
			}

		}
		else
		{
			p = p->lchild;
			while ((p != NULL) && (p->rchild != NULL))
			{
				flag = 0;
				if ((strcmp(p->rchild->data.file_name, filename) == 0) && (file_inode[p->rchild->data.dir_inode].file_style == 1))  //存在同名文件
				{
					p2 = p->rchild;flag = 1;
					//检查系统打开表是否有这个文件，有则不能删除
					for (int j = 0;j<200;j++)
					{
						if (sys_open_table[j].f_inode == p2->data.dir_inode)
						{
							printf("该文件已经打开，请关闭之后再进行连接操作！\n");return;
						}
					}

				}
				p = p->rchild;
			}
		}
	}
	//权限检测，p2为当前删除的文件节点
	if (flag == 0) { printf("输入文件名有误，请检查是否存在该文件!\n"); return; }

	if (cur_user.userid == file_inode[p2->data.dir_inode].file_userid)     //属主
	{
		if (file_inode[p2->data.dir_inode].file_mode[1] == 1)
		{
			str2stack(s);
			findtreeinode(s, L_Ftree, p4);     //根据路径找到对应节点
			if (p4 == NULL) printf("输入文件不存在！请检查！\n");
			if (file_inode[p2->data.dir_inode].file_length>0)  //该文件原来有内容则删除
			{
				free_disk(p2->data.dir_inode);
			}
			file_inode[p2->data.dir_inode].inode_number = -1; //原来inode删除
			p2->data.dir_inode = p4->data.dir_inode;         //赋予新的inode
			file_inode[p4->data.dir_inode].file_icount++;  //硬链接加一

		}
		else printf("您不具备权限，请联系管理员修改权限！（chmod + 文件名）\n");

	}
	else if (cur_user.group == file_inode[p2->data.dir_inode].file_groupid)   //同组
	{
		if (file_inode[p2->data.dir_inode].file_mode[4] == 1)
		{
			str2stack(s);
			findtreeinode(s, L_Ftree, p4);     //根据路径找到对应节点
			if (p4 == NULL) printf("输入文件不存在！请检查！\n");
			if (file_inode[p2->data.dir_inode].file_length>0)  //该文件原来有内容则删除
			{
				free_disk(p2->data.dir_inode);
			}
			file_inode[p2->data.dir_inode].inode_number = -1; //原来inode删除
			p2->data.dir_inode = p4->data.dir_inode;         //赋予新的inode
			file_inode[p4->data.dir_inode].file_icount++;  //硬链接加一
		}
		else printf("您不具备权限，请联系管理员修改权限！（chmod + 文件名）\n");

	}
	else
	{
		if (file_inode[p2->data.dir_inode].file_mode[7] == 1)
		{
			str2stack(s);
			findtreeinode(s, L_Ftree, p4);     //根据路径找到对应节点
			if (p4 == NULL) printf("输入文件不存在！请检查！\n");
			if (file_inode[p2->data.dir_inode].file_length>0)  //delete file content
			{
				free_disk(p2->data.dir_inode);
			}
			file_inode[p2->data.dir_inode].inode_number = -1; //delete inode
			p2->data.dir_inode = p4->data.dir_inode;         //new inode
			file_inode[p4->data.dir_inode].file_icount++;  //hard link count add 1
		}
		else printf("您不具备权限，请联系管理员修改权限！（chmod + 文件名）\n");

	}
	if (p2 == NULL) { printf("输入文件名有误，请检查是否存在该文件!\n"); return; }
	int i = 0;
	clear_dir(file_dir);
	Tree_to_dir(i, file_dir, L_Ftree->lchild);
	FILE *fp1;
	WriteToFile(fp1);
}

void file2str(int a, char buff[], int n)   //a为inode号，buff为文件内容存放数组
{
	n = 0;
	int b = file_inode[a].file_length, i;
	if (b == 0)
	{
		printf("该软连接没有内容！请先输入内容！\n");
		return;
	}
	if (b<13)
	{
		for (i = 0;i<b;i++)
		{
			for (int j = 0;j<10;j++)
			{
				if (phy[file_inode[a].file_address[i]].p[j] == 0) return;
				buff[n] = phy[file_inode[a].file_address[i]].p[j];
				n++;
			}
		}
	}
	else
	{
		for (int i = 0;i<12;i++)
		{
			for (int j = 0;j<10;j++)
			{
				if (phy[file_inode[a].file_address[i]].p[j] == 0) return;
				buff[n] = phy[file_inode[a].file_address[i]].p[j];n++;
			}
		}
	}
	if (b >= 13)
	{
		//一次间接寻址
		int f1[2];
		char b1[5], b2[5];
		for (i = 0;i<5;i++)
		{
			b1[i] = phy[file_inode[a].file_address[12]].p[i];
			b2[i] = phy[file_inode[a].file_address[12]].p[i + 5];
		}
		f1[0] = atoi(b1);f1[1] = atoi(b2);
		for (i = 0;i<2;i++)
		{
			for (int j = 0;j<10;j++)
			{
				if (phy[f1[i]].p[j] == 0) return;
				buff[n] = phy[f1[i]].p[j];n++;
			}
		}
	}
	if (b >= 14)
	{
		//一次间接寻址
		int f1[2];
		char b1[5], b_2[5];
		for (i = 0;i<5;i++)
		{
			b1[i] = phy[file_inode[a].file_address[13]].p[i];
			b_2[i] = phy[file_inode[a].file_address[13]].p[i + 5];
		}
		f1[0] = atoi(b1);f1[1] = atoi(b_2);

		//二次间接寻址
		int f2[4];
		char b2[4][5];
		for (int j = 0;j<5;j++)
		{
			b2[0][j] = phy[f1[0]].p[j];
			b2[1][j] = phy[f1[0]].p[j + 5];
			b2[2][j] = phy[f1[1]].p[j];
			b2[3][j] = phy[f1[1]].p[j + 5];
		}
		for (i = 0;i<4;i++)
		{
			f2[i] = atoi(b2[i]);
		}

		for (i = 0;i<4;i++)
		{
			for (int j = 0;j<10;j++)
			{
				if (phy[f2[i]].p[j] == 0) return;
				buff[n] = phy[f2[i]].p[j];n++;
			}
		}
	}
	if (b >= 15)
	{
		//一次间接寻址
		int f1[2];
		char b1[5], b_2[5];
		for (i = 0;i<5;i++)
		{
			b1[i] = phy[file_inode[a].file_address[14]].p[i];
			b_2[i] = phy[file_inode[a].file_address[14]].p[i + 5];
		}
		f1[0] = atoi(b1);f1[1] = atoi(b_2);

		int f2[4];
		char b2[4][5];
		for (int j = 0;j<5;j++)
		{
			b2[0][j] = phy[f1[0]].p[j];
			b2[1][j] = phy[f1[0]].p[j + 5];
			b2[2][j] = phy[f1[1]].p[j];
			b2[3][j] = phy[f1[1]].p[j + 5];
		}
		for (i = 0;i<4;i++)
		{
			f2[i] = atoi(b2[i]);
		}

		int f3[8], j;
		char b3[8][5];
		for (j = 0;j<5;j++)
		{
			b3[0][j] = phy[f2[0]].p[j];
			b3[1][j] = phy[f2[0]].p[j + 5];
			b3[2][j] = phy[f2[1]].p[j];
			b3[3][j] = phy[f2[1]].p[j + 5];
			b3[4][j] = phy[f2[2]].p[j];
			b3[5][j] = phy[f2[2]].p[j + 5];
			b3[6][j] = phy[f2[3]].p[j];
			b3[7][j] = phy[f2[3]].p[j + 5];
		}
		for (i = 0;i<8;i++)
		{
			f3[i] = atoi(b3[i]);
		}


		for (i = 0;i<8;i++)
		{
			for (int j = 0;j<10;j++)
			{
				if (phy[f3[i]].p[j] == 0) return;
				buff[n] = phy[f3[i]].p[j];n++;
			}
		}
	}
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
//软连接函数
void s_link(char filename[])
{
	char buff[260] = { 0 };
	int n = 0;
	int a;     //a代表代开文件的inode号
	FTreepoint p = NULL, p2 = NULL;
	path_tnode(cur_dir, L_Ftree, p);
	if (p->lchild == NULL)      //为空
	{
		printf("不存在该文件！\n");
	}
	else   //非空
	{
		p = p->lchild;
		while (p != NULL)
		{
			if ((strcmp(p->data.file_name, filename) == 0) && (file_inode[p->data.dir_inode].file_style == 1))
			{
				a = p->data.dir_inode;
				if (cur_user.userid == file_inode[a].file_userid) 
				{
					if (file_inode[a].file_mode[0] == 1)
					{
						file2str(a, buff, n);
						buff[n] = 0;
						if (buff + 1 == NULL) { printf("软连接错误！\n");return; }
						do_file(buff + 1);
						printf("\n");
					}
					else printf("您不具备权限，请联系管理员修改权限！（chmod + 文件名）\n");
				}
				else if (cur_user.group == file_inode[a].file_groupid)
				{
					if (file_inode[a].file_mode[3] == 1)
					{
						file2str(a, buff, n);buff[n] = 0;do_file(buff + 1);	printf("\n");
					}
					else printf("您不具备权限，请联系管理员修改权限！（chmod + 文件名）\n");
				}
				else
				{
					if (file_inode[a].file_mode[6] == 1)
					{
						file2str(a, buff, n);buff[n] = 0;do_file(buff + 1);	printf("\n");
					}
					else printf("您不具备权限，请联系管理员修改权限！（chmod + 文件名）\n");
				}

				return;
			}
			p = p->rchild;
		}
		printf("不存在该文件！\n");
	}
}

//rename
void rename(char filename[])
{

	char newfname[10];
	FTreepoint p = NULL, p2 = NULL, p3;
	path_tnode(cur_dir, L_Ftree, p);
	if (p->lchild == NULL)      //为空
	{
		printf("File not existed\n");
	}
	else
	{
		p2 = p->lchild;
		while (p2 != NULL)
		{
			if ((strcmp(p2->data.file_name, filename) == 0) && (file_inode[p2->data.dir_inode].file_style == 1))  //存在同名文件
			{
				printf("Please input a new file name:");
				scanf("%s", newfname);
				if (strcmp(newfname, filename) == 0) { printf("Please do not input the same name^-^!\n");return; }
				p3 = p->lchild;
				while (p3 != NULL)
				{
					if ((strcmp(p3->data.file_name, newfname) == 0) && (file_inode[p3->data.dir_inode].file_style == 1))  //存在同名文件
					{
						printf("This name has already used^-^!\n");return;
					}
					p3 = p3->rchild;
				}
				strcpy(p2->data.file_name, newfname);
				return;
			}
			p2 = p2->rchild;
		}
	}
	int i = 0;
	clear_dir(file_dir);
	Tree_to_dir(i, file_dir, L_Ftree->lchild);
	FILE *fp;
	WriteToFile(fp);
}