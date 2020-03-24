#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

//#define __KDEBUG__

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::atoi;
using std::string;

#define BLOCK_SIZE_DEFAULT 10
#define FILENAME_DEFAULT "huge.txt"
#define OFILENAME_DEFAULT "filter.txt"

int main(int argc, char *argv[])
{
    int cBlockSize = BLOCK_SIZE_DEFAULT;
    bool cGetAverage = true; // false, gets last data
    string cFileName = FILENAME_DEFAULT;
    string cOFileName = OFILENAME_DEFAULT;

    // check parameters
    if(argc>1)
    {
        int loop=1;
        char main_arg = ' ';
        while(loop<argc)
        {
            if(main_arg != ' ') // get sub arg
            {
                switch(main_arg)
                {
                    case 'f':
                        cFileName = argv[loop];
                        break;
                    case 'o':
                        cOFileName = argv[loop];
                        break;
                    case 'b':
                        cBlockSize = atoi(argv[loop]);
                        if(!cBlockSize) cBlockSize = BLOCK_SIZE_DEFAULT;
                        break;
                }
                main_arg = ' ';
            }
            else if(strlen(argv[loop])==2&&argv[loop][0]=='-') // option!
            {
                switch(argv[loop][1])
                {
                    case 'b':
                    case 'f':
                    case 'o':
                    case 'h':
                        main_arg = argv[loop][1];
                        break;
                    case 'l':
                        cGetAverage = false;
                        break;
                    default:
                        cout << "Unknown command/option \""
                            << argv[loop] << "\".\n";
                }
            }
            else
            {
                cout << "Unknown parameter \"" << argv[loop] << "\".\n";
            }
            // check if help requested
            if(main_arg == 'h')
            {
                cout << "Data reduction program by kaz." << endl;
                cout << "Usage:" << endl;
                cout << "  " << argv[0] << "[-f <filename>] [-o <filename>] [-b size] [-l]" << endl;
                cout << "Options:" << endl;
                cout << "  -f <filename> => specify input file (default is " << FILENAME_DEFAULT << ")" << endl;
                cout << "  -o <filename> => specify output file (default is " << OFILENAME_DEFAULT << ")" << endl;
                cout << "  -b size       => specify block size" << endl;
                cout << "  -l            => take last in block instead of average" << endl;
                cout << endl;
                return 0;
            }
            loop++;
        }
    }

    // process
    cout << "Filtering large data file: " << cFileName << endl;
    cout << "*Parameters* " << endl;
    if(cGetAverage)
        cout << "Data reduction by Block Average." << endl;
    else
        cout << "Data reduction using last data in block." << endl;
    cout << "Block size: " << cBlockSize << endl;
    cout << "Output data file: " << cOFileName << endl;

    ifstream input(cFileName.c_str());
    ofstream output(cOFileName.c_str());
    if(!input.is_open()||!output.is_open())
    {
        if(input.is_open()) input.close();
        if(output.is_open()) output.close();
        cout << "Error opening files! Exiting!" << endl;
        return 1;
    }

#ifdef __KDEBUG__
    int dbg_count = 0;
#endif
    string label1,label2;
    double data1, data2, acc1, acc2;
    int count=0, icount=0, ocount=0;
    char buffer = 0x0;
    while(1)
    {
        buffer = input.get();
        if(buffer=='\t'||buffer==';')
            break;
        label1 += buffer;
    }
    //input >> label1;
    cout << "Label 1: " << label1 << endl;
    // second label
    buffer = 0x0;
    while(1)
    {
        buffer = input.get();
        if(buffer=='\t'||buffer==';'||buffer=='\n')
            break;
        label2 += buffer;
    }
    //input >> label2;
    cout << "Label 2: " << label2 << endl;
    // write output
    output << label1 << "\t" << label2 << endl;
    acc1 = 0.0; acc2 = 0.0;
    while(!input.eof())
    {
        input >> data1;
        input >> data2;
        icount++;
#ifdef __KDEBUG__
        if(dbg_count<cBlockSize*5)
        {
            cout << "Data1: " << data1 <<
                "\tData2: " << data2 << endl;
            dbg_count++;
        }
#endif
        acc1 += data1;
        acc2 += data2;
        count++;
        if(count==cBlockSize)
        {
            if(cGetAverage)
            {
                acc1 /= count; acc2 /= count;
                output << acc1 << "\t" << acc2 << endl;
            }
            else
            {
                output << data1 << "\t" << data2 << endl;
            }
            acc1 = 0.0; acc2 = 0.0;
            count = 0;
            ocount++;
        }
    }
    // remaining data
    if(count)
    {
        if(cGetAverage)
        {
            acc1 /= count; acc2 /= count;    ifstream input(cFileName.c_str());
    ofstream output(cOFileName.c_str());
    if(!input.is_open()||!output.is_open())
    {
        if(input.is_open()) input.close();
        if(output.is_open()) output.close();
        cout << "Error opening files! Exiting!" << endl;
        return 1;
    }

#ifdef __KDEBUG__
    int dbg_count = 0;
#endif
    string label1,label2;
    double data1, data2, acc1, acc2;
    int count=0, icount=0, ocount=0;
    char buffer = 0x0;
    while(1)
    {
        buffer = input.get();
        if(buffer=='\t'||buffer==';')
            break;
        label1 += buffer;
    }
    //input >> label1;
    cout << "Label 1: " << label1 << endl;
    // second label
    buffer = 0x0;
    while(1)
    {
        buffer = input.get();
        if(buffer=='\t'||buffer==';'||buffer=='\n')
            break;
        label2 += buffer;
    }
    //input >> label2;
    cout << "Label 2: " << label2 << endl;
    // write output
    output << label1 << "\t" << label2 << endl;
    acc1 = 0.0; acc2 = 0.0;
    while(!input.eof())
    {
        input >> data1;
        input >> data2;
        icount++;
#ifdef __KDEBUG__
        if(dbg_count<cBlockSize*5)
        {
            cout << "Data1: " << data1 <<
                "\tData2: " << data2 << endl;
            dbg_count++;
        }
#endif
        acc1 += data1;
        acc2 += data2;
        count++;
        if(count==cBlockSize)
        {
            if(cGetAverage)
            {
                acc1 /= count; acc2 /= count;
                output << acc1 << "\t" << acc2 << endl;
            }
            else
            {
                output << data1 << "\t" << data2 << endl;
            }
            acc1 = 0.0; acc2 = 0.0;
            count = 0;
            ocount++;
        }
    }
    // remaining data
    if(count)
    {
        if(cGetAverage)
        {
            acc1 /= count; acc2 /= count;
            output << acc1 << "\t" << acc2 << endl;
        }
        else
        {
            output << data1 << "\t" << data2 << endl;
        }
        ocount++;
    }
    cout << "Total Input Data: " << icount << endl;
    cout << "Total Output Data: " << ocount << endl;

    input.close();
    output.close();

            output << acc1 << "\t" << acc2 << endl;
        }
        else
        {
            output << data1 << "\t" << data2 << endl;
        }
        ocount++;
    }
    cout << "Total Input Data: " << icount << endl;
    cout << "Total Output Data: " << ocount << endl;

    input.close();
    output.close();

	return 0;
}
