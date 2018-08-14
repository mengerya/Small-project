#include<stdio.h>
#include<fcntl.h>
#include<mysql/mysql.h>
#include"cgi_base.hpp"
/*192.168.17.129:9090/mysql/mysql/select.html?num=41509030127&password=1272727*/

//写成功的网页
void RenderHtml(unsigned long long id){
  //1.打开 select.html文件
  int fd = open("./wwwroot/mysql/select.html",O_RDONLY);
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
      printf("%llu",id);
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
  //query_string 形如num=41509030127&password=1272727
  unsigned long long id=0;
  char password[24]={0};
  sscanf(query_string,"num=%llu&password=%s",&id,password);

  /***********DEBUG**************/
  fprintf(stderr,"num=%llu,password=%s\n",id,password);
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
  sprintf(sql,"select password from teacher where id = '%llu'",id);

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
  //5.读取服务器返回的结构
  MYSQL_RES * result = mysql_store_result(connect_fd);
  if(NULL == result){
    fprintf(stderr,"mysql_store_result failed!\n");
    mysql_close(connect_fd);
    return 1;
  }

  //6.获取到服务器中对应用户名所对应的密码
  char password_sql[100]={0};
  //获取到元素的具体值
  MYSQL_ROW row = mysql_fetch_row(result);
  if(NULL == row){
    printf("<html>");
    printf("<h1>mysql 查询失败，用户不存在，请重新尝试！</h1><br>");
    printf("</html>");
    mysql_close(connect_fd);
    return 1;
  }

  strcpy(password_sql,row[0]);
  /************************DEBUG****************************/
  fprintf(stderr,"password_sql:%s",password_sql);
  /*********************************************************/

  mysql_close(connect_fd);
  //判断用户输入密码是否正确
  if(strcmp(password,password_sql) == 0){
    RenderHtml(id);
  }else{
    printf("<html>");
    printf("<h1>用户密码输入错误，请重新输入！</h1>");
    printf("</html>");
  }
  

	return 0;
}
