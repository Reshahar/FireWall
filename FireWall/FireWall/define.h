#ifndef DEF_H
#define DEF_H

#define MAXPATH 1024 

//通信消息结构体
typedef struct _msg_struct
{
	char 	ProcessName[MAXPATH];
	int		ProcessNameLen;
	int		Pid;
	int		flag;		
	char 	OtherMsg[MAXPATH];
	int     OtherMsglen;
}MSG_STRUCT,*PMSG_STRUCT;

#define PROCESS_CHOSE	0
#define REG_CHOSE		1
#define FILE_CHOSE      2
#define NET_CHOSE		3
#define PROCESS_LOG_FILE "\\Processlog.txt"
#define REG_LOG_FILE	 "\\Reglog.txt"
#define FILE_LOG_FILE	 "\\Filelog.txt"
#define NET_LOG_FILE	 "\\Netlog.txt"
#define INI_FILE_NAME		"\\INIFILE.txt"

#define PROCESS_LIST_FILE  "\\process.txt"
#define REG_LIST_FILE		"\\reg.txt"
#define FILE_LIST_FILE		"\\file.txt"
#define NET_LIST_FILE		"\\net.txt"
#define DRIVER_PATH			"\\sys\\SSDT.sys"	
#define SERVICE_NAME_FILE   "\\service_name.txt"
#define BASE_PATH           "\\Data"

#define PROCESS_CREATE 0
#define PROCESS_EXIT  1
#define REG_SET_VALUE 2
#define REG_DEL_VALUE 3
#define MYFILE_CREATE   4
#define MYFILE_DELETE   5
#define NET_PASS		6
#define NET_DENY		7



#define MY_THREAD_DEFAULT			0


#define MY_PROCESS_CREATE_THREAD	1
#define MY_PROCESS_EXIT_THREAD		2
#define MY_PROCESS_ALL_THREAD		3

#define MY_REG_SET_THREAD			4
#define MY_REG_DEL_THREAD			5
#define MY_REG_ALL_THREAD			6

#define MY_FILE_CREATE_THREAD		7
#define MY_FILE_DELETE_THREAD		8
#define MY_FILE_ALL_THREAD			9

#define MY_NET_PASS_THREAD			10
#define MY_NET_DENY_THREAD			11
#define MY_NET_ALL_THREAD			12

#define STATUS_NUM		11

#define MY_NO_CMD		0
#define MY_NO_REG		1
#define MY_NO_HIDE		2
#define MY_NO_PING		3
#define MY_NO_NET		4
#define MY_NO_DRIVER		5
#define MY_NO_ADD_USER		6
#define MY_NO_DEL_USER		7
#define MY_NO_CHANGE_TIME		8
#define MY_NO_START		9
#define MY_NO_TO_DISH		10

#define MAX_BOX 50



#endif	


