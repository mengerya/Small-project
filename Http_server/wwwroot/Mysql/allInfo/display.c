#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<mysql/mysql.h>
#include"url_m.hpp"
#include"cgi_base.hpp"




//写成功的网页
void RenderHtml(char* res){
  //1.打开 select.html文件
  int fd = open("./wwwroot/mysql/allInfo/display.html",O_RDONLY);
  if(fd<0){
    fprintf(stderr,"open failed!\n");
    return;
  }
  //2.逐个字节读取文件中的内容
  char c='\0';
  while(read(fd,&c,1)>0){
    //判定当前位置是否为特殊字符位置
    //如果是，在该位置写入要填写的信息
    if(c == '\1'){
      printf("%s",res);
      fflush(stdout);
    }
    else{
      //将读到的当前字���直接写入
      write(1,&c,1);
    }
  }
  //3.关闭文件
  close(fd);
}



int main(){
  //0.获取 query_string
  char buf[1024*4]={0};
  if(GetQueryString(buf)<0){
    fprintf(stderr,"[CGI]GetQueryString error\n");
    return 1;
  }
  /////////////////////////////////////////////////////////////
  // 接下来进行有关数据库的操作
  // 直接把数据库的表中的数据都给查出来
  // mysql api  使用的一般思路：
  // 1.连接到数据库
  // 2.拼装sql语句
  // 3.把SQL语句发送到服务器
  // 4.读取并遍历服务器返回的结果
  // 5.断开连接
  /////////////////////////////////////////////////////////////
  
  
  //1.连接数据库
  //创建MYSQL句柄
  MYSQL* connect_fd = mysql_init(NULL);
  //连接句柄和数据库
  MYSQL* connect_ret=mysql_real_connect(connect_fd,"127.0.0.1","root","maomeng0501","school",3306,NULL,0);

  if(NULL == connect_ret){
    fprintf(stderr,"mysql connect failed\n");
    return 1;
  }
  fprintf(stderr,"mysql connect ok!\n");

  //2.拼装SQL语句
  const char* sql = "select * from student";

  //3.把SQL语句发送到服务器上
  int ret = mysql_query(connect_fd,sql);
  if(ret<0){
    fprintf(stderr,"mysql_query failed!%s\n",sql);
    return 1;
  }

  //4.读取并遍历服务器返回的结果
  MYSQL_RES* result = mysql_store_result(connect_fd);
  if(NULL == result){
    fprintf(stderr,"[CGI]mysql_store_result failed!\n");
    return 1;
  }
  char res[1024*10]={0};
  //a)获取到表里面有几行几列
  int rows=mysql_num_rows(result);
  int fields=mysql_num_fields(result);
  //b)获取到结果集合的表结构
  MYSQL_FIELD* field = mysql_fetch_field(result);
  while(field != NULL){
    strcat(res,field->name);
    strcat(res,"    ");
    field = mysql_fetch_field(result);
  }
  strcat(res,"<br>");
  //c)获取到每个元素的具体值
  int i=0;
  for(;i<rows;++i){
    MYSQL_ROW row=mysql_fetch_row(result);
    int j=0;
    for(;j<fields;++j){
      if(j==1 || j==2){
        urldecode(row[j]);
      }
      strcat(res,row[j]);
      strcat(res,"    ");
    }
    strcat(res,"<br>");
  }
  strcat(res,"<br>");

  /*********************DEBUG***********************/
  fprintf(stderr,"[display]result:%s\n",res);
  /*************************************************/ 
  RenderHtml(res);
  //5.断开连接
  mysql_close(connect_fd);
  return 0;
  
}
