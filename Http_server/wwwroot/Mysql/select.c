#include<stdio.h>
#include<mysql/mysql.h>
#include"cgi_base.hpp"
/*192.168.17.129:9090/mysql/mysql/select.html?num=41509030127&password=1272727*/

int main(){
	//1.获取到query_string
  char query_string[1024*4]={0};
  int ret = GetQueryString(query_string);
  if(ret == 0){
    fprintf(stderr,"GetQueryString error!\n");
    return 1;
  }
  //query_string 形如num=41509030127&password=1272727
  long int num=0;
  char password[24]={0};
  sscanf(query_string,"num=%ld&password=%s",&num,password);
  //将用户名与密码在数据库中进行查找配对，若成功则进入学生管理系统，若失败则返回失败页面
  //2.连接数据库
  //创建MYSQL句柄
  MYSQL* connect_fd = mysql_init(NULL);
  //连接句柄和数据库
  MYSQL* connect_ret = mysql_real_connect(connect_fd,"127.0.0.1","root","maomeng0501","school",3306,NULL,0);

  if(NULL== connect_ret){
    fprintf(stderr,"mysql_real_connect failed!\n");
    return 1;
  }
  fprintf(stderr,"mysql connect ok!\n");

  //3.拼装SQL语句
  char sql[1024*4]={0};
  sprintf(sql,"select password from teacher where num='%ld'",num);

  //4.将SQL语句返回给服务器
  ret = mysql_query(connect_fd,sql);
  if(ret != 0){
    printf("用户不存在，请重新刷新页面！\n");
    mysql_close(connect_fd);
    return 1;
  }
  //5.读取服务器返回的结构
  MYSQL_RES* result = mysql_store_result(connect_fd);
  if(NULL == result){
    fprintf(stderr,"mysql_store_result failed!\n");
    mysql_close(connect_fd);
    return 1;
  }

  //6.获取到服务器中对应用户名所对应的密码
  

	return 0;
}
