#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
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

void my_user_update(int);
void my_insert(int client_fd)
{
	message msg;
	int ret;
ERR2:
	printf("请输入id：");
	scanf("%d",&msg.userid);
	puts("");
	
	printf("请输入姓名：");
	scanf("%s",msg.name);
	puts("");
	
	printf("请输入年龄：");
	scanf("%d",&msg.age);
	puts("");
	
	printf("请输入性别：");
	scanf("%s",msg.sex);
	puts("");
	
	printf("请输入工资：");
	scanf("%f",&msg.salary);
	puts("");
	
	printf("请输入部门：");
	scanf("%s",msg.depar);
	puts("");
	
	printf("请输入家庭住址：");
	scanf("%s",msg.address);
	puts("");
	
	printf("请输入电话：");
	scanf("%d",&msg.phone);
	puts("");
	
	printf("请输入入职时间：");
	scanf("%s",msg.time);
	puts("");
	
	printf("请输入初始密码：");
	scanf("%d",&msg.password);
	puts("");
	send(client_fd,&msg,sizeof(msg),0);
	recv(client_fd,&ret,sizeof(ret),0);
	if(ret!=9)
	{
		printf("error\n");
		goto ERR2;
	}
	puts("增加成功！");
}
void my_delete(int client_fd)
{
	int ret;
	message msg;
ERR3:
	printf("请输入离职人员工号：");
	scanf("%d",&msg.userid);
	send(client_fd,&msg,sizeof(msg),0);
	recv(client_fd,&ret,sizeof(ret),0);
	if(ret!=9)
	{
		printf("请重试！");
		goto ERR3;
	}
	return;
}
void my_update(int client_fd)
{
	int ret;
	int cmd;
	message msg;
ERR4:
	printf("请输入需要修改的工号：");
	scanf("%d",&msg.userid);
	puts("请输入需要修改信息的名称：1.工资2.部门3.退出");
	scanf("%d",&cmd);
	switch (cmd){
	case 1:
		printf("请输入修改后工资：");
		scanf("%f",&msg.salary);
		break;
	case 2:
		printf("请输入修改后部门：");
		scanf("%s",msg.depar);
		break;
	case 3:
		return;
	}
	send(client_fd,&cmd,sizeof(cmd),0);
	send(client_fd,&msg,sizeof(msg),0);
	recv(client_fd,&ret,sizeof(ret),0);
	if(ret!=9)
	{
		printf("错误，请重试！");
		goto ERR4;
	}
	puts("修改成功！");
}
void admin(int client_fd)
{
	int ret=0;
	int i=0;
	char buf[128];
ERR1:
	puts("*************亲爱的1001管理员，您好！***************");
	message msg;
	int cmd;
	printf("请输入用户名：");
	scanf("%d",&msg.userid);
	puts("");
	printf("请输入密码：");
	scanf("%d",&msg.password);
	send(client_fd,&msg,sizeof(msg),0);

	recv(client_fd,&ret,sizeof(ret),0);
	if(ret==7)
	{
		printf("您不是管理员！\n");
		return;
	}
	recv(client_fd,&ret,sizeof(ret),0);
	printf("%d\n",ret);
	if(ret!=9)
	{
		printf("账号或密码错误,请重新输入！\n");
		goto ERR1;
	}
	puts("登录成功！");
	puts("*******************************************************************");
	puts("***3.查询**4.增加员工****5.删除信息****6.修改员工信息*****7.退出***");
	puts("*******************************************************************");
	scanf("%d",&cmd);
	switch (cmd){
	case 3:
		send(client_fd,&cmd,sizeof(cmd),0);
		while(1)
		{
			i++;
			recv(client_fd,buf,sizeof(buf),0);
			if(strncmp(buf,"over",4)==0)
			{
				break;
			}
			printf("%-15s",buf);
			if(i%10==0)
			{
				puts("");
			}
		}
		break;
	case 4:
		send(client_fd,&cmd,sizeof(cmd),0);
		my_insert(client_fd);
		break;
	case 5:
		send(client_fd,&cmd,sizeof(cmd),0);
		my_delete(client_fd);
		break;
	case 6:
		send(client_fd,&cmd,sizeof(cmd),0);
		my_update(client_fd);
		break;
	case 7:
		return;
	}
}

void the_user(int client_fd)
{	
	int ret=0;
	int i=0;
	char buf[128];
ERR1:
	puts("您好！");
	message msg;
	int cmd;
	printf("请输入用户名：");
	scanf("%d",&msg.userid);
	puts("");
	printf("请输入密码：");
	scanf("%d",&msg.password);
	send(client_fd,&msg,sizeof(msg),0);
	recv(client_fd,&ret,sizeof(ret),0);
	if(ret!=9)
	{
		printf("账号或密码错误,请重新输入！\n");
		goto ERR1;
	}
	puts("登录成功！");
	printf("*******************员工%d,您好!*******************\n",msg.userid);
	puts("*******************************************************************");
	puts("*******8.查看个人信息********9.修改个人信息**********10.退出********");
	puts("*******************************************************************");
ERR5:
	scanf("%d",&cmd);
	switch (cmd){
	case 8:
		send(client_fd,&cmd,sizeof(cmd),0);
		send(client_fd,&msg,sizeof(msg),0);
		if(ret!=9)
		{
			puts("查询失败！");
			goto ERR5;
		}
		while(1)
		{
			recv(client_fd,buf,sizeof(buf),0);
			i++;
			if(strncmp(buf,"over",4)==0)
			{
				break;
			}
			printf("%-15s",buf);
			if(i%10==0)
			{
				putchar(10);
			}
		}
		puts("");
		break;
	case 9:
		send(client_fd,&cmd,sizeof(cmd),0);
		my_user_update(client_fd);
		break;
	case 10:
		return;
	}
}
void my_user_update(int client_fd)
{
	
	int ret;
	int cmd;
	message msg;
	printf("请输入需要修改的工号：");
	scanf("%d",&msg.userid);
	puts("请输入需要修改信息的名称：1.家庭住址2.电话3.密码4.退出");
	scanf("%d",&cmd);
	send(client_fd,&cmd,sizeof(cmd),0);
	switch (cmd){
	case 1:
		printf("请输入新地址：");
		scanf("%s",msg.address);
		break;
	case 2:
		printf("请输入新电话号：");
		scanf("%d",&msg.phone);
		break;
	case 3:
		printf("请输入新的密码：");
		scanf("%d",&msg.password);
		break;
	case 4:
		return;
	}
	send(client_fd,&msg,sizeof(msg),0);
	recv(client_fd,&ret,sizeof(ret),0);
	if(ret!=9)
	{
		printf("修改失败!");
		return;
	}
	puts("修改成功！");
	return;
}

int main(int argc, const char *argv[])
{
	int ret;
	int cmd;
	int client_fd;
	struct sockaddr_in server_addr;
	int size=sizeof(server_addr);
	client_fd=socket(AF_INET,SOCK_STREAM,0);
	if(client_fd<0)
	{
		perror("socket error!");
		return -1;
	}
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(8888);
	server_addr.sin_addr.s_addr=inet_addr("192.168.1.109");
	ret=connect(client_fd,(struct sockaddr*)&server_addr,size);
	if(ret<0)
	{
		perror("connect error!");
		return -1;
	}
	while(1)
	{
		puts("*********************************************************");
		puts("****1.管理员模式****2.用户模式********3.退出*************");
		puts("**:******************************************************");
		printf("请输入需要输入的数字：");
		scanf("%d",&cmd);
		switch (cmd){
		case 1:send(client_fd,&cmd,sizeof(cmd),0);
			   admin(client_fd);
			   break;
		case 2:send(client_fd,&cmd,sizeof(cmd),0);
			   the_user(client_fd);
			   break;
	/*	case 3:
	*/	
		case 3:
			   return;
		}
		 
	}
	close(client_fd);
	return 0;
}
