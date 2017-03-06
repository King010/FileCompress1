#define _CRT_SECURE_NO_WARNINGS 1
#include<iostream>
#include"Huffman.h"
#include"Heap.hpp"
#include"FileCompress.h"
using namespace std;

void Funtest()
{
	FileCompress fr;
	fr.CompressFile("D:\\code\\文件压缩\\文件压缩\\1.txt");
	fr.UnCompressFile("1.hzp");
}



int main()
{
	Funtest();
	system("pause");
	return 0;
}