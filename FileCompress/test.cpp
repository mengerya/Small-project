#include"FileCompress.hpp"


void menu(){
	cout << "*****************************************************************" << endl;
	cout << "****                       文件压缩                          ****" << endl;
	cout << "****                                                         ****" << endl;
	cout << "****         1.压缩文件                    2.解压缩           ****" << endl;
	cout << "****                                                         ****" << endl;
	cout << "*****************************************************************" << endl;
}

void test(){
	menu();
	int input = 0;
	CharCompress comfile;
	switch (input)
	{
	case 1:
	{
			  cout << "请输入你要压缩文件的完整路径：" << endl;
			  char filepath[100] = { 0 };
			  scanf("%s", filepath);
			  comfile.CompressFile(filepath);
			  break;
	}
	case 2:{
			   cout << "请输入你要解压缩文件的完整路径：" << endl;
			   char filepath2[100] = { 0 };
			   scanf("%s", filepath2);
			   comfile.unCompressFile(filepath2);
			   break;
	}
	default:
		printf("输入错误，请重新输入！\n");
		break;
	}
}

int main(){
	test();
	return 0;
}
