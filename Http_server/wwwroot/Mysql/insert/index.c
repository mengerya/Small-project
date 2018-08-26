#include<stdio.h>
#include<fcntl.h>
#include<mysql/mysql.h>
#include"cgi_base.hpp"

//写成功的网页
void RenderHtml(){
  //1.打开文件
  int fd = open("./wwwroot/mysql/insert/index.html",O_RDONLY);
  if(fd<0){
    fprintf(stderr,"open failed!\n");
    return;
  }
  //2.逐个字节读取文件中的内容
  char c='\0';
  while(read(fd,&c,1)>0){
      //将读到的当前字符直接写入
      write(1,&c,1);
  }
  //3.关闭文件
  close(fd);
}


int main(){
   RenderHtml();
  

	return 0;
}
