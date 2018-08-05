#include<stdio.h>
#include<mysql/mysql.h>
#include"cgi_base.hpp"

int main(){
  //0.获取 query_string
  char buf[1024*4]={0};
  if(GetQueryString(buf)<0){
    fprintf(stderr,"[CGI]GetQueryString error\n");
    return 1;
  }

  //约定客户传过来的参数是：name=hehe&id=123
  int id=0;
  char name[1024]={0};
  sscanf(buf,"name=%s&id=%d",name,&id);
  fprintf(stderr,"[CGI]name:%s,id=%d",name,id);
  /////////////////////////////////////////////////////////////
  // 接下来进行有关数据库的操作
  // 直接把数据库的表中的数据都给查出来
  // mysql api  使用的一般思路：
  // 1.连接到数据库
  // 2.拼装sql语句
  // 3.把SQL语句发送到服务器
  // 4.向客户端返回插入结果
  // 5.断开连接
  /////////////////////////////////////////////////////////////
  
  
  //1.连接数据库
  //创建MYSQL句柄
  MYSQL* connect_fd = mysql_init(NULL);
  //连接句柄和数据库
  MYSQL* connect_ret=mysql_real_connect(connect_fd,"127.0.0.1","root","maomeng0501","test1",3306,NULL,0);

  if(NULL == connect_ret){
    fprintf(stderr,"mysql connect failed\n");
    return 1;
  }
  fprintf(stderr,"mysql connect ok!\n");

  //2.拼装SQL语句
  char sql[1024*4]={0};
  sprintf(sql,"insert into TestTable values(%d,\"%s\")",id,name);

  //3.把SQL语句发送到服务器上
  int ret = mysql_query(connect_fd,sql);
  if(ret<0){
    fprintf(stderr,"mysql_query failed!%s\n",sql);
  }

  //4.向客户端反馈插入结果是否成功
  if(ret == 0){
    printf("<html><h1>插入成功！</h1></html>");
  }else{
    printf("<html><h1>插入失败！</h1></html>");
  }

  //5.断开连接
  mysql_close(connect_fd);
  return 0;
  
}
