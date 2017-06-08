#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <iostream>
#include <string>
#include <fstream>

//#define __KDEBUG__
#define BLOCK_SIZE_DEFAULT 10
#define IFILENAME_DEFAULT "huge.txt"
#define OFILENAME_DEFAULT "filter.txt"

struct GObject
{
  Fl_Counter* pBlockSize;
  Fl_Check_Button* pGetAverage;
  Fl_Check_Button* pGetLabel;
  Fl_Check_Button* pGet3Data;
  Fl_Input* pIFileName;
  Fl_Input* pOFileName;
  Fl_File_Chooser* pFileChooser;
  int mTag;
};

void incall(Fl_Widget* widget,void* data)
{
  GObject* pObjects = (GObject*) data;
  Fl_File_Chooser *pick = pObjects->pFileChooser;
  pObjects->mTag = 0;
  pick->value(pObjects->pIFileName->value());
  pick->show();
  while(pick->visible()) Fl::wait();
}

void opcall(Fl_Widget* widget,void* data)
{
  GObject* pObjects = (GObject*) data;
  Fl_File_Chooser *pick = pObjects->pFileChooser;
  pObjects->mTag = 1;
  pick->value(pObjects->pOFileName->value());
  pick->type(Fl_File_Chooser::CREATE);
  pick->show();
  while(pick->visible()) Fl::wait();
}

void fc_callback(Fl_File_Chooser *fc,	void* data)
{
  GObject* pObjects = (GObject*) data;
  if(!pObjects->mTag)
  {
    pObjects->pIFileName->value(fc->value());
  }
  else
  {
    pObjects->pOFileName->value(fc->value());
  }
}

void bt_callback(Fl_Widget*, void* userdata)
{
  GObject* pObjects = (GObject*) userdata;

  int cBlockSize;
  bool cGetAverage, cGetLabel, cGet3Data;
  std::string cIFileName;
  std::string cOFileName;

  cBlockSize = (int) pObjects->pBlockSize->value();
  if(pObjects->pGetAverage->value()) cGetAverage = false;
  else cGetAverage = true;
  if(pObjects->pGetLabel->value()) cGetLabel = true;
  else cGetLabel = false;
  if(pObjects->pGet3Data->value()) cGet3Data = true;
  else cGet3Data = false;
  cIFileName = pObjects->pIFileName->value();
  cOFileName = pObjects->pOFileName->value();

  // process
  std::cout << "Filtering large data file: " << cIFileName << std::endl;
  std::cout << "*Parameters* " << std::endl;
  if(cGetAverage)
      std::cout << "Data reduction by Block Average." << std::endl;
  else
      std::cout << "Data reduction using last data in block." << std::endl;
  if(cGetLabel)
      std::cout << "First row treated as label." << std::endl;
  else
      std::cout << "First row treated as data." << std::endl;
  if(cGet3Data)
      std::cout << "3-column data." << std::endl;
  else
      std::cout << "2-column data." << std::endl;
  std::cout << "Block size: " << cBlockSize << std::endl;
  std::cout << "Output data file: " << cOFileName << std::endl;

  std::ifstream input(cIFileName.c_str());
  std::ofstream output(cOFileName.c_str());

  if(!input.is_open()||!output.is_open())
  {
    if(input.is_open()) input.close();
    if(output.is_open()) output.close();
    std::cout << "Error opening files! Exiting!" << std::endl;
    exit(1);
  }

#ifdef __KDEBUG__
  int dbg_count = 0;
#endif
  std::string label1,label2,label3;
  double data1, data2, data3, acc1, acc2, acc3;
  int count=0, icount=0, ocount=0;
  char buffer = 0x0;
  if(cGetLabel)
  {
    // get first label
    while(1)
    {
      buffer = input.get();
      if(buffer=='\t')
        break;
      label1 += buffer;
    }
    //input >> label1;
    std::cout << "Label 1: " << label1 << std::endl;
    // second label
    buffer = 0x0;
    while(1)
    {
      buffer = input.get();
      if(buffer=='\t'||buffer=='\n')
        break;
      label2 += buffer;
    }
    //input >> label2;
    std::cout << "Label 2: " << label2 << std::endl;
    if(cGet3Data)
    {
    // third label
    buffer = 0x0;
    while(1)
    {
      buffer = input.get();
      if(buffer=='\t'||buffer=='\n')
        break;
      label3 += buffer;
    }
    //input >> label3;
    std::cout << "Label 3: " << label3 << std::endl;
    }
    // write output
    output << label1 << "\t" << label2 << "\t" << label3 << std::endl;
  }
  acc1 = 0.0; acc2 = 0.0; acc3 = 0.0;
  while(!input.eof())
  {
    input >> data1;
    input >> data2;
    if(cGet3Data) input >> data3;
    else data3 = 0.0;
    icount++;
#ifdef __KDEBUG__
    if(dbg_count<cBlockSize*5)
    {
      std::cout << "Data1: " << data1 <<
        "\tData2: " << data2 <<
        "\tData3: " << data3 << std::endl;
      dbg_count++;
    }
#endif
    acc1 += data1;
    acc2 += data2;
    acc3 += data3;
    count++;
    if(count==cBlockSize)
    {
      if(cGetAverage)
      {
        acc1 /= count; acc2 /= count; acc3 /= count;
        output << acc1 << "\t" << acc2;
        if(cGet3Data) output << "\t" << acc3;
        output << std::endl;
      }
      else
      {
        output << data1 << "\t" << data2;
        if(cGet3Data) output << "\t" << data3;
        output << std::endl;
      }
      acc1 = 0.0; acc2 = 0.0; acc3 = 0.0;
      count = 0;
      ocount++;
    }
  }
  // remaining data
  if(count)
  {
    if(cGetAverage)
    {
      acc1 /= count; acc2 /= count; acc3 /= count;
      output << acc1 << "\t" << acc2;
      if(cGet3Data) output << "\t" << acc3;
      output << std::endl;
    }
    else
    {
      output << data1 << "\t" << data2;
      if(cGet3Data) output << "\t" << data3;
      output << std::endl;
    }
    ocount++;
  }
  std::cout << "Total Input Data: " << icount << std::endl;
  std::cout << "Total Output Data: " << ocount << std::endl;

  input.close();
  output.close();
}

int main (int argc, char ** argv)
{
  Fl_Window *window;
  Fl_Input *input;
  Fl_Input *output;
  Fl_Button *inpick;
  Fl_Button *oppick;
  Fl_Box *box;
  Fl_Counter *counter;
  Fl_Check_Button *cbutton;
  Fl_Check_Button *dbutton;
  Fl_Check_Button *ebutton;
  Fl_Return_Button *rbutton;
  Fl_File_Chooser *fpick;

  GObject myobjects;
  myobjects.mTag = 0;

  window = new Fl_Window (305, 195, "Bulk Data Filter");
  window->align(FL_ALIGN_CLIP|FL_ALIGN_INSIDE);
  window->begin();
  input = new Fl_Input(100, 10, 150, 25, "Input File:");
  input->value(IFILENAME_DEFAULT);
  inpick = new Fl_Button(260, 10, 25, 25, "...");
  inpick->callback((Fl_Callback*)incall, (void*)&myobjects);
  output = new Fl_Input(100, 40, 150, 25, "Output File:");
  output->value(OFILENAME_DEFAULT);
  oppick = new Fl_Button(260, 40, 25, 25, "...");
  oppick->callback((Fl_Callback*)opcall, (void*)&myobjects);
  box = new Fl_Box (15, 70, 100, 25,"Block Size:");
  counter = new Fl_Counter(105, 70, 150, 25);
  counter->step(1);
  counter->lstep(10);
  counter->value(BLOCK_SIZE_DEFAULT);
  cbutton = new Fl_Check_Button(10, 100, 100, 25, "Use Last Value in Block");
  dbutton = new Fl_Check_Button(10, 130, 100, 25, "First Row Data Label");
  dbutton->set();
  ebutton = new Fl_Check_Button(10, 160, 100, 25, "3-Column Data");
  rbutton = new Fl_Return_Button(195, 160, 100, 25, "Filter");
  rbutton->callback((Fl_Callback*)bt_callback, (void*)&myobjects);
  fpick = new Fl_File_Chooser(".", "*", Fl_File_Chooser::SINGLE, "Select a file");
  fpick->preview(0);
  fpick->callback(fc_callback, (void*)&myobjects);
  myobjects.pBlockSize = counter;
  myobjects.pGetAverage = cbutton;
  myobjects.pGetLabel = dbutton;
  myobjects.pGet3Data = ebutton;
  myobjects.pIFileName = input;
  myobjects.pOFileName = output;
  myobjects.pFileChooser = fpick;
  window->end ();
  window->show (argc, argv);

  return(Fl::run());
}
