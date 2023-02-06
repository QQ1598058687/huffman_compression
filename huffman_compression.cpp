//严蔚敏版本的huffman扩展为带压缩的功能(by 桔皮沙拉 qq1598058687)
#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <string>
#include <fstream>

#if defined(_WIN32)||defined(_WIN64)
#pragma warning(disable:4996)
#endif

//#define CHAR_4SIZE

#define COMPRESSION   //方法选择

using namespace std;

typedef struct HuffmanNode
{
	//包括父节点,左右孩子节点,编码,编码长度,字符长度,频率与字符
	int parent, left, right, code, codelen, runlen, freq;
	char symbol;
	HuffmanNode():parent(0),left(0),right(0),code(0), codelen(0),runlen(0),freq(0),symbol(' '){}
	HuffmanNode(int p, int l, int r, int c, int clen, int rlen,int f, char s) :
		parent(p),left(l), right(r), code(c), codelen(clen), runlen(rlen),freq(f), symbol(s) {}
	HuffmanNode(const HuffmanNode &r):parent(r.parent),
		left(r.left),right(r.right),code(r.code),codelen(r.codelen), runlen(r.runlen),freq(r.freq), symbol(r.symbol){}

	bool operator<(const HuffmanNode &r)const {
		return freq < r.freq;
	}
	bool operator==(const HuffmanNode &r)const {
		return symbol == r.symbol && runlen == r.runlen;
	}
	friend ostream& operator<<(ostream &os, const HuffmanNode &r) {
		os << "parent:" << r.parent << " left:" << r.left << " right:" << r.right << "  "; 
		string binCode = "";
		int val = r.code;
		for (int i = 0; i < r.codelen; ++i)
		{
			binCode += ((val & 1) + '0');
			val >>= 1;
		}
		binCode = string(binCode.rbegin(), binCode.rend());
		os << "<编码 "<< r.code << ": " << binCode << "> 编码长度:" << r.codelen << " 出现次数：" << r.runlen << " 字符:" << r.symbol;
		return os;
	}
}HuffmanNode;

class HuffmanCoding {
	static const int arr_size = 100;
	vector<HuffmanNode> hTree;
	int chars[256];
	char outFileNamePre[arr_size];
	char outFileName[arr_size];
	int dataSize,binarySize;
	//string buffer;

	fstream fIn,fOut;

public:
	friend ostream& operator<<(ostream &os, const HuffmanCoding &r) {
		for (size_t i = 0; i < r.hTree.size(); ++i)
		{
			os << i  << " " << r.hTree[i] << "\n";
		}
		return os;
	}
	HuffmanCoding() {
		for (int i = 0; i < 256; ++i)
		{
			chars[i] = 0;
		}
	}
	
	void toList(int p) {
		if (hTree[p].left == 0 && hTree[p].right == 0)
		{
			char code = hTree[p].symbol;
			hTree[p].right = chars[(unsigned char)code];
			chars[(unsigned char)code] = p;
		}
		else
		{
			toList(hTree[p].left);
			toList(hTree[p].right);
		}
	}
	
	void Select(int end,int &m1, int &m2) {
		int w1, w2;
		w1 = w2 = INT_MAX;

		for (int i = 1; i <= end; ++i)
		{
			if (hTree[i].parent != 0)continue;//如果处理过父节点则跳过
			//如果当前运行长度大于当前
			if (hTree[i].freq < w1)
			{
				//把运行的第二个长度等于1
				w2 = w1, m2 = m1;
				//获得当前长度与下标
				w1 = hTree[i].freq, m1 = i;
			}
			//如果当前大等于节点1并小于节点2的则获取第二个最小位置
			else if (hTree[i].freq >= w1 &&
				hTree[i].freq < w2) {
				w2 = hTree[i].freq;
				m2 = i;
			}
		}
	}
	void createHuffmanTree() {

		int hTreeCnt = dataSize * 2 - 1;

		hTree.resize(hTreeCnt + 1);

		int m1, m2;
		for (int i = dataSize + 1; i <= hTreeCnt; ++i)
		{
			Select(i - 1, m1, m2);
			//左右节点分别得到父节点
			hTree[m1].parent = hTree[m2].parent = i;
			//分配左右节点
			hTree[i].left = m1, hTree[i].right = m2;
			//计算总长
			hTree[i].freq = hTree[m1].freq + hTree[m2].freq;
		}

#ifdef COMPRESSION
		//从根节点往前遍历
		int f = 0, j;

		for (int i = 1; i <= dataSize; ++i)
		{
			int level = 0, code = 0;
			for (j = i, f = hTree[i].parent;
				f != 0;
				j = f, f = hTree[f].parent) {

				if (hTree[f].left == j)
				{
					level++;
				}
				else if (hTree[f].right == j) {
					code += (1 << level);//从尾部往前插入
					level++;
				}

			}
			hTree[i].code = code, hTree[i].codelen = level;
		}
#else
		//前序遍历
		int r = hTreeCnt , len = 0;

		auto H = hTree;

		for (auto &i:H)
		{
			i.runlen = 0;
		}

		int level = 0, code = 0;

		while (r != 0)
		{
			if (H[r].runlen == 0) {
				//标记为先往左走
				H[r].runlen = 1;

				if (H[r].left != 0) {
					r = H[r].left;
					level++;
					code <<= 1;
				}
				else if (H[r].right == 0) {

					hTree[r].code = code, hTree[r].codelen = level;
				}
			}
			else if (H[r].runlen == 1) {
				//标记为往右走
				H[r].runlen = 2;

				if (H[r].right != 0) {
					r = H[r].right;
					level++;
					code = (code << 1) + 1;
				}
			}
			else {
				r = H[r].parent;
				code >>= 1;
				level--;
			}
		}
#endif
	}

	void readNormalFile() {

		char ch, ch2;
		HuffmanNode r;
		vector<HuffmanNode>::iterator i;

		for (fIn.get(ch); !fIn.eof(); ch = ch2) {
			r.freq = 1;
			//不断读取下一个字符,统计出现长度(直到与ch不同或结束为止)
			for (r.runlen = 1, fIn.get(ch2); !fIn.eof() && ch2 == ch; r.runlen++)
				fIn.get(ch2);

			//buffer += ch;//用来写入编码的顺序
			r.symbol = ch;//存储字符

			i = find(hTree.begin(), hTree.end(), r);
			if (i == hTree.end())
				hTree.push_back(r);
			else
				i->freq++;

		}
		//排序后用来写入文件使用
	//	std::sort(hTree.begin(), hTree.end());
		//得到文件长度
		fIn.clear();

		dataSize = hTree.size();
		binarySize = (int)fIn.tellg();
		hTree.insert(hTree.begin(), HuffmanNode(0, 0, 0, 0,0, dataSize, binarySize, ' '));
	}
	
	int read4Byte() {
		int num = 0;
		for (int i = 0; i < sizeof(num); ++i)
		{
			num <<= 8;
			num += fIn.get();
		}
		return num;
	}
	void write4Byte(int num) {
		int bit_v = 32;
		for (size_t i = 0; i < sizeof(num); i++)
		{
			bit_v -= 8;
			//从左到右 xx xx xx xx的值
			fOut.put((num >> bit_v) & 0xff);
		}
	}
	void writeCompressionFile(const char *fileName) {

		memset(outFileName, 0, arr_size);
		int len = strlen(fileName);

		
		strcpy(outFileName, outFileNamePre);
		strcat(outFileName, fileName);

		if (strchr(outFileName, '.'))
		{
			strcpy(strchr(outFileName, '.') + 1, "bin");
		}
		else strcat(outFileName, ".bin");

		cout << outFileName << endl;

		fOut.open(outFileName, ios::out | ios::binary);
		//写入文件名长度
		write4Byte(len);
		//写入文件名
		fOut.write(fileName, len);
		//写入数据长度
		write4Byte(dataSize);
		//写入文件长度
		write4Byte(binarySize);

		//写入码表
		for (int i = 1; i < dataSize +1; ++i)
		{
			//分别写入字符，字符个数
			fOut.put(hTree[i].symbol);
#ifdef CHAR_4SIZE
			write4Byte(hTree[i].runlen);
#else
			fOut.put(hTree[i].runlen >> 8).put(hTree[i].runlen);
#endif
			write4Byte(hTree[i].freq);
		}


		//写入二进制编码顺序
		long packCnt = 0, maxPack = 32, pack = 0, bitsLeft = 0;
		//第一个为空节点
		char c, c2;
		int runlen = 1, p = 0;
		for (fIn.get(c);!fIn.eof();)
		{
			
			for (runlen = 1, fIn.get(c2); !fIn.eof() && c == c2; runlen++)
			{
				fIn.get(c2);
			}
			
			for (p = chars[(unsigned char)c];
				p != 0 && runlen != hTree[p].runlen;
				p = hTree[p].right)
			//	cout << runlen<<" "<<p<<" "<< hTree[p].runlen << endl;

			if (p == 0)
				cerr << "encode error " << endl;

			//最大的长度32位减计数器,取整
			if (hTree[p].codelen < maxPack - packCnt) {
				//把内容左移编码的长度+编码本身
				pack = (pack << hTree[p].codelen) | hTree[p].code;
				//计数器加编码长度，
				packCnt += hTree[p].codelen;
			}
			else
			{
				//当编码长度大于能用的长度时,得到一个最大值减计数器所需要的左移值
				bitsLeft = maxPack - packCnt;
				//(pack是带有原先的值)先把pack左移这个值
				pack <<= bitsLeft;
				//如果这两个不相等，那就是长度大于左移值
				if (hTree[p].codelen  > bitsLeft )
				{
					//得到编码
					int hold = hTree[p].code;
					//用编码长度减只能包含的长度，得到一个右移次数，把编码右移
					hold = hold >> (hTree[p].codelen - bitsLeft);
					//然后加上只有高位剩余的编码
					pack |= hold;
				}

				else//如果这两个相等，pack直接加编码
					pack |= hTree[p].code;

				//写入int整形内容
				write4Byte(pack);

				//如果大于bitsLeft
				if (hTree[p].codelen > bitsLeft)
				{
					//说明前面处理过，就直接获取编码
					pack = hTree[p].code;
					//获取最大32bit减编码减去(ht.codelen - bitsLeft)的多出来的剩余长度
				//	packCnt = maxPack - (ht.codelen - bitsLeft);
				//	cout << "1:"<<packCnt << endl;
					//获取移位后剩余的长度
					packCnt = hTree[p].codelen - bitsLeft; //cout << "2:" << packCnt << endl;
				}
				else
					packCnt = 0;

			}
			c = c2;
		}
		//当他不等于0时说明还有未存储完
		if (packCnt != 0) {
			pack = pack << (maxPack - packCnt);
			write4Byte(pack);
		}
		fOut.close();
	}
	void compress(const char *fileName) {
		fIn.open(fileName, ios::in | ios::binary);
		if (!fIn)
		{
			cerr << "没有 <" << fileName << "> 这个文件" << endl;
			getchar();
			return;
		}
		readNormalFile();
		

		createHuffmanTree();
		//从根节点开始
		fIn.seekg(0, ios::beg);
		toList(dataSize * 2 - 1);
		
		writeCompressionFile(fileName);

		fIn.close();

		cout << "压缩完成" << endl;
	}
	void readCompressionFile()
	{
		memset(outFileName, 0, arr_size);

		//读取文件名长度，文件名，数据个数，输出文件的长度
		int len = read4Byte();
		char tmp[arr_size]{ 0 };

		fIn.read(tmp, len);

		strcpy(outFileName, outFileNamePre);
		strcat(outFileName, tmp);//拷贝到输出文件
		strcpy(strchr(outFileName,'.'), "--解压文件");//再拷贝扩展内容
		strcat(outFileName, strchr(tmp, '.'));//连接尾部

		dataSize = read4Byte();
		binarySize = read4Byte();
		//获取节点内容
		HuffmanNode data;
		for (int i = 0; i < dataSize; ++i)
		{
			//每次读取一个字符
			data.symbol = fIn.get();
#ifdef CHAR_4SIZE
			data.runlen = read4Byte();
#else
			data.runlen = 0;
			data.runlen = fIn.get() << 8;
			data.runlen |= fIn.get();
#endif
			data.freq = read4Byte();
			hTree.push_back(data);
		}
		hTree.insert(hTree.begin(), HuffmanNode(0, 0, 0, 0,0, dataSize, binarySize, ' '));

	}
	void writeOutFile() {

		cout << outFileName << endl;
		fOut.open(outFileName, ios::out | ios::binary);
		char c;
		int chars = 0, m = dataSize * 2 - 1, bitCnt = 1;
		for (chars = 0, fIn.get(c); !fIn.eof() && chars < binarySize;)
		{
			for (int p = m;;)
			{
				if (hTree[p].left == 0 && hTree[p].right == 0)
				{
					for (int j = 0; j < hTree[p].runlen; ++j)
					{
						fOut.put(hTree[p].symbol);
					}
					chars += hTree[p].runlen;
					break;
				}
				else if ((c & 0x80) == 0)
					p = hTree[p].left;
				else
					p = hTree[p].right;
				if (bitCnt++ == CHAR_BIT) {
					fIn.get(c);
					bitCnt = 1;
				}
				else
					c <<= 1;
			}
		}

		fOut.close();
	}
	void decompress(const char *fileName) {
		fIn.open(fileName, ios::in | ios::binary);
		if (!fIn)
		{
			cerr << "没有 <"<<fileName<<"> 这个文件" << endl;
			getchar();
			return;
		}
		//读取压缩文件
		readCompressionFile();
		//创建树
		createHuffmanTree();
		writeOutFile();
		fIn.close();
		cout << "解压完成" << endl;
	}
	void GetFile(const char *fileName) {
		string s = fileName;
		int i;
		if ((i = s.find_last_of('\\')) != string::npos || (i = s.find_last_of('/')) != string::npos)
		{
			memset(outFileNamePre, 0, arr_size);
			memcpy(outFileNamePre, s.substr(0, i + 1).c_str(), s.substr(0, i + 1).size());
			s = s.substr(i + 1);

		}
		if (s.find(".bin") != string::npos)
		{
			decompress(s.c_str());
		}
		else
			compress(s.c_str());

	}
};

int main(int argc,char *argv[]){
//直接把文件拖进来,解压/或压缩
	if (argc >= 2)
	for (int i = 1; i < argc; ++i)
	{
		HuffmanCoding h, h2;
		h.GetFile(argv[i]);
		cout << "回车继续" << endl;
		//getchar();
	}
	else
	{
		cout << "没有文件!回车继续" << endl;
		getchar();
	}

}