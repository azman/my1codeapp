//------------------------------------------------------------------------------
#include <iostream>
#include <fstream>
//------------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------------
#define MAXROW 30
#define MAXCOL 30
#define LIFECHAR '$'
//------------------------------------------------------------------------------
int main(void)
{
	char *cLifeCell = NULL; // actual storage
	char *cLif2Cell = NULL; // actual storage
	char *pCurrCell = NULL; // operations pointer - Current Gen Cell
	char *pNextCell = NULL; // operations pointer - Next Gen Cell
	char *pSwapCell = NULL; // operations pointer - Swap Pointer
	int cCellRow=0, cCellCol=0;
	bool cFlag = true;

	int GetCellIndex(int, int);
	void CalculateCells(char*, char*, int, int);
	void ViewCells(char*, int, int);

	cout << endl;
	do
	{
		cout << "Life Game\n---------" << endl;
		if(cLifeCell==NULL)
		{
			cout << "<C>reate Cells" << endl;
		}
		else
		{
			cout << "<D>elete Cells" << endl;
			cout << "<P>opulate Cell" << endl;
			cout << "<T>erminate Cell" << endl;
			cout << "<V>iew Cells" << endl;
			cout << "<L>oad Cells" << endl;
			cout << "<S>ave Cells" << endl;
			cout << "<G>enerations" << endl;
		}
		cout << "<E>xit Program" << endl << endl;
		cout << "Your choice: ";
		char cTemp;
		cin >> cTemp;
		cout << endl;

		int cTempRow, cTempCol;
		fstream cFile;
		char cBufferName[80];
		switch(cTemp)
		{
		case 'E':
		case 'e':
			cout << "Peace!";
			cFlag = false;
			break;
		case 'C':
		case 'c':
			if(cLifeCell==NULL)
			{
				// cell size has 2-MAXROW/MAXCOL limit
				do
				{
					cout << "Enter row count for cell(s): ";
					cin >> cTempRow;
				}
				while(cTempRow<2||cTempRow>MAXROW);
				do
				{
					cout << "Enter column count for cell(s): ";
					cin >> cTempCol;
				}
				while(cTempCol<2||cTempCol>MAXCOL);
				// create cells
				int cSize = cTempRow * cTempCol;
				cLifeCell = new char[cSize];
				if(cLifeCell)
				{
					for(int cLoop=0;cLoop<cSize;cLoop++)
						cLifeCell[cLoop] = ' ';
				}
				// nextgen cells
				cLif2Cell = new char[cSize];
				if(cLif2Cell)
				{
					for(int cLoop=0;cLoop<cSize;cLoop++)
						cLif2Cell[cLoop] = ' ';
				}
				// check if both cells OK
				if(!cLifeCell)
				{
					if(cLif2Cell)
					{
						delete[] cLif2Cell;
						cLif2Cell = NULL;
					}
					cout << "Error creating Life cells!" << endl;
				}
				else if(!cLif2Cell)
				{
					delete[] cLifeCell;
					cLifeCell = NULL;
					cout << "Error creating Life cells!" << endl;
				}
				else
				{
					// OK! set parameters
					pCurrCell = cLifeCell;
					pNextCell = cLif2Cell;
					cCellRow = cTempRow;
					cCellCol = cTempCol;
				}
			}
			break;
		case 'D':
		case 'd':
			if(cLifeCell!=NULL)
			{
				cout << "Are you sure you want to delete the cells (Y/N)? ";
				cin >> cTemp;
				if(cTemp=='Y'||cTemp=='y')
				{
					delete[] cLifeCell;
					cLifeCell = NULL;
					pCurrCell = NULL;
					delete[] cLif2Cell;
					cLif2Cell = NULL;
					pNextCell = NULL;
					cCellRow = 0;
					cCellCol = 0;
				}
			}
			break;
		case 'P':
		case 'p':
			if(cLifeCell!=NULL)
			{
				// populate cell
				pCurrCell[GetCellIndex(cCellRow,cCellCol)] = LIFECHAR;
				cout << "Cell populated!" << endl;
			}
			break;
		case 'T':
		case 't':
			if(cLifeCell!=NULL)
			{
				// terminate cell
				pCurrCell[GetCellIndex(cCellRow,cCellCol)] = ' ';
				cout << "Cell decimated!" << endl;
			}
			break;
		case 'V':
		case 'v':
			if(cLifeCell!=NULL)
			{
				ViewCells(pCurrCell,cCellRow,cCellCol);
			}
			break;
		case 'L':
		case 'l':
			if(cLifeCell!=NULL)
			{
				cout << "Filename: ";
				cin >> cBufferName;
				cFile.open(cBufferName,ios::in);
				if(cFile.is_open())
				{
					char cHeader[9];
					int cTempR, cTempC;
					char *pPoint = cHeader;
					cFile.read(pPoint,sizeof(cHeader));
					if(cHeader[0]=='K'&&cHeader[1]=='i'&&
						cHeader[2]=='n'&&cHeader[3]=='g'&&
						cHeader[4]=='s'&&cHeader[5]=='L'&&
						cHeader[6]=='i'&&cHeader[7]=='f'&&
						cHeader[8]=='e')
					{
						// header ok
						pPoint = (char*) &cTempR;
						cFile.read(pPoint,sizeof(int));
						pPoint = (char*) &cTempC;
						cFile.read(pPoint,sizeof(int));
						if(cTempR==cCellRow&&cTempC==cCellCol)
						{
							// size ok
							for(int cLoop1=0;cLoop1<cCellRow;cLoop1++)
							{
								for(int cLoop2=0;cLoop2<cCellCol;cLoop2++)
								{
									cFile.read(&pCurrCell[cLoop1*cCellCol+
										cLoop2],sizeof(char));
								}
							}
							cout << "Life Cells loaded!" << endl;
						}
						else
						{
							cout << "Incompatible size!" << endl;
							cout << "Row = " << cTempR << endl;
							cout << "Col = " << cTempC << endl;
						}
					}
					else
					{
						cout << "File format not recognized!" << endl;
					}
					cFile.close();
				}
				else
					cout << "Cannot open file!";
			}
			break;
		case 'S':
		case 's':
			if(cLifeCell!=NULL)
			{
				cout << "Filename: ";
				cin >> cBufferName;
				cFile.open(cBufferName,ios::out);
				if(cFile.is_open())
				{
					char cHeader[] = "KingsLife";
					cFile.write(cHeader,sizeof(cHeader));
					char *pPoint = (char*) &cCellRow;
					cFile.write(pPoint,sizeof(int));
					pPoint = (char*) &cCellCol;
					cFile.write(pPoint,sizeof(int));
					for(int cLoop1=0;cLoop1<cCellRow;cLoop1++)
					{
						for(int cLoop2=0;cLoop2<cCellCol;cLoop2++)
						{
							cFile.write(&pCurrCell[cLoop1*cCellCol+
								cLoop2],sizeof(char));
						}
					}
					cout << "Life Cells saved!" << endl;
					cFile.close();
				}
				else
					cout << "Cannot open file!";
			}
			break;
		case 'G':
		case 'g':
			if(cLifeCell!=NULL)
			{
				cout << "Enter number of generations to simulate: ";
				cin >> cTempRow;
				cout << endl;
				cout << "Current Generation: " << endl;
				ViewCells(pCurrCell, cCellRow, cCellCol);
				for(int cLoop=0;cLoop<cTempRow;cLoop++)
				{
					// option for break
					int cBuffer;
					cout << "<1> To Continue <2> To Stop" << endl;
					cout << "Your Choice: ";
					cin >> cBuffer;
					if(cBuffer==2)
						break;
					// calculate & show
					cout << endl << "Generation " << (cLoop+1) << ": " << endl;
					CalculateCells(pCurrCell, pNextCell, cCellRow,cCellCol);
					ViewCells(pNextCell, cCellRow, cCellCol);
					// swap for next
					pSwapCell = pNextCell;
					pNextCell = pCurrCell;
					pCurrCell = pSwapCell;
					cout << endl;
				}
			}
			break;
		default:
			cout << "Ekkk! Wrong answer!" << endl;
			break;
		}
		cout << endl;
	}
	while(cFlag);

	if(cLifeCell)
		delete[] cLifeCell;

	return 0;
}
//------------------------------------------------------------------------------
int GetCellIndex(int aRow, int aCol)
{
	int cTempRow, cTempCol;

	do
	{
		cout << "Enter row index: ";
		cin >> cTempRow;
	}
	while(cTempRow<0||cTempRow>=aRow);
	do
	{
		cout << "Enter column index: ";
		cin >> cTempCol;
	}
	while(cTempCol<0||cTempCol>=aCol);

	return cTempRow*aCol+cTempCol;
}
//------------------------------------------------------------------------------
void CalculateCells(char* aLife, char* aNext, int aRow, int aCol)
{
	for(int cLoopRow=0;cLoopRow<aRow;cLoopRow++)
	{
		for(int cLoopCol=0;cLoopCol<aCol;cLoopCol++)
		{
			// for each cell, check neighbours
			int cIndexRow = cLoopRow + 2;
			int cIndexCol = cLoopCol + 2;
			int cCount = 0;
			for(int cLoop1=cLoopRow-1;cLoop1<cIndexRow;cLoop1++)
			{
				for(int cLoop2=cLoopCol-1;cLoop2<cIndexCol;cLoop2++)
				{
					if(cLoop1==cLoopRow&&cLoop2==cLoopCol)
						continue;
					if(aLife[cLoop1*aCol+cLoop2]==LIFECHAR)
						cCount++;
				}
			}
			// copy current cell to next
			aNext[cLoopRow*aCol+cLoopCol]=aLife[cLoopRow*aCol+cLoopCol];
			// implement rules
			if(aLife[cLoopRow*aCol+cLoopCol]==LIFECHAR)
			{
				// death rule - crowd or lone
				// covers survival rule
				if(cCount>3||cCount<2)
					aNext[cLoopRow*aCol+cLoopCol]=' ';
			}
			else
			{
				// birth rule
				if(cCount==3)
					aNext[cLoopRow*aCol+cLoopCol]=LIFECHAR;
			}
		}
	}
}
//------------------------------------------------------------------------------
void ViewCells(char* aLife, int aRow, int aCol)
{
	for(int cLoop=0;cLoop<aRow;cLoop++)
	{
		// draw row border
		for(int cLoop1=0;cLoop1<aCol;cLoop1++)
		{
			cout << "+-";
		}
		cout << "+" << endl;
		// draw row cells + border
		for(int cLoop1=0;cLoop1<aCol;cLoop1++)
		{
			cout << "|" << (char) aLife[cLoop*aCol+cLoop1];
		}
		cout << "|" << endl;
	}
	// draw row border
	for(int cLoop1=0;cLoop1<aCol;cLoop1++)
	{
		cout << "+-";
	}
	cout << "+" << endl;
}
//------------------------------------------------------------------------------
