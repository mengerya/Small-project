#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<mysql/mysql.h>
#include"url_m.hpp"
#include"cgi_base.hpp"
#include<sys/types.h>
#include<regex.h>

/*/mysql/insert/insert/index.html?name=41509030127&password=nisndna*/

//将llu类型转换成字符串
void my_lltoa(char id[],unsigned long long name){
  sprintf(id,"%llu",name);
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
  int fd = open("./wwwroot/mysql/register/register.html",O_RDONLY);
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


int main(){
	//1.获取到query_string
  char query_string[1024*4]={0};
  int ret = GetQueryString(query_string);
  if(ret == 0){
    fprintf(stderr,"GetQueryString error!\n");
    return 1;
  }
  /*?num=41509030127&password=snodnosdn*/
  unsigned long long name=0;
  char password[128]={0};
  char id[12]={0};
  memset(id,0x00,sizeof(id));
  //切分query_string
  sscanf(query_string,"num=%llu&password=%s",&name,password);

  /***********DEBUG**************/
  fprintf(stderr,"name:%llu,password=%s\n",name,password);
  /******************************/

  //将llu类型转换成字符串
  my_lltoa(id,name);
  /*******************DEBUG**********************/
  fprintf(stderr,"[my_lltoi]id:%s,name:%llu",id,name);
  /**********************************************/ 

  //判定输入的用户名是否符合规则
  int status_name=0;
  char pattern_name[1024]="^[1-9][0-9]{10}$";
  status_name=my_match(pattern_name,id);
  if(status_name == REG_NOMATCH){
    char res[1024]="<h3>用户名不符合规则，请返回首页重新输入11位首位非0的数字，谢谢配合^-^</h3>";
    RenderHtml(res);
    return 1;
  }

  //判断输入的密码是否符合规则
  int status_password=0;
  char pattern_password[1024]="^[a-z][a-z0-9]{4,15}$";
  status_password=my_match(pattern_password,password);
  if(status_password==REG_NOMATCH){
    char res[1024]="<h3>密码不符合规则，请返回首页重新输入5~16位首位为小写字母其他位为小写字母或数字，谢谢配合^-^</h3>";
    RenderHtml(res);
    return 1;
  }
  if(0 != status_name || 0 != status_password){
    char res[1024]="<h3>您的命名不符合规则，请返回首页重新注册</h3>";
    RenderHtml(res);
    return 1;
  }
  fprintf(stderr,"正则匹配成功\n");
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

  //检查要注册的用户账号在数据库teacher表中是否已经存在
  //若存在，则返回错误页面
  //若不存在，则继续注册账户
  //拼装判断账户是否存在首位SQL语句
  char sql_id[1024]={0};
  sprintf(sql_id,"select * from teacher where id=%llu",name);
  
  ret = mysql_query(connect_fd,sql_id);
  if(ret < 0){
    fprintf(stderr,"[select]mysql_query failed!\n");
    mysql_close(connect_fd);
    return 1;
  }

  //读取服务器返回的结果
  MYSQL_RES * result=mysql_store_result(connect_fd);
  if(NULL == result){
    fprintf(stderr,"mysql_store_result failed!\n");
    mysql_close(connect_fd);
    return 1;
  }

  //获取到具体的元素值
  MYSQL_ROW row = mysql_fetch_row(result);
  if(NULL != row){
    char Res[1024]="<h3>该用户已经存在，请返回主页面登录^-^</h3>";
    RenderHtml(Res);
    mysql_close(connect_fd);
    return 1;
  }


  //3.拼装SQL语句
  char sql[1024*4]={0};
  sprintf(sql,"insert into teacher values(%llu,\"%s\")",name,password);

  /***************************DEBUG****************************/
  fprintf(stderr,"sql语句：%s\n",sql);
  /************************************************************/

  //4.将SQL语句返回给服务器
  ret = mysql_query(connect_fd,sql);
  if(ret < 0){
    fprintf(stderr,"[insert]mysql_query failed!\n");
    mysql_close(connect_fd);
    return 1;
  }


  mysql_close(connect_fd);
  char success[256]={"注册成功，可以返回上一页面登录!"};
  RenderHtml(success);

	return 0;
}
