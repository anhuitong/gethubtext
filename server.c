#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <signal.h>
#include <sqlite3.h>
#include <time.h>
typedef struct{
	int userid;
	char name[32];
	int age;
	char sex[8];
	float salary;
	char depar[32];
	char address[32];
	int phone;
	char time[32];
	int password;
}message;

char * my_time()
{
	time_t now;
	struct tm * timenow;  
	time(&now);
	timenow=localtime(&now);
	return asctime(timenow);
	//printf("%s\n",asctime(timenow));
}
void denglu(int client_fd,sqlite3 *db)
{
	message msg;
	int ret=0;
	char *err;
	char **rep;
	int n_rows;
	int n_column;
	char sql[128]={0};
ERR1:
	recv(client_fd,&msg,sizeof(msg),0);
	sprintf(sql,"select * from staff where userid=%d and password=%d",msg.userid,msg.password);
	if(sqlite3_get_table(db,sql,&rep,&n_rows,&n_column,&err)!=SQLITE_OK) 
	//if(sqlite3_exec(db,sql,NULL,NULL,&err)!=SQLITE_OK)
	{
		send(client_fd,&ret,sizeof(ret),0);
		printf("sqlite3_exec error!");
		return;
	}
	if(n_rows==0)
	{
		printf("error!");
		send(client_fd,&ret,sizeof(ret),0);
		goto ERR1;
	}
	printf("欢迎%d                                 %s\n",msg.userid,my_time());
	ret=9;
	send(client_fd,&ret,sizeof(ret),0);
	return;
}
void admin_denglu(int client_fd,sqlite3 *db)
{
	message msg;
	int ret=0;
	char *err;
	char **rep;
	int n_rows;
	int n_column;
	char sql[128]={0};
ERR1:
	recv(client_fd,&msg,sizeof(msg),0);
	if(msg.userid!=1001)
	{
		ret=7;
		send(client_fd,&ret,sizeof(ret),0);
		return;
	}
	else
	{
		send(client_fd,&ret,sizeof(ret),0);
	}
	sprintf(sql,"select * from staff where userid=%d and password=%d",msg.userid,msg.password);
	if(sqlite3_get_table(db,sql,&rep,&n_rows,&n_column,&err)!=SQLITE_OK) 
	//if(sqlite3_exec(db,sql,NULL,NULL,&err)!=SQLITE_OK)
	{
		send(client_fd,&ret,sizeof(ret),0);
		printf("sqlite3_exec error!");
		return;
	}
	if(n_rows==0)
	{
		printf("error!");
		send(client_fd,&ret,sizeof(ret),0);
		goto ERR1;
	}
	printf("欢迎%d           %s\n",msg.userid,my_time());
	ret=9;
	send(client_fd,&ret,sizeof(ret),0);
	return;
}
void my_insert(int client_fd,sqlite3 *db)
{
	char *err;
	int ret;
	message msg;
	char sql[1024]={0};
	recv(client_fd,&msg,sizeof(msg),0);
	sprintf(sql,"insert into staff values(%d,'%s',%d,'%s',%f,'%s','%s',%d,'%s',%d)",msg.userid,\
			msg.name,msg.age,msg.sex,msg.salary,msg.depar,msg.address,msg.phone,msg.time,msg.password);
	printf("管理员增加了新员工，工号为%d                 %s\n",msg.userid,my_time());
	if(sqlite3_exec(db,sql,NULL,NULL,&err)!=SQLITE_OK)
	{
		printf("sqlite3_exec error!");
		return ;
	}
	ret=9;
	send(client_fd,&ret,sizeof(ret),0);
	return ;
}
void my_delete(int client_fd,sqlite3 *db)
{
	message msg;
	int ret;
	char *err;
	char sql[128];
	recv(client_fd,&msg,sizeof(msg),0);
	sprintf(sql,"delete from staff where userid==%d",msg.userid);
	printf("管理员删除了一个员工，其工号为%d             %s\n",msg.userid,my_time());
	if(sqlite3_exec(db,sql,NULL,NULL,&err)!=SQLITE_OK)
	{
		printf("sqlite3_exec error!\n");
		return;
	}
	ret=9;
	send(client_fd,&ret,sizeof(ret),0);
	return;

}
void my_update(int client_fd,sqlite3 * db)
{
	message msg;
	int ret;
	char *err;
	char sql[128];
	recv(client_fd,&ret,sizeof(ret),0);
	recv(client_fd,&msg,sizeof(msg),0);
	if(ret==1)
	{
		sprintf(sql,"update staff set wage=%f where userid=%d",msg.salary,msg.userid);
		printf("管理员修改了员工%d的个人工资              %s\n",msg.userid,my_time());
	}
	else
	{
		sprintf(sql,"update staff set department=%s where userid=%d",msg.depar,msg.userid);	
		printf("管理员修改了员工%d的部门                  %s\n",msg.userid,my_time());
	}
	if(sqlite3_exec(db,sql,NULL,NULL,&err)!=SQLITE_OK)
	{
		printf("sqlite3_exec error!");
		return;
	}
	ret=9;
	send(client_fd,&ret,sizeof(ret),0);
	return;
}
void my_select(int client_fd,sqlite3 *db)
{
	message msg;
	char buf[128];
	int i,j;
	int ret=0;
	char *err;
	char **rep;
	int n_rows;
	int n_column;
	char sql[128]={0};
	recv(client_fd,&msg,sizeof(msg),0);
	sprintf(sql,"select * from staff where userid=%d",msg.userid);
	if(sqlite3_get_table(db,sql,&rep,&n_rows,&n_column,&err)!=SQLITE_OK)
	{
		printf("error!");
		return;
	}
	if(n_rows==0)
	{
		send(client_fd,&ret,sizeof(ret),0);
		puts("查询失败！");
		return;
	}
	else
	{
		for(i=0;i<n_column;i++)
		{
			for(j=0;j<n_rows+1;j++)
			{
				strcpy(buf,*rep++);
				send(client_fd,buf,sizeof(buf),0);
			}
		}
		strcpy(buf,"over");
		send(client_fd,buf,sizeof(buf),0);
	}
}
void user_upatde(int client_fd,sqlite3 *db)
{
	message msg;
	int ret;
	char *err;
	char sql[128];
	recv(client_fd,&ret,sizeof(ret),0);
	recv(client_fd,&msg,sizeof(msg),0);
	if(ret==1)
	{
		sprintf(sql,"update staff set address=%s where userid=%d",msg.address,msg.userid);
		printf("员工%d修改了个人家庭住址                     %s\n",msg.userid,my_time());
	}
	else if(ret==2)
	{
		sprintf(sql,"update staff set phone=%d where userid=%d",msg.phone,msg.userid);
		printf("员工%d修改了个人手机号码                     %s\n",msg.userid,my_time());
	}
	else
	{
		sprintf(sql,"update staff set password=%d where userid=%d",msg.password,msg.userid);
		printf("员工%d修改了个人密码                         %s\n",msg.userid,my_time());
	}
	if(sqlite3_exec(db,sql,NULL,NULL,&err)!=SQLITE_OK)
	{
		printf("sqlite3_exec error!");
		return;
	}
	ret=9;
	send(client_fd,&ret,sizeof(ret),0);
	return;
}
void admin_select(int client_fd,sqlite3 *db)
{
	
	char buf[128];
	int i,j;
	int ret=0;
	char *err;
	char **rep;
	int n_rows;
	int n_column;
	char sql[128]={0};
	sprintf(sql,"select * from staff");
	if(sqlite3_get_table(db,sql,&rep,&n_rows,&n_column,&err)!=SQLITE_OK)
	{
		printf("error!");
		return;
	}
	if(n_rows==0)
	{
		send(client_fd,&ret,sizeof(ret),0);
		puts("查询失败！");
		return;
	}
	else
	{
		for(i=0;i<n_column;i++)
		{
			for(j=0;j<n_rows+1;j++)
			{
				strcpy(buf,*rep++);
				send(client_fd,buf,sizeof(buf),0);
			}
		}
		
		printf("管理员查询了所有成员                        %s\n",my_time());
		strcpy(buf,"over");
		send(client_fd,buf,sizeof(buf),0);
	}
}

int main(int argc, const char *argv[])
{
	int ret;
	int pid;
	int cmd;
	char buf[128];
	int server_fd,client_fd;
	struct sockaddr_in server_addr,client_addr;
 	int size=sizeof(client_addr);
	sqlite3 *db;
	if(sqlite3_open("./staff.db",&db)!=SQLITE_OK)
	{
		printf("sqlite3_open error!");
		return -1;
	}
	server_fd=socket(AF_INET,SOCK_STREAM,0);
	if(server_fd<0)
	{
		perror("socket error!");
		return -1;
	}
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(8888);
	server_addr.sin_addr.s_addr=inet_addr("192.168.1.109");
	ret=bind(server_fd,(struct sockaddr *)&server_addr,sizeof(server_addr));
	if(ret<0)
	{
		perror("bind error!");
		return -1;
	}
	ret=listen(server_fd,10);
	if(ret<0)
	{
		perror("listen error!");
		return -1;
	}
	while(1)
	{
		client_fd= accept(server_fd,(struct sockaddr*)&client_addr,&size);
		if(client_fd<0)
		{
			perror("accept error!");
			return -1;
		}
		pid=fork();
		if(pid<0)
		{
			perror("pid");
			return -1;
		}
		else if(pid ==0)
		{
			while(1)
			{
				if(recv(client_fd,&cmd,sizeof(cmd),0)>0);
				switch (cmd){
				case 1:admin_denglu(client_fd,db);
					   cmd=0;
					   break;
				case 2:denglu(client_fd,db);break;
				case 3:admin_select(client_fd,db);break;
				case 4:my_insert(client_fd,db);break;
				case 5:my_delete(client_fd,db);break;
				case 6:my_update(client_fd,db);break;
				case 8:my_select(client_fd,db);break;
				case 9:user_upatde(client_fd,db);break;
				}
			}
		}
	}
	close(server_fd);
	close(client_fd);
	return 0;
}


