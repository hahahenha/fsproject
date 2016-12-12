/*
* Copyright (c) 2016, Randolph Han, Zhejiang University of Technology, China
* All rights reserved.
* Project name��Simple File System
* Programmer��Randolph Han
* Finish��2016.12.11-2016.12.??
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
	printf("\t01.�˳�ϵͳ.......................................(exit)\n");
	printf("\t02.��ʾ��������...................................(help)\n");
	printf("\t03.�鿴��ǰĿ¼�ļ��б�.............................(ls)\n");
	printf("\t04.���뵱ǰĿ¼�µ�ָ��Ŀ¼................(cd + Ŀ¼��)\n");
	printf("\t05.������һ��Ŀ¼.................................(cd..)\n");
	printf("\t06.����Ŀ¼.............................(mkdir + Ŀ¼��)\n");
	printf("\t07.ɾ��Ŀ¼.............................(rmdir + Ŀ¼��)\n");
	printf("\t08.�����ļ�............................(create + �ļ���)\n");
	printf("\t09.���ļ�..............................(open + �ļ���)\n");
	printf("\t10.�ļ��Ķ�..............................(read + �ļ���)\n");
	printf("\t11.�ļ���д.............................(write + �ļ���)\n");
	printf("\t12.�ر��ļ�.............................(close + �ļ���)\n");
	printf("\t13.ɾ���ļ�................................(rm + �ļ���)\n");
	printf("\t14.�鿴ϵͳ��Ϣ...................................(info)\n");
	printf("\t15.ע����������.................................(logout)\n");
	printf("\t16.�л��û�................................(su + �û���)\n");
	printf("\t17.�����ļ�����Ȩ��.....................(chmod + �ļ���)\n");
	printf("\t18.�����û�(��ʾ����˵�)........................(Muser)\n");
	printf("\t19.Ӳ����...............................(hlink + �ļ���)\n");
	printf("\t20.������...............................(slink + �ļ���)\n");
	printf("\t21.������..................................(mv + �ļ���)\n");
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
		printf("Already at root dir��\n");
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
				printf("Already exist a same name dir��Create failed��\n");
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
				printf("Already exist a same name file��Create failed��\n");
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
				printf("The file has already opened, please close it before delete��\n");return;
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
	int a;     //a��������ļ���inode��
	FTreepoint p = NULL, p2 = NULL;
	path_tnode(cur_dir, L_Ftree, p);
	if (p->lchild == NULL){
		printf("File not exist��\n");
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
	int a;     //a��������ļ���inode��
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
	printf("File content��\n");
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
				else if (cur_user.group == file_inode[a].file_groupid)   //ͬ��
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
	printf("Please input the content��end with '$'��(�����ļ����ܳ���13312���ַ�����(12+2+4+8)*512=13312��������һ�����̿�512�ַ����㣩)\n");
	//buffer
	while ((ch = getchar()) != '$'){
		buffer[num] = ch;
		num++;
	}
	buffer[num] = 0;
	printf("Total: %d��\n", num);

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
	if (num>12*512){   //�򻯴���ֱ�ӿ��������洢�ռ�
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
		//��ʼ�洢����
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
		//һ�μ��Ѱַ
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
		//���μ��Ѱַ
		for (i = 0;i<256;i++)
		{
			phy[first].p[i] = b1[i];
			phy[first].p[i + 256] = b2[i];
			phy[second].p[i] = b3[i];
			phy[second].p[i + 256] = b4[i];
		}
		//��ʼ�洢����
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

//д�ļ��������ļ����ܳ���2600���ַ�����(12+2+4+8)*512=2600��������һ�����̿�10�ַ����㣩һ��д����д��������Ϊд��
void write_file(char filename[])
{
	int a;     //a��������ļ���inode��
	FTreepoint p = NULL, p2 = NULL;
	path_tnode(cur_dir, L_Ftree, p);
	if (p->lchild == NULL)      //Ϊ��
	{
		printf("�����ڸ��ļ���\n");
	}
	else   //�ǿ�
	{
		p = p->lchild;
		while (p != NULL)
		{
			if ((strcmp(p->data.file_name, filename) == 0) && (file_inode[p->data.dir_inode].file_style == 1))  //����ͬ���ļ�
			{
				//�ر��ļ��������û���ϵͳ�ǼǱ�ͻinode��Ҫ����
				a = p->data.dir_inode;
				if (cur_user.userid == file_inode[a].file_userid)     //����
				{
					if (file_inode[a].file_mode[1] == 1)
					{
						w_f(a);
					}
					else printf("�����߱�Ȩ�ޣ�����ϵ����Ա�޸�Ȩ�ޣ���chmod + �ļ�����\n");
				}
				else if (cur_user.group == file_inode[a].file_groupid)   //ͬ��
				{
					if (file_inode[a].file_mode[4] == 1)
					{
						w_f(a);
					}
					else printf("�����߱�Ȩ�ޣ�����ϵ����Ա�޸�Ȩ�ޣ���chmod + �ļ�����\n");
				}
				else
				{
					if (file_inode[a].file_mode[7] == 1)
					{
						w_f(a);
					}
					else printf("�����߱�Ȩ�ޣ�����ϵ����Ա�޸�Ȩ�ޣ���chmod + �ļ�����\n");
				}

				return;
			}
			p = p->rchild;
		}
		printf("�����ڸ��ļ���\n");
	}
}

//�鿴ϵͳ��Ϣ
void show_info()
{

	//��ӡϵͳ��ǰ��Ϣ
	printf("\t*************************\n");

	printf("\t��ǰϵͳ��Ϣ:\n\n");
	int n_file, n_dir;   //ͳ���ļ��������ļ��и���
	n_dir = n_file = 0;
	int i, m, k = 0;
	for (i = 0;i<20500;i++)
	{
		if (hx_superblock.phydata[i] == 0)
			k++;
	}
	m = 20500 - k;
	printf("\t���е��̿����ǣ�\t");
	printf("%d\n", k);
	printf("\tʹ�õ��̿����ǣ�\t");
	printf("%d\n", m);
	//��ӡ�û��򿪱�״̬

	printf("\t*************************\n");
	printf("\tϵͳ������ϢΪ��\n\n");
	printf("\t��ǰ�û��򿪱������Ϊ��\t");
	printf("%d\n", cul_num_usetalbe(cur_user.userid));
	printf("\tϵͳ�򿪱������Ϊ��\t");
	printf("%d\n", cul_num_systable());
	printf("\t�inode�������Ϊ��\t");
	printf("%d\n", cul_num_inodetable());
	printf("\t*************************\n");

	printf("\t��ڵ���ʹ������\t");
	printf("%d\n", find_num_of_file());
	printf("\t��ڵ�ʣ�������\t");
	printf("%d\n", 512 - find_num_of_file());

	cul_num(L_Ftree->lchild, n_file, n_dir);

	printf("\tĿ¼�ڵ���ʹ������\t");
	printf("%d\n", n_dir + n_file);
	printf("\t�ļ��и���Ϊ��\t");
	printf("\t%d\n", n_dir);
	printf("\t�ļ�����Ϊ��\t");
	printf("\t%d\n", n_file);
	printf("\tĿ¼�ڵ�ʣ�������\t");
	printf("%d\n", 512 - (n_dir + n_file));

	//��ӡ��ǰ�û���Ϣ
	printf("\t****************************\n");
	printf("\t��ǰ�û���Ϣ��\n\n");
	printf("\t\t�û�ID��\t%d\n", cur_user.userid);
	printf("\t\t�û�����\t%s\n", cur_user.username);
	printf("\t\t�û��飺\t%d\n", cur_user.group);
}

//ע��
void logout(FILE *fp)
{
	//��ϵͳ��Ϣд���ļ�
	WriteToFile(fp);
	//����
	system("cls");
	login();
}
//�л��û�
int change_user(FILE *fp, char username[])
{
	char password[10];
	int count;
	int i = 0;
	int f = -1;
	//���Ѵ��ڵ��û���,����Ҫ�л����û�
	for (i = 0;i<10;i++)
	{
		if (L_user[i].userid != -1)
			if (strcmp(username, L_user[i].username) == 0)
			{
				f = i;
				break;
			}
	}
	//���Ҳ���
	if (f == -1)
	{
		printf("�޴��û�!\n");
		return -1;
	}
	//��Ŀǰ����Ϣд���ļ�,��ֹ�ող�����ʧ
	WriteToFile(fp);
	system("cls");
	//���λ��������û�������
	for (count = 0; count < 3; count++)
	{
		printf("password:");
		i = 0;
		//��ȡ�������룬ͬʱ���������ַ�
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
			printf("\n��½�ɹ�!\n");
			return 1;
			break;
		}
		else
		{
			printf("�������!\n");
		}
	}
	//��½ʧ��
	if (count >= 3)
	{
		printf("\n��½ʧ��!\n");
		return 0;
	}
	return 0;
}
//�޸��ļ�����Ȩ��,ֻ���ļ��������ܹ���Ȩ���޸��ļ�Ȩ��
void change_mode(char filename[])
{
	int a;     //a��������ļ���inode��
	FTreepoint p = NULL, p2 = NULL;
	path_tnode(cur_dir, L_Ftree, p);
	if (p->lchild == NULL)      //Ϊ��
	{
		printf("�����ڸ��ļ���\n");
	}
	else   //�ǿ�
	{
		p = p->lchild;
		while (p != NULL)
		{
			//
			if ((strcmp(p->data.file_name, filename) == 0) && (file_inode[p->data.dir_inode].file_style == 1))  //����ͬ���ļ�
			{
				a = p->data.dir_inode;
				//���ж��ļ��Ƿ��
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
					printf("ϵͳ�ļ��������޸��ļ������ȴ򿪸��ļ����ߴ������ļ���\n");
					return;
				}
				if (cur_user.userid == file_inode[a].file_userid)     //����
				{
					printf("������0��1������Ӧ��λΪһ��������Ӧ��Ȩ�ޣ�0����û�У�\n");
					printf("��ʽ��r w e r w e r w e\n");
					//	scanf("%s",file_inode[a].file_mode);
					for (int j = 0;j<9;j++)
					{
						//		fflush(stdin);
						scanf("%d", &(file_inode[a].file_mode[j]));
					}
					return;
				}
				else printf("�������ļ������������߱�Ȩ�ޣ�����ϵ����Ա�޸�Ȩ�ޣ�\n");
			}
			p = p->rchild;
		}
		printf("�����ڸ��ļ���\n");
	}
}

//�û�����ģ��
void manage_user()
{
	FILE *fp;
	fp = fopen("users.txt", "wb");
	//Ȩ�޲���
	if (cur_user.level != 1)    //����Ա����Ȩ�޽��й���
	{
		printf("�Բ�����û��Ȩ�޽����û�����\n");
		SaveUsers(fp);
		return;
	}
	printf("��ӭ�����û�����ϵͳ!\n");
	int i, j;
	int f;
	char tempuser[10];
	char temppass[10];
	int tempgroup;
	int tempuid;
	while (1)
	{
		printf("��ѡ�����(1.�鿴 2.���� 3.ɾ��  0.�����˳�):\n");
		scanf("%d", &f);
		//���ļ���Ϣ����,�˳�
		if (f == 0)
		{
			SaveUsers(fp);
			break;
		}
		//ѭ����ʾ�û���Ϣ
		if (f == 1)
		{
			printf("�û�ID\t�û���\t�û���\t�û��ȼ�\n");
			for (i = 0;i<10;i++)
			{
				if (L_user[i].userid != -1)
				{
					printf("%d\t%s\t%d\t%d\n", L_user[i].userid, L_user[i].username, L_user[i].group, L_user[i].level);
				}
			}
		}
		//�����û���Ϣ
		if (f == 2)
		{
			//���ҵ�һ�����û�
			for (j = 0;j<10;j++)
			{
				if (L_user[j].userid == -1)
					break;
			}
			if (j == 10)
			{
				printf("�û����������ܲ���!\n");
				continue;
			}
			//��ȡ�û�������Ϣ
			printf("�������û�����");
			scanf("%s", &tempuser);
			printf("���������룺");
			scanf("%s", &temppass);
			printf("���������1��2��3����");
			scanf("%d", &tempgroup);
			//��֤�û�������Ϣ��ȷ���
			if (tempgroup<1 || tempgroup>3)
			{
				printf("�û�����󣬲��ܲ���!\n");
				continue;
			}
			//���û���Ϣ���
			L_user[j].userid = j;
			strcpy(L_user[j].username, tempuser);
			strcpy(L_user[j].password, temppass);
			L_user[j].group = tempgroup;
			L_user[j].level = 0;
		}
		//ɾ���û���Ϣ
		if (f == 3)
		{
			printf("������Ҫɾ�����û�Id��");
			scanf("%d", &tempuid);
			//��֤�û������id�Ƿ���ȷ
			if (tempuid<0 || tempuid>9)
			{
				printf("���û������ڣ�\n");
				continue;
			}
			//��������Ա����ɾ��
			if (tempuid == 0)
			{
				printf("��������Ա����ɾ����\n");
				continue;
			}
			//�����û��ĸ�����Ϣ
			L_user[tempuid].userid = -1;
			strcpy(L_user[tempuid].username, "");
			strcpy(L_user[tempuid].password, "");
			L_user[tempuid].group = -1;
		}
	}
}

void str2stack(SqStack &s);     //�����ļ�·����ת���ɶ�Ӧ��ջ·���洢
int findtreeinode(SqStack S, FTreepoint T, FTreepoint &p);     //����·���ҵ���Ӧ�ڵ�

															   //Ӳ���Ӻ���
void h_link(char filename[])
{
	SqStack s;
	FTreepoint p = NULL, p2 = NULL, p3, p4;
	path_tnode(cur_dir, L_Ftree, p);
	int flag = -1;
	if (p->lchild == NULL)
	{
		printf("�����ļ������������Ƿ���ļ�����!\n");
		return;
	}
	if (p->lchild->rchild == NULL)   //˵����ǰֻ��һ���ļ�
	{
		path_tnode(cur_dir, L_Ftree, p);
		p2 = p->lchild;
		for (int j = 0;j<200;j++)
		{
			if (sys_open_table[j].f_inode == p2->data.dir_inode)
			{
				printf("���ļ��Ѿ��򿪣���ر�֮���ٽ������Ӳ�����\n");return;
			}
		}
		if ((strcmp(p2->data.file_name, filename) == 0) && (file_inode[p2->data.dir_inode].file_style == 1))  //ɾ����һ���ļ�
		{
			//���Ӳ���

		}
	}
	else    //�ж���ļ�
	{
		p2 = p->lchild;
		if ((strcmp(p2->data.file_name, filename) == 0) && (file_inode[p2->data.dir_inode].file_style == 1))  //ɾ����һ���ļ�
		{
			//���ϵͳ�򿪱��Ƿ�������ļ���������ɾ��
			for (int j = 0;j<200;j++)
			{
				if (sys_open_table[j].f_inode == p2->data.dir_inode)
				{
					printf("���ļ��Ѿ��򿪣���ر�֮���ٽ������Ӳ�����\n");return;
				}
			}

		}
		else
		{
			p = p->lchild;
			while ((p != NULL) && (p->rchild != NULL))
			{
				flag = 0;
				if ((strcmp(p->rchild->data.file_name, filename) == 0) && (file_inode[p->rchild->data.dir_inode].file_style == 1))  //����ͬ���ļ�
				{
					p2 = p->rchild;flag = 1;
					//���ϵͳ�򿪱��Ƿ�������ļ���������ɾ��
					for (int j = 0;j<200;j++)
					{
						if (sys_open_table[j].f_inode == p2->data.dir_inode)
						{
							printf("���ļ��Ѿ��򿪣���ر�֮���ٽ������Ӳ�����\n");return;
						}
					}

				}
				p = p->rchild;
			}
		}
	}
	//Ȩ�޼�⣬p2Ϊ��ǰɾ�����ļ��ڵ�
	if (flag == 0) { printf("�����ļ������������Ƿ���ڸ��ļ�!\n"); return; }

	if (cur_user.userid == file_inode[p2->data.dir_inode].file_userid)     //����
	{
		if (file_inode[p2->data.dir_inode].file_mode[1] == 1)
		{
			str2stack(s);
			findtreeinode(s, L_Ftree, p4);     //����·���ҵ���Ӧ�ڵ�
			if (p4 == NULL) printf("�����ļ������ڣ����飡\n");
			if (file_inode[p2->data.dir_inode].file_length>0)  //���ļ�ԭ����������ɾ��
			{
				free_disk(p2->data.dir_inode);
			}
			file_inode[p2->data.dir_inode].inode_number = -1; //ԭ��inodeɾ��
			p2->data.dir_inode = p4->data.dir_inode;         //�����µ�inode
			file_inode[p4->data.dir_inode].file_icount++;  //Ӳ���Ӽ�һ

		}
		else printf("�����߱�Ȩ�ޣ�����ϵ����Ա�޸�Ȩ�ޣ���chmod + �ļ�����\n");

	}
	else if (cur_user.group == file_inode[p2->data.dir_inode].file_groupid)   //ͬ��
	{
		if (file_inode[p2->data.dir_inode].file_mode[4] == 1)
		{
			str2stack(s);
			findtreeinode(s, L_Ftree, p4);     //����·���ҵ���Ӧ�ڵ�
			if (p4 == NULL) printf("�����ļ������ڣ����飡\n");
			if (file_inode[p2->data.dir_inode].file_length>0)  //���ļ�ԭ����������ɾ��
			{
				free_disk(p2->data.dir_inode);
			}
			file_inode[p2->data.dir_inode].inode_number = -1; //ԭ��inodeɾ��
			p2->data.dir_inode = p4->data.dir_inode;         //�����µ�inode
			file_inode[p4->data.dir_inode].file_icount++;  //Ӳ���Ӽ�һ
		}
		else printf("�����߱�Ȩ�ޣ�����ϵ����Ա�޸�Ȩ�ޣ���chmod + �ļ�����\n");

	}
	else
	{
		if (file_inode[p2->data.dir_inode].file_mode[7] == 1)
		{
			str2stack(s);
			findtreeinode(s, L_Ftree, p4);     //����·���ҵ���Ӧ�ڵ�
			if (p4 == NULL) printf("�����ļ������ڣ����飡\n");
			if (file_inode[p2->data.dir_inode].file_length>0)  //delete file content
			{
				free_disk(p2->data.dir_inode);
			}
			file_inode[p2->data.dir_inode].inode_number = -1; //delete inode
			p2->data.dir_inode = p4->data.dir_inode;         //new inode
			file_inode[p4->data.dir_inode].file_icount++;  //hard link count add 1
		}
		else printf("�����߱�Ȩ�ޣ�����ϵ����Ա�޸�Ȩ�ޣ���chmod + �ļ�����\n");

	}
	if (p2 == NULL) { printf("�����ļ������������Ƿ���ڸ��ļ�!\n"); return; }
	int i = 0;
	clear_dir(file_dir);
	Tree_to_dir(i, file_dir, L_Ftree->lchild);
	FILE *fp1;
	WriteToFile(fp1);
}

void file2str(int a, char buff[], int n)   //aΪinode�ţ�buffΪ�ļ����ݴ������
{
	n = 0;
	int b = file_inode[a].file_length, i;
	if (b == 0)
	{
		printf("��������û�����ݣ������������ݣ�\n");
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
		//һ�μ��Ѱַ
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
		//һ�μ��Ѱַ
		int f1[2];
		char b1[5], b_2[5];
		for (i = 0;i<5;i++)
		{
			b1[i] = phy[file_inode[a].file_address[13]].p[i];
			b_2[i] = phy[file_inode[a].file_address[13]].p[i + 5];
		}
		f1[0] = atoi(b1);f1[1] = atoi(b_2);

		//���μ��Ѱַ
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
		//һ�μ��Ѱַ
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
//�����Ӻ���
void s_link(char filename[])
{
	char buff[260] = { 0 };
	int n = 0;
	int a;     //a��������ļ���inode��
	FTreepoint p = NULL, p2 = NULL;
	path_tnode(cur_dir, L_Ftree, p);
	if (p->lchild == NULL)      //Ϊ��
	{
		printf("�����ڸ��ļ���\n");
	}
	else   //�ǿ�
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
						if (buff + 1 == NULL) { printf("�����Ӵ���\n");return; }
						do_file(buff + 1);
						printf("\n");
					}
					else printf("�����߱�Ȩ�ޣ�����ϵ����Ա�޸�Ȩ�ޣ���chmod + �ļ�����\n");
				}
				else if (cur_user.group == file_inode[a].file_groupid)
				{
					if (file_inode[a].file_mode[3] == 1)
					{
						file2str(a, buff, n);buff[n] = 0;do_file(buff + 1);	printf("\n");
					}
					else printf("�����߱�Ȩ�ޣ�����ϵ����Ա�޸�Ȩ�ޣ���chmod + �ļ�����\n");
				}
				else
				{
					if (file_inode[a].file_mode[6] == 1)
					{
						file2str(a, buff, n);buff[n] = 0;do_file(buff + 1);	printf("\n");
					}
					else printf("�����߱�Ȩ�ޣ�����ϵ����Ա�޸�Ȩ�ޣ���chmod + �ļ�����\n");
				}

				return;
			}
			p = p->rchild;
		}
		printf("�����ڸ��ļ���\n");
	}
}

//rename
void rename(char filename[])
{

	char newfname[10];
	FTreepoint p = NULL, p2 = NULL, p3;
	path_tnode(cur_dir, L_Ftree, p);
	if (p->lchild == NULL)      //Ϊ��
	{
		printf("File not existed\n");
	}
	else
	{
		p2 = p->lchild;
		while (p2 != NULL)
		{
			if ((strcmp(p2->data.file_name, filename) == 0) && (file_inode[p2->data.dir_inode].file_style == 1))  //����ͬ���ļ�
			{
				printf("Please input a new file name:");
				scanf("%s", newfname);
				if (strcmp(newfname, filename) == 0) { printf("Please do not input the same name^-^!\n");return; }
				p3 = p->lchild;
				while (p3 != NULL)
				{
					if ((strcmp(p3->data.file_name, newfname) == 0) && (file_inode[p3->data.dir_inode].file_style == 1))  //����ͬ���ļ�
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