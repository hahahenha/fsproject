/*
* Copyright (c) 2016, Randolph Han, Zhejiang University of Technology, China
* All rights reserved.
* Project name£ºSimple File System
* Programmer£ºRandolph Han
* Finish£º2016.12.18
*
*/
#pragma once
/*
*	Size of physical data: 20480.
*/
#define PHY_DATA_SIZE 20480
/*
*	50 blocks/group
*/
#define BLOCK_GROUP_NUM 50
/*
*	The maximum number of inodes.
*/
#define INODES_COUNT 512
/*
*	The maximum number of directories.
*/
#define DIR_COUNT 512
/*
*	Size of a block: 512.
*/
#define DATA_BLOCK_SIZE 10
/*
*	First indirect addressing block number
*/
#define FIRST_INDIRECT_NUM 2
/*
*	Second indirect addressing block number
*/
#define SECOND_INDIRECT_NUM FIRST_INDIRECT_NUM*2
/*
*	Third indirect addressing block number
*/
#define THIRD_INDIRECT_NUM SECOND_INDIRECT_NUM*2
/*
*	Size of BLOCK_GTOUP
*/
#define BLOCK_GROUP_SIZE (PHY_DATA_SIZE/DATA_BLOCK_SIZE)
/*
*	Number of adressing: 15.
*/
#define DATA_COUNT 15
/*
*	The maximum length of a file name.
*/
#define FILE_NAME_LENGTH 14
/*
*	The maximum length of a directory name.
*/
#define DIR_NAME_LENGTH 14
/*
*	The maximum length of a path.
*/
#define PATH_LENGTH 14
/*
*	The maximum deep of stack.
*/
#define STACK_SIZE 100
/*
*	The maximum deep of free inode stack.
*/
#define FREE_INODE_STACK_SIZE 50
/*
*	The number of data blocks.
*/
#define DATA_BLOCKS_COUNT 120				
/*
*	The maximum number of the file system users.
*/
#define USER_COUNT 5
/*
*	The maximum number of files allowing to access.
*/
#define USER_ALLOW_OPEN_COUNT 25
/*
*	The maximum number of files allowing to access.
*/
#define SYSTEM_ALLOW_OPEN_COUNT 200
/*
*						File related
*	First bit:	The type of inode(0 stands for a directory, 1 stands for a file.).
*	Second bit:	Write permission for the user.
*	Third bit:	Read permissions for the user.
*	Forth bit:	Executable permissions for the user.
*	Fifth bit:	Write permission for the user group.
*	Sixth bit:	Read permissions for the users group.
*	Seventh bit:Executable permissions for the users group.
*	Eight bit:	Write permission for other users.
*	Ninth bit:	Read permissions for other users.
*	Tenth bit:	Executable permissions for other users.
*/
#define PERMISSIONS 9
/*
*	The maximum length of a command.
*/
#define CMD_LENGTH 10
/*
*	The maximum number of commands.
*/
#define COM_NUM 26
/*
*	The maximum size of user's name.
*/
#define USER_NAME_LENGTH 14
/*
*	The maximum size of user's password.
*/
#define USER_PASSWORD_LENGTH 14
/*
*	The maximum size of group's name.
*/
#define GROUP_NAME_LENGTH 14
/*
*	The maximum number of a group's account.
*/
#define GROUP_USER_COUNT 8
/*
*	Default password.
*/
#define DEFAULT_PASSWORD " "
/*
*	Disk name
*/
#define DISK_NAME "disk.han"
/*
*	Root folder name
*/
#define ROOT_FOLDER_NAME "/"
/*
*	Users' file name
*/
#define USERS_FILE_NAME "pwd"
/*
*	Groups' file name
*/
#define GROUPS_FILE_NAME "group"
/*
*	File content buffer(maximum size of file)
*/
#define FILE_BUFFER ((DATA_COUNT-3)+FIRST_INDIRECT_NUM+SECOND_INDIRECT_NUM+THIRD_INDIRECT_NUM)*DATA_BLOCK_SIZE
/*
*	File address buffer
*/
#define FILE_ADDRESS_BUFFER 1000
/*
*	Try username & password times number
*/
#define TRY_NUM 5