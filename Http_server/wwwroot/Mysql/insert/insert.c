#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<mysql/mysql.h>
#include"url_m.hpp"
#include"cgi_base.hpp"
#include<sys/types.h>
#include<regex.h>


/*/mysql/insert/insert/index.html?id=12344&name=毛&sex=女&number=12345678902&math=99&chinese=99&english=99*/

void my_lltoa(char id[],unsigned long long name){
  sprintf(id,"%llu",name);
}



void my_itoa(char id[],int name){
  sprintf(id,"%d",name);
}


int my_match(char* pattern,char* buf){
  int status,i;
  int flag=REG_EXTENDED;
  regmatch_t pmatch[1];
  const size_t nmatch=1;
  regex_t  reg;
  //编译正则模式
  regcomp(&reg,pattern,flag);
  //执行正则表达式和缓存的比较
  status=regexec(&reg,buf,nmatch,pmatch,0);
  //打印匹配的字符串
  /**********************DEBUG***************************/
  fprintf(stderr,"匹配的字符串：");
  for(i=pmatch[0].rm_so;i<pmatch[0].rm_eo;++i){
    fprintf(stderr,"%c",buf[i]);
  }
  fprintf(stderr,"\n");
  /*******************************************************/ 
  regfree(&reg);
  return status;
}


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
  
  //判断各项是否符合规则
  
  //id
  char id_s[6]={0};
  char pattern_id[128]="^[1-9][0-9]{4}$";
  int status_id=0;
  my_lltoa(id_s,id);
  status_id=my_match(pattern_id,id_s);
  if(status_id == REG_NOMATCH){
    char Res[256]="<h3>输入ID错误，请返回主页面，重新输入首位非0的5位整数</h3>";
    RenderHtml(Res);
    return 1;
  }

  //name
  fprintf(stderr,"pattern_name will going\n");
  char pattern_name[128]="^[%][\%a-zA-Z0-9]{0,59}$";
  int status_name=0;
  status_name=my_match(pattern_name,name);
  if(status_name==REG_NOMATCH){
    char Res[256]="<h3>您输入的名字错误，请返回主页面，重新输入6个以内汉字</h3>";
    RenderHtml(Res);
    return 1;
  }
  fprintf(stderr,"pattern_name OK\n");

  //sex
  if((strcmp(sex,"\%E5\%A5\%B3")!=0) && (strcmp(sex,"\%E7\%94\%B7")!=0)){
    char Res[256]="<h3>您输入的性别有误，请返回主页面，重新输入男/女</h3>";
    RenderHtml(Res);
    return 1;
  }

  //number
  char pattern_number[128]="^1[3578][0-9]{9}$";
  char number_s[12]={0};
  int status_number=0;
  my_lltoa(number_s,number);
  status_number=my_match(pattern_number,number_s);
  if(status_number == REG_NOMATCH){
    char Res[128]="<h3>您输入的手机号码格式错误，请返回主页面，输入正确的中国大陆地区手机号码</h3>";
    RenderHtml(Res);
    return 1;
  }

  /*?id=12344&name=毛&sex=女&number=12345678902&math=99&chinese=99&english=99*/
  //math
  if(math<0||math>100){
    char Res[128]="<h3>您输入的math成绩格式有误，请返回主页，输入正确的0~100内的整数</h3>";
    RenderHtml(Res);
    return 1;
  }

  //chinese
  if(chinese<0||chinese>100){
    char Res[128]="<h3>您输入的chinese成绩格式有误，请返回主页，输入正确的0~100内的整数</h3>";
    RenderHtml(Res);
    return 1;
  }
  
  //english
  if(english<0||english>100){
    char Res[128]="<h3>您输入的english成绩格式有误，请返回主页，输入正确的0~100内的整数</h3>";
    RenderHtml(Res);
    return 1;
  }

  if(status_id != 0 || status_name != 0 || status_number != 0){
    char Res[128]="<h3>您的输入格式错误，请返回主页重新输入</h3>";
    RenderHtml(Res);
    return 1;
  }


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

