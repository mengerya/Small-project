#pragma once 


#include<stdio.h>
#include <stdlib.h>
#include<string.h>
#include<unistd.h>


//分GET和POST两种情况读取计算的参数
//1.GET从query_string中读取
//1.POST从body中读取
//读取的结果就放入缓冲区buf中

int GetQueryString(char buf[]){
  //1.从环境变量中读取的方法是什么

  char* method=getenv("REQUEST_METHOD");
  if(NULL == method){
    //当前的CGI程序对应的标准输出已被重定向到管道之中
    //且这部分数据会被返回到客户端
    //避免让程序内部的错误暴露给普通用户
    //可以通过stderr作为输入日志的手段
    fprintf(stderr,"method==NULL\n");
    return 0;
  }

  //2.判定方法是 GET，还是POST
  //如果是GET，就从环境变量里读取QUERY_STRING
  //如果是POST，就需要从环境变量里读取CONTENT_LENGTH
  //GET   /cgi-bin/test?a=10&b=20 HTTP/1.1

	//POST  /cgi-bin/test HTTP/1.1
	//header...
  //
  //a=10&b=20
  
  if(strcasecmp(method,"GET")==0){
    char* query_string=getenv("QUERY_STRING");
    if(query_string == NULL){
      fprintf(stderr,"query_string == NULL\n");
      return 0;
    }
    //拷贝完成后，buf里面的内容形如：a=10&b=20
    fprintf(stderr,"[CGI-GET]query_string:%s\n",query_string);
    strcpy(buf,query_string);
  }
  else{
    //POST
    char* content_length_str=getenv("CONTENT_LENGTH");
    if(NULL == content_length_str){
      fprintf(stderr,"content_length == NULL\n");
      return 0;
    }
    int content_length = atoi(content_length_str);
    int i=0;
    
    fprintf(stderr,"[CGI-POST]body");
    for(;i<content_length;++i){
      fprintf(stderr,"%c",buf[i]);
      read(0,&buf[i],1);
    }
    fprintf(stderr,"\n");
    buf[i]='\0';
    
  }
  return 1;
  
}
