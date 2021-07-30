#include "TradingSignals.h"

#include <fstream>

int main(int argc, char* argv[])
{
   std::vector<std::string> vecArguments(argv + 1, argv + argc);

   TradingSignals oTradingSignals(55, 21) ;

   if (vecArguments.size() != 2 || vecArguments[0] != "--file_path")
   {
      std::cerr<<"usage: cppTool.exe --file_path FILE_PATH\nerror: the following arguments are required : "
         "--file_path\n";
      return -1;
   }
   auto vecOutputResult = oTradingSignals.parseFile(std::string(argv[2]));

   std::ofstream outFile;
   try
   {
      outFile.open("signals.csv");
   }
   catch (...)
   {
      std::cerr << "Error opening or creating output file. This could be a permission issue\n";
      return -1;
   }
   outFile << "TIME,OPEN,HIGH,LOW,CLOSE,FASTMA,SLOWMA,SIGNAL\n";
   for (const auto &sLine : vecOutputResult)
   {
      outFile << sLine;
   }
   outFile.close();
   return 0;
}