/*
* Copyright (c) 2016, Randolph Han, Zhejiang University of Technology, China
* All rights reserved.
* Project name£ºSimple File System
* Programmer£ºRandolph Han
* Finish£º2016.12.12
*
*/
#pragma once

/*
*	Size of a block: 512.
*/
#define DATA_BLOCK_SIZE 512
/*
*	Number of direct adressing: 4.
*/
#define DIRECT_DATA_COUNT 4
/*
*	Number of first level indirect addressing.
*/
#define SINGAL_DATA_COUNT sizeof(inode) / sizeof(short)
/*
*	Number of second level indirect addressing.
*/
#define DOUBLE_DATA_COUNT sizeof(inode) / sizeof(short) * sizeof(inode) / sizeof(short)
/*
*	The maximum length of a file name.
*/
#define FILE_NAME_LENGTH 14
/*
*	The maximum deep of free inode stack.
*/
#define FREE_INODE_STACK_SIZE 128
/*
*	The maximum deep of free block stack.
*/
#define FREE_BLOCK_STACK_SIZE 20
/*
*	The maximum number of inodes.
*/
#define INODES_COUNT 128
/*
*	The number of data blocks.
*/
#define DATA_BLOCKS_COUNT 120				
/*
*	The maximum number of the file system users.
*/
#define USER_COUNT 15
/*
*	The maximum number of files allowing to access.
*/
#define USER_ALLOW_OPEN_COUNT 15	
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
#define INODE_TYPE 1 << 0
#define USER_R 1 << 1
#define USER_W 1 << 2
#define USER_X 1 << 3
#define GROUP_R 1 << 4
#define GROUP_W 1 << 5
#define GROUP_X 1 << 6
#define OTHER_R 1 << 7
#define OTHER_W 1 << 8
#define OTHER_X 1 << 9
/*
*	ALL permissions to a file.
*/
#define FILE_MAX (USER_R | USER_W | GROUP_R | GROUP_W | OTHER_R | OTHER_W)
/*
*	ALL permissions to a directory.
*/
#define DIR_MAX (USER_R | USER_W | GROUP_R | GROUP_W | OTHER_R | OTHER_W)
/*
*	Size of direct adressing block.
*/
#define FILE_DIRECT_SIZE_MAX DATA_BLOCK_SIZE * DIRECT_DATA_COUNT
/*
*	Size of first indirect adressing block.
*/
#define FILE_SINGAL_SIZE_MAX sizeof(Inode) / sizeof(short) * DATA_BLOCK_SIZE
/*
*	Size of second indirect adressing block.
*/
#define FILE_DOUBLE_SIZE_MAX sizeof(Inode) / sizeof(short) * sizeof(Inode) / sizeof(short) * DATA_BLOCK_SIZE
/*
*	The maximum size of a file.
*/
#define FILE_SIZE_MAX FILE_DIRECT_SIZE_MAX + FILE_SINGAL_SIZE_MAX + FILE_DOUBLE_SIZE_MAX
/*
*	The maximum length of a command.
*/
#define CMD_LENGTH 10
/*
*	The maximum number of commands.
*/
#define CMD_COUNT 20
/*
*	The inode id for root directory.
*/
#define ROOT_INODE_NO 0
/*
*	The inode number of users' data.
*/
#define USERS_INODE_NO 1
/*
*	The inode number of groups' data.
*/
#define GROUPS_INODE_NO 2
/*
*	Administrator's id.
*/
#define SUPER_USER_ID 0
/*
*	Administrator group id.
*/
#define SUPER_GROUP_ID 0
/*
*	User group id.
*/
#define GENERAL_GROUP_ID 1
/*
*	The maximum size of user's name.
*/
#define USER_NAME_LENGTH 16
/*
*	The maximum size of user's password.
*/
#define USER_PASSWORD_LENGTH 16
/*
*	The maximum size of group's name.
*/
#define GROUP_NAME_LENGTH 16
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