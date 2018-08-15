#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<mysql/mysql.h>
#include"url_m.hpp"
#include"cgi_base.hpp"

/*/mysql/insert/insert/index.html?id=12344&name=毛&sex=女&number=12345678902&math=99&chinese=99&english=99*/



//写成功的网页
void RenderHtml(char* res){
  //1.打开 select.html文件
  int fd = open("./wwwroot/mysql/insert/insert.html",O_RDONLY);
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
      //将读到的当前字符直接写入
      write(1,&c,1);
    }
  }
  //3.关闭文件
  close(fd);
}

void arrinit(char buf[],int size){
  int i=0;
  for(i=0;i<size;++i){
    buf[i]=0;
  }
}


int main(){
	//1.获取到query_string
  char query_string[1024*4]={0};
  int ret = GetQueryString(query_string);
  if(ret == 0){
    fprintf(stderr,"GetQueryString error!\n");
    return 1;
  }
  /*?id=12344&name=毛&sex=女&number=12345678902&math=99&chinese=99&english=99*/
  unsigned long long id=0;
  char name[128]={0};
  char sex[10]={0};
  unsigned long long number=0;
  int math=0;
  int chinese=0;
  int english=0;
  //sscanf(query_string,"id=%llu&name=%s&sex=%s&number=%llu&math=%d&chinese=%d&english=%d",&id,name,sex,&number,&math,&chinese,&english);

  //切分query_string
  char* p=query_string;
  int count = 0;
  char buf[256]={0};
  if(*p != '\0'){
    if(*p == 'i' && *(p+1)=='d' && *(p+2) == '='){
      p+=3;
      while(*p != '&'){
        buf[count++]=*p;
        p++;
      }
      id=atoll(buf);
    }
    p+=6;
    count=0;
    while(*p != '&'){
      name[count++]=*p;
      p++;
    }
    p+=5;
    count=0;
    while(*p != '&'){
      sex[count++]=*p;
      p++;
    }
    p+=8;
    arrinit(buf,256);
    count=0;
    while(*p != '&'){
      buf[count++]=*p;
      p++;
    }
    number=atoll(buf);
    count=0;
    arrinit(buf,256);
    p+=6;
    while(*p != '&'){
      buf[count++]=*p;
      p++;
    }
    math=atoll(buf);
    count=0;
    arrinit(buf,256);
    p+=9;
    while(*p != '&'){
      buf[count++]=*p;
      p++;
    }
    chinese=atoll(buf);
    count=0;
    arrinit(buf,256);
    p+=9;
    while(*p !='\0'){
      buf[count++]=*p;
      p++;
    }
    english=atoll(buf);
  }

  /***********DEBUG**************/
  fprintf(stderr,"id=%llu,name=%s,sex=%s,number=%llu,math=%d,chinese=%d,english=%d\n",id,name,sex,number,math,chinese,english);
  /******************************/
  
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
  sprintf(sql,"insert into student values(%llu,\"%s\",\"%s\",%llu,%d,%d,%d)",id,name,sex,number,math,chinese,english);

  /***************************DEBUG****************************/
  fprintf(stderr,"sql语句：%s\n",sql);
  /************************************************************/

  //4.将SQL语句返回给服务器
  ret = mysql_query(connect_fd,sql);
  if(ret < 0){
    fprintf(stderr,"mysql_query failed!\n");
    mysql_close(connect_fd);
    return 1;
  }


  mysql_close(connect_fd);
  char success[256]={"插入成功，可以返回上一页面查看!"};
  RenderHtml(success);

	return 0;
}
