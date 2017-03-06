#pragma once
#include<string>
#include"Heap.hpp"
#include"Huffman.h"
#include<stdlib.h>
#include<assert.h>

struct FileInfo
{
	FileInfo()
	:_count(0)
	{}
	std::string _strCode; //字符编码
	char _ch;	//当前字符
	size_t _count; //字符出现的次数
	FileInfo operator+(const FileInfo&fileInfo)
	{
		FileInfo ret(*this);
		ret._count += fileInfo._count;
		return ret;
	}
	bool operator<(const FileInfo&fileInfo)const
	{
		return _count < fileInfo._count;
	}

	bool operator!=(const FileInfo&fileInfo)const
	{
		return _count != fileInfo._count;
	}
};


class FileCompress
{
public:

	FileCompress()
	{
		memset(_fileInfo, 0, sizeof(_fileInfo));
		for (size_t idx = 0; idx < 256; ++idx)
		{
			_fileInfo[idx]._count = 0;
			_fileInfo[idx]._ch = idx;
		}
	}

	FileCompress(const std::string &strFileName)
	{
		memset(_fileInfo, 0, sizeof(_fileInfo));
	}

	void FileCount(const std::string &strFileName)
	{
		//统计相同字符出现的次数
		FILE*fOut = fopen(strFileName.c_str(), "r");
		assert(fOut);
		char readBuff[1024];
		while (true)
		{
			size_t readSize = fread(readBuff, 1, 1024, fOut);
			if (0 == readSize)
				break;
			for (size_t idx = 0; idx < readSize; ++idx)
			{
				_fileInfo[readBuff[idx]]._ch = readBuff[idx];
				_fileInfo[readBuff[idx]]._count++;
			}
		}
	}

	//	while (true)
	//	{
	//		int ReadSize = fread(readBuff, 1, 1024, fOut);
	//		if (0 == ReadSize)
	//			break;
	//		char value = 0;
	//		for (int idx = 0; idx < ReadSize; ++idx)
	//		{
	//			//
	//			int pos = 0;
	//			std::string strCode = _fileInfo[readBuff[idx]]._strCode;
	//			for (int idx = 0; idx < strCode.length(); ++idx)
	//			{
	//				value <<= 1;
	//				if ('1' == strCode[idx])
	//					value |= 1;
	//				if (8 == ++pos)
	//				{

	//				}
	//			}



	//		}
	//	}


	//	fclose(fOut);
	//}

	

	void GenerateHuffmanCode()
	{
		FileInfo value;
		HuffmanTree<FileInfo> ht(_fileInfo,sizeof(_fileInfo) / sizeof(_fileInfo[0]), value);
		HuffmanTreeNode<FileInfo>* root = ht.GetRoot();
		_GenerateHuffmanCode(root);
	}

	void CompressFile(const std::string &strFileName)
	{
		FileCount(strFileName);
		FILE*fOut = fopen(strFileName.c_str(), "r");
		assert(fOut);
		char readBuff[1024];
		HuffmanTree<FileInfo> ht(_fileInfo, sizeof(_fileInfo) / sizeof(_fileInfo[0]), FileInfo());
		_GenerateHuffmanCode(ht.GetRoot());//ht.GetRoot;
		char wrBuff[1024];
		int writeSize = 0;
		char value = 0;
		int pos = 0;
	//	std::string compressFileName("1");
		std::string compressFileName;
		compressFileName = GetFileName(strFileName)+".hzp";
	//	compressFileName += ".hzp";
//		compressFileName = GetFileName("D:\\code\\文件压缩\\文件压缩\\1.txt");
		
		FILE*fIn = fopen(compressFileName.c_str() , "w");
		assert(fIn);
		//编码信息
		std::string strHead;
	//	strHead += GetFilePostFix(strFileName);
	//	strHead += GetFilePostFix(strFileName);
		strHead+=(".txt\n");
		size_t idx = 0;
		size_t lineCount = 0;
		std::string strCode;
		char strLineCount[34];
		while (idx<sizeof(_fileInfo)/sizeof(_fileInfo[0]))
		{
			
			if (_fileInfo[idx]._count)
			{
				strCode += _fileInfo[idx]._ch;
				strCode += ':';
				_itoa(_fileInfo[idx]._count, strLineCount, 10);
				strCode += strLineCount;
				strCode += '\n';
				lineCount++;
			}
			idx++;
		}
		//strLineCount.c_str();
		_itoa(lineCount, strLineCount, 10);
		strHead += strLineCount;
		strHead += '\n';
		strHead += strCode;
		fwrite(strHead.c_str(), 1, strHead.length(), fIn);
		fseek(fOut, 0, SEEK_SET);
		while (true)
		{
			int ReadSize = fread(readBuff, 1, 1024, fOut);
			if (0 == ReadSize)
				break;
			for (int idx = 0; idx < ReadSize; ++idx)
			{
				//
				std::string strCode = _fileInfo[readBuff[idx]]._strCode;
				for (int idx = 0; idx < strCode.length(); ++idx)
				{
					value <<= 1;
					if ('1' == strCode[idx])
						value |= 1;
					if (8 == ++pos)
					{
						wrBuff[writeSize++] = value;
						if (1024 == writeSize)
						{
							fwrite(wrBuff, 1, 1024, fIn);
							writeSize = 0;
						}
						value = 0;
						pos = 0;
					}
				}
			}
		}
		//处理---遇到问题
		if (pos < 8)
		{
			value <<= (8 - pos);
			wrBuff[writeSize++] = value;
			fwrite(wrBuff, 1, writeSize, fIn);
		}
		fclose(fOut);
		fclose(fIn);
	}
	

	void UnCompressFile(const std::string compressFileName)
	{
		//1.获取编码信息--验证.hzp
		FILE*fOut = fopen(compressFileName.c_str(), "r");
		assert(fOut);
		std::string strPostFix = ReadLine(fOut);
		int LineCount = atoi(ReadLine(fOut).c_str());
		size_t lineidx = 0;

		while (lineidx < LineCount)
		{
			//有问题
			std::string strCode = ReadLine(fOut);
			_fileInfo[strCode[0]]._count =atoi(strCode.substr(2).c_str());
			++lineidx;
		}

		std::string fileName("1");
		fileName += strPostFix;
		FILE*fIn = fopen(fileName.c_str(), "wb");
		//解压缩
		int pos = 8;
		HuffmanTree<FileInfo>ht(_fileInfo, sizeof(_fileInfo) / sizeof(_fileInfo[0]), FileInfo());
		HuffmanTreeNode<FileInfo>*pRoot = ht.GetRoot();
		HuffmanTreeNode<FileInfo>*pCur = ht.GetRoot();
		long long fileSize = pRoot->_weight._count;
		_GenerateHuffmanCode(pRoot);
		char rdBuff[1024];
		char wrBuff[1024];
		int wrSize = 0;
		while (true)
		{
			//读取压缩文件
			int rdSize=fread(rdBuff, 1, 1024, fOut);
			if (rdSize == 0)
				break;
			int idx = 0;
			for (idx = 0; idx < rdSize; ++idx)
			{
				while (idx<rdSize)
				{
					--pos;
					if (rdBuff[idx] & (1 << pos))
						pCur = pCur->_pRight;
					else
						pCur = pCur->_pLeft;

					if (NULL == pCur->_pLeft&&pCur->_pRight == NULL)
					{
						wrBuff[wrSize++] = pCur->_weight._ch;
						if (wrSize == 1024)
						{
							fwrite(wrBuff, 1, wrSize, fIn);
							wrSize = 0;
						}
						pCur = pRoot;
						if (0 == --fileSize)//问题，写了两次
						{
							fwrite(wrBuff, 1, wrSize, fIn);
							break;
						}
					}
					if (0 == pos)
					{
						pos = 8;
						break;
					}
					//没放满?
					
				}
				
				
			}
		}
		fclose(fOut);
		fclose(fIn);
	}


	std::string GetFilePostFix(const std::string strFilePath)
	{
		std::string FileName;
		int begin = strFilePath.find_last_of("\\");
		int end = strFilePath.find_last_of(".");
		
		FileName = strFilePath.substr(0,begin+1);
		return FileName;
	}
	std::string GetFileName(const std::string strFilePath)
	{
		std::string FileName;
		size_t begin = strFilePath.find_last_of("\\");
		size_t end = strFilePath.find_last_of (".");
		FileName = strFilePath.substr(begin+1, end-begin-1);
		return FileName;
	}
private:
	void _GenerateHuffmanCode(HuffmanTreeNode<FileInfo>*pRoot)
	{
		if (pRoot)
		{
			_GenerateHuffmanCode(pRoot->_pLeft);
			_GenerateHuffmanCode(pRoot->_pRight);

			HuffmanTreeNode<FileInfo>* pCur = pRoot;
			HuffmanTreeNode<FileInfo>* pParent = pCur->_pParent;
			std::string strCode;

			if (NULL == pCur->_pLeft&&NULL == pCur->_pRight)
			{
				while (pParent)
				{
					if (pParent->_pLeft == pCur)
						strCode += '0';
					if (pParent->_pRight == pCur)
						strCode += '1';
					pParent = pParent->_pParent;
					pCur = pCur->_pParent;
				}
				std::reverse(strCode.begin(), strCode.end());
				_fileInfo[pRoot->_weight._ch]._strCode = strCode;
			}
		}
	}
	//

	std::string ReadLine(FILE*fp)
	{
		
		std::string strLine; 
		if (feof(fp))//看文件是否是结尾
			return NULL;

		char c=fgetc(fp);
		while ('\n' != c)
		{
			strLine += c;
			if (feof(fp))//看文件是否是结尾
				return strLine;
			c=fgetc(fp);
		}
		return strLine;
	}
private:
	FileInfo _fileInfo[256];
};

