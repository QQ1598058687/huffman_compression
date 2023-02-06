//��ε���汾��huffman��չΪ��ѹ���Ĺ���(by ��Ƥɳ�� qq1598058687)
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

#define COMPRESSION   //����ѡ��

using namespace std;

typedef struct HuffmanNode
{
	//�������ڵ�,���Һ��ӽڵ�,����,���볤��,�ַ�����,Ƶ�����ַ�
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
		os << "<���� "<< r.code << ": " << binCode << "> ���볤��:" << r.codelen << " ���ִ�����" << r.runlen << " �ַ�:" << r.symbol;
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
			if (hTree[i].parent != 0)continue;//�����������ڵ�������
			//�����ǰ���г��ȴ��ڵ�ǰ
			if (hTree[i].freq < w1)
			{
				//�����еĵڶ������ȵ���1
				w2 = w1, m2 = m1;
				//��õ�ǰ�������±�
				w1 = hTree[i].freq, m1 = i;
			}
			//�����ǰ����ڽڵ�1��С�ڽڵ�2�����ȡ�ڶ�����Сλ��
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
			//���ҽڵ�ֱ�õ����ڵ�
			hTree[m1].parent = hTree[m2].parent = i;
			//�������ҽڵ�
			hTree[i].left = m1, hTree[i].right = m2;
			//�����ܳ�
			hTree[i].freq = hTree[m1].freq + hTree[m2].freq;
		}

#ifdef COMPRESSION
		//�Ӹ��ڵ���ǰ����
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
					code += (1 << level);//��β����ǰ����
					level++;
				}

			}
			hTree[i].code = code, hTree[i].codelen = level;
		}
#else
		//ǰ�����
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
				//���Ϊ��������
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
				//���Ϊ������
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
			//���϶�ȡ��һ���ַ�,ͳ�Ƴ��ֳ���(ֱ����ch��ͬ�����Ϊֹ)
			for (r.runlen = 1, fIn.get(ch2); !fIn.eof() && ch2 == ch; r.runlen++)
				fIn.get(ch2);

			//buffer += ch;//����д������˳��
			r.symbol = ch;//�洢�ַ�

			i = find(hTree.begin(), hTree.end(), r);
			if (i == hTree.end())
				hTree.push_back(r);
			else
				i->freq++;

		}
		//���������д���ļ�ʹ��
	//	std::sort(hTree.begin(), hTree.end());
		//�õ��ļ�����
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
			//������ xx xx xx xx��ֵ
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
		//д���ļ�������
		write4Byte(len);
		//д���ļ���
		fOut.write(fileName, len);
		//д�����ݳ���
		write4Byte(dataSize);
		//д���ļ�����
		write4Byte(binarySize);

		//д�����
		for (int i = 1; i < dataSize +1; ++i)
		{
			//�ֱ�д���ַ����ַ�����
			fOut.put(hTree[i].symbol);
#ifdef CHAR_4SIZE
			write4Byte(hTree[i].runlen);
#else
			fOut.put(hTree[i].runlen >> 8).put(hTree[i].runlen);
#endif
			write4Byte(hTree[i].freq);
		}


		//д������Ʊ���˳��
		long packCnt = 0, maxPack = 32, pack = 0, bitsLeft = 0;
		//��һ��Ϊ�սڵ�
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

			//���ĳ���32λ��������,ȡ��
			if (hTree[p].codelen < maxPack - packCnt) {
				//���������Ʊ���ĳ���+���뱾��
				pack = (pack << hTree[p].codelen) | hTree[p].code;
				//�������ӱ��볤�ȣ�
				packCnt += hTree[p].codelen;
			}
			else
			{
				//�����볤�ȴ������õĳ���ʱ,�õ�һ�����ֵ������������Ҫ������ֵ
				bitsLeft = maxPack - packCnt;
				//(pack�Ǵ���ԭ�ȵ�ֵ)�Ȱ�pack�������ֵ
				pack <<= bitsLeft;
				//�������������ȣ��Ǿ��ǳ��ȴ�������ֵ
				if (hTree[p].codelen  > bitsLeft )
				{
					//�õ�����
					int hold = hTree[p].code;
					//�ñ��볤�ȼ�ֻ�ܰ����ĳ��ȣ��õ�һ�����ƴ������ѱ�������
					hold = hold >> (hTree[p].codelen - bitsLeft);
					//Ȼ�����ֻ�и�λʣ��ı���
					pack |= hold;
				}

				else//�����������ȣ�packֱ�Ӽӱ���
					pack |= hTree[p].code;

				//д��int��������
				write4Byte(pack);

				//�������bitsLeft
				if (hTree[p].codelen > bitsLeft)
				{
					//˵��ǰ�洦�������ֱ�ӻ�ȡ����
					pack = hTree[p].code;
					//��ȡ���32bit�������ȥ(ht.codelen - bitsLeft)�Ķ������ʣ�೤��
				//	packCnt = maxPack - (ht.codelen - bitsLeft);
				//	cout << "1:"<<packCnt << endl;
					//��ȡ��λ��ʣ��ĳ���
					packCnt = hTree[p].codelen - bitsLeft; //cout << "2:" << packCnt << endl;
				}
				else
					packCnt = 0;

			}
			c = c2;
		}
		//����������0ʱ˵������δ�洢��
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
			cerr << "û�� <" << fileName << "> ����ļ�" << endl;
			getchar();
			return;
		}
		readNormalFile();
		

		createHuffmanTree();
		//�Ӹ��ڵ㿪ʼ
		fIn.seekg(0, ios::beg);
		toList(dataSize * 2 - 1);
		
		writeCompressionFile(fileName);

		fIn.close();

		cout << "ѹ�����" << endl;
	}
	void readCompressionFile()
	{
		memset(outFileName, 0, arr_size);

		//��ȡ�ļ������ȣ��ļ��������ݸ���������ļ��ĳ���
		int len = read4Byte();
		char tmp[arr_size]{ 0 };

		fIn.read(tmp, len);

		strcpy(outFileName, outFileNamePre);
		strcat(outFileName, tmp);//����������ļ�
		strcpy(strchr(outFileName,'.'), "--��ѹ�ļ�");//�ٿ�����չ����
		strcat(outFileName, strchr(tmp, '.'));//����β��

		dataSize = read4Byte();
		binarySize = read4Byte();
		//��ȡ�ڵ�����
		HuffmanNode data;
		for (int i = 0; i < dataSize; ++i)
		{
			//ÿ�ζ�ȡһ���ַ�
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
			cerr << "û�� <"<<fileName<<"> ����ļ�" << endl;
			getchar();
			return;
		}
		//��ȡѹ���ļ�
		readCompressionFile();
		//������
		createHuffmanTree();
		writeOutFile();
		fIn.close();
		cout << "��ѹ���" << endl;
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
//ֱ�Ӱ��ļ��Ͻ���,��ѹ/��ѹ��
	if (argc >= 2)
	for (int i = 1; i < argc; ++i)
	{
		HuffmanCoding h, h2;
		h.GetFile(argv[i]);
		cout << "�س�����" << endl;
		//getchar();
	}
	else
	{
		cout << "û���ļ�!�س�����" << endl;
		getchar();
	}

}