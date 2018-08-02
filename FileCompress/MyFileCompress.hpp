#pragma once
#include"HuffManTree.hpp"
#include<string>
#include<assert.h>
#include<iostream>
#include<fstream>

using namespace std;


typedef class CharInfo{
public:
	CharInfo()
		:ch_count(0)
		, str_Code("")
	{}

	CharInfo(const CharInfo& c)
		:ch(c.ch)
		, ch_count(c.ch_count)
		, str_Code(c.str_Code)
	{}

	CharInfo(const char c)
		:ch(c)
		, ch_count(0)
		, str_Code("")
	{}
	bool operator!=(const CharInfo& info)
	{
		return ch_count != info.ch_count;
	}
	CharInfo operator+(const CharInfo& info)
	{
		CharInfo ret;
		ret.ch_count = ch_count + info.ch_count;
		return ret;
	}
	bool operator>(const CharInfo& info) const
	{
		return ch_count > info.ch_count;
	}
	bool operator<(const CharInfo& info) const
	{
		return ch_count < info.ch_count;
	}
	char ch;//字符信息
	long long ch_count;//字符出现的次数
	string str_Code;//字符的哈夫曼编码
}CharInfo;





#if 1


class CharCompress{
public:
	//初始化字符表
	CharCompress(){
		for (size_t i = 0; i < 256; ++i){
			_charInfo[i].ch = i;
		}
	}

	

	void CompressFile(const string& strFilePath){
		//1.找每个字符出现的次数--读文件
		FILE* fIn = fopen(strFilePath.c_str(), "r");
		assert(fIn);
		
		//每次从源文件读取1kb内容，将每个字符出现的次数记录在_charInfo数组中
		char *readbuf = new char[1024];
		while (1){
			size_t rdsize = fread(readbuf, 1, 1024, fIn);
			if (rdsize == 0)
				break;
			//计算每个字符出现的次数
			for (size_t i = 0; i < rdsize; ++i){

				_charInfo[(unsigned char)readbuf[i]].ch_count++;
			}
		}

		//2.以每个字符出现的次数为权值构建哈夫曼树
		HuffTree<CharInfo> hf(_charInfo, 256, CharInfo(0));

		//3.获取每个字符对应的哈夫曼编码
		_GetHuffCode(hf.GetRoot());
		//4.将解压信息先写入新文件（一共有多少行解压信息---为了与解压后的真正文件信息区分开、源文件格式后缀、出现的字符及出现的次数）

		//获得要压缩文件的后缀，写入压缩文件的第一行
		string Filename;
		string FilePosFix;
		FilePosFix += (GetFilePosFix(strFilePath));
		FilePosFix += "\n";


		cout << "是否压缩到指定文件？" << endl;
		cout << "1.是         2.否 " << endl;

		int select = 0;
		while (1){
			cin >> select;
			if (select == 1){
				char newfile[100] = { 0 };
				cout << "请输入要压缩的目录：" << endl;
				scanf("%s", newfile);
				Filename += newfile;
			}
		}
		Filename += (GetFileName(strFilePath));
		Filename += ".hfm";

		FILE* fOut = fopen(Filename.c_str(), "w");
		assert(fOut);
		fwrite(FilePosFix.c_str(), 1, FilePosFix.size(), fOut);
		string strHuffInfo;
		char szCount[32];
		long long count_str = 0;
		//将文件一共有几行，及哈夫曼树中存放的有效数字字符以及每个字符的个数写入压缩文件中
		for (size_t i = 0; i < 256; ++i){
			if (_charInfo[i].ch_count != 0){
				count_str++;
				strHuffInfo += _charInfo[i].ch;
				strHuffInfo += ",";
				_itoa(_charInfo[i].ch_count, szCount, 10);
				strHuffInfo += szCount;
				strHuffInfo += "\n";
			}
		}
		_itoa(count_str, szCount, 10);
		string strFileHead;
		strFileHead += szCount;
		strFileHead += "\n";
		strFileHead += strHuffInfo;
		fwrite(strFileHead.c_str(), 1, strHuffInfo.size() + 2, fOut);
		

		//5.按照字符编码重新改写文件
		/*FILE* fOut = fopen("output.txt", "w");*/

		char ch = 0;
		char* buf_out = new char[1024];
		int flag = 0;
		string str;
		int count = 0;

		fseek(fIn, 0, SEEK_SET);//将文件指针偏移到文件开始位置
		while (1){
			size_t rdsize = fread(readbuf, 1, 1024, fIn);
			if (rdsize == 0)
				break;
			char c_no = 0;
			for (size_t i = 0; i < rdsize; ++i){
				str = _charInfo[readbuf[i]].str_Code;
				for (size_t j = 0; j < str.size(); ++j){
					ch <<= 1;
					if (str[j] == '1'){
						ch |= 1;

					}
					flag++;
					if (flag == 8){
						buf_out[count++] = ch;
						if (1024 == count){
							/*size_t rdsize = fread(readbuf, 1, 1024, fIn);*/
							fwrite(buf_out, 1, 1024, fOut);
							count = 0;
						}
						ch = 0;
						flag = 0;
					}

				}

			}
		}
		if (flag<8)
			ch <<= (8 - flag);
		buf_out[count] = ch;
		fwrite(buf_out, 1, count, fOut);
		
		delete[] readbuf;
		delete[] buf_out;
		fclose(fIn);
		fclose(fOut);
	}
private:

	bool IsExist(const char* file)//判断文件是否存在
	{
		ifstream ifs(file);
		if (!ifs)
		{
			return false;//不存在
		}
		return true;//存在
	}

	string GetFilePosFix(string filepath){
		return filepath.substr(filepath.find_last_of('.'));
	}

	string GetFileName(string filepath){
		return filepath.substr(0, filepath.find_last_of('.'));
	}

	void _GetHuffCode(HuffNode<CharInfo>* & root){
		//以递归的形式来实现
		if (NULL == root){
			return;
		}
		_GetHuffCode(root->_pLeft);
		_GetHuffCode(root->_pRight);
		if (root->_pLeft == NULL && root->_pRight == NULL){
			//找到叶子节点
			//规定右为0，左为1
			HuffNode<CharInfo>* pCur = root;
			HuffNode<CharInfo>* pParent = pCur->_pParent;
			string& str = _charInfo[root->_weight.ch].str_Code;  //root->_weight.str_Code;
			while (pParent){
				if (pParent->_pLeft == pCur){
					str += '1';
				}
				else if (pParent->_pRight == pCur)
					str += '0';

				pCur = pParent;
				pParent = pCur->_pParent;
			}
		}
		return;
	}
private:
	CharInfo _charInfo[256];//所有字符的数组集合，数组里放的是自定义的CharInfo类型
};


void test(){
	CharCompress Charcom;

	Charcom.CompressFile("./input.txt");


}

#endif

