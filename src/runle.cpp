//---------------------------------------------------------------------------
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
typedef unsigned char byte;
//---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	// function to strip filename from full path on win32
	char* StripFileName(char* aString,bool aNoExtension=false);

	cout << endl;
	if(argc!=4)
	{
		cout << "File Compression (Simple Run Length Encoding)." <<
			endl << endl;
		StripFileName(argv[0],true);
		cout << "Usage: " << argv[0] << " <commmand>"
			<< " <source file> <target file>" << endl << endl;
		cout << "Command:" << endl;
		cout << "c - Compress" << endl;
		cout << "d - Decompress" << endl;
	}
	else
	{
		bool cCompressThis = false;
		bool cDeCompressThis = false;
		int cCount = strlen(argv[1]);
		for(int cLoop=0;cLoop<cCount;cLoop++)
		{
			char cTest = tolower(argv[1][cLoop]);
			if(cTest=='c')
			{
				if(!cDeCompressThis)
					cCompressThis = true;
			}
			else if(cTest=='d')
			{
				if(!cCompressThis)
					cDeCompressThis = true;
			}
		}
		fstream cFile1(argv[2],ios::in|ios::binary);
		fstream cFile2(argv[3],ios::out|ios::binary);
		if(cFile1.is_open()&&cFile2.is_open())
		{
			// source file info basic_istream basic_ostream
			StripFileName(argv[2]);
			cFile1.seekg(0,ios::end);
			int cSize1 = cFile1.tellg();
			cout << "File 1: " << argv[2] << " [" << dec << cSize1
				<< " byte(s)]" << endl;
			cFile1.seekg(0,ios::beg);
			bool cShowFile2 = true;

			if(cCompressThis)
			{
				// start compression
				cout << "Compressing...";
				// method 1: maximum 128 non-identical
				byte cTempBytes[128];
				byte cCompressByte;
				cTempBytes[0] = (byte) cFile1.get();
				int cTempIndex = 1;
				bool cCopyMode = true;
				for(int cLoop=1;cLoop<cSize1;cLoop++)
				{
					byte cTest1 = (byte) cFile1.get();
					if(cCopyMode)
					{
						int cIndex = cTempIndex-1;
						if(cTest1!=cTempBytes[cIndex])
						{
							// keep on copying
							if(cTempIndex==128)
							{
								// write out if buffer is full
								cFile2.put(0x7f);
								for(int cLoop1=0;cLoop1<128;cLoop1++)
								{
									cFile2.put(cTempBytes[cLoop1]);
								}
								cTempIndex = 0;
							}
							cTempBytes[cTempIndex++] = cTest1;
						}
						else
						{
							// change to compress mode
							cCopyMode = false;
							if(cTempIndex>1)
							{
								// write out prev non-identical
								// cIndex = cTempIndex - 1
								byte cTempBuff = (byte) (cIndex-1);
								cFile2.put(cTempBuff);
								for(int cLoop1=0;cLoop1<cIndex;cLoop1++)
								{
									cFile2.put(cTempBytes[cLoop1]);
								}
							}
							cCompressByte = cTempBytes[cIndex];
							cTempIndex = 2;
						}
					}
					else // compress mode @ NOT copy mode
					{
						if(cTest1==cCompressByte)
						{
							if(cTempIndex==129)
							{
								// write out if buffer is full
								cFile2.put(0xff);
								cFile2.put(cCompressByte);
								cTempIndex = 0;
								cTempBytes[0] = cTest1;
								// back to copy mode
								cCopyMode = true;
							}
							cTempIndex++;
						}
						else
						{
							// write out compressed data
							byte cTempBuff = (byte)(cTempIndex-2);
							cTempBuff |= 0x80; // compress flag
							cFile2.put(cTempBuff);
							cFile2.put(cCompressByte);
							cTempBytes[0] = cTest1;
							cTempIndex = 1;
							// back to copy mode
							cCopyMode = true;
						}
					}
				}
				// write out remaining
				if(cTempIndex)
				{
					if(cCopyMode)
					{
						byte cTempBuff = (byte)(cTempIndex-1);
						cFile2.put(cTempBuff);
						for(int cLoop1=0;cLoop1<cTempIndex;cLoop1++)
						{
							cFile2.put(cTempBytes[cLoop1]);
						}
					}
					else
					{
						byte cTempBuff = (byte)(cTempIndex-2);
						cTempBuff |= 0x80; // compress flag
						cFile2.put(cTempBuff);
						cFile2.put(cTempBytes[0]);
					}
				}
				cout << " done." << endl;
			}
			else if(cDeCompressThis)
			{
				// start decompression
				cout << "Decompressing...";
				// method 1: maximum 128 non-identical
				bool cFirstByte = true;
				bool cCopyMode;
				int cNumberOfBytes;
				for(int cLoop=0;cLoop<cSize1;cLoop++)
				{
					byte cTest1 = (byte) cFile1.get();
					if(cFirstByte)
					{
						cNumberOfBytes = cTest1 & 0x7f;
						if(cTest1&0x80)
						{
							cCopyMode = false;
							cNumberOfBytes+=2;
						}
						else
						{
							cCopyMode = true;
							cNumberOfBytes++;
						}
						cFirstByte = false;
					}
					else if(cCopyMode)
					{
						cFile2.put(cTest1);
						cNumberOfBytes--;
						if(cNumberOfBytes==0)
							cFirstByte = true;
					}
					else // compressed mode @ NOT cCopyMode
					{
						while(cNumberOfBytes>0)
						{
							cFile2.put(cTest1);
							cNumberOfBytes--;
						}
						cFirstByte = true;
					}
				}
				cout << " done." << endl;
			}
			else
			{
				cShowFile2 = false;
				cout << "No command found." << endl;
			}

			if(cShowFile2)
			{
				// source file info
				StripFileName(argv[3]);
				int cSize2 = cFile2.tellp();
				cout << "File 2: " << argv[3] << " [" << dec << cSize2
					<< " byte(s)]" << endl;
			}
		}
		else
		{
			cout << "Error opening files!" << endl;
		}

		if(cFile1.is_open())
			cFile1.close();
		if(cFile2.is_open())
			cFile2.close();
	}
	return 0;
}
//---------------------------------------------------------------------------
char* StripFileName(char* aString,bool aNoExtension)
{
	int cIndex = -1;
	int cSize = -1;
	int cIndexDot = -1;
	int cCount = 0;
	while(aString[cCount]!='\0')
		cCount++;
	for(int cLoop=cCount-1;cLoop>=0;cLoop--)
	{
		if(aString[cLoop]=='.')
		{
			if(cIndexDot<0)
				cIndexDot = cLoop;
		}
		if(aString[cLoop]=='\\')
		{
			cIndex = cLoop + 1;
			cSize = cCount - cIndex;
			if(aNoExtension&&cIndexDot>=0)
				cSize -= cCount - cIndexDot;
			break;
		}
	}
	if(cIndex>=0)
	{
		for(int cLoop=0;cLoop<cSize;cLoop++)
		{
			aString[cLoop] = aString[cLoop+cIndex];
		}
		aString[cSize] = '\0';
	}
	return aString;
}
//---------------------------------------------------------------------------
