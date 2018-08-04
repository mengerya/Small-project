
#include<stdio.h>
#include<stdlib.h>
#include"cgi_base.hpp"

int main(){
  fprintf(stderr,"CGI-test.c\n");
  //1.调用封装好的函数，获取相应参数
  char buf[4*1024]={0};

  int ret = GetQueryString(buf);
  if(ret == 0){
    fprintf(stderr,"[CGI]GetQueryString failed\n");
    return 1;
  }

  //此时获取到的buf为：a=10&b=20
  int a,b;
  sscanf(buf,"a=%d&b=%d",&a,&b);
  fprintf(stderr,"[CGI-test]a=%d,b=%d\n",a,b);
  int sum = 0;
  sum=a+b;
  //printf输出的结果就会返回到客户端
  printf("<h1>sum=%d</h1>",sum);
  return 0;
}
