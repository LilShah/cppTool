#include "TradingSignals.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

#define LAST_LINE true

TradingSignals::TradingSignals(int nSMaSize, int nFMaSize) :
   nSlowMaSize(nSMaSize),
   nFastMaSize(nFMaSize),
   fPrevDiff(0)
{
   if (nFastMaSize > nSlowMaSize)
   {
      std::cerr << "Fast moving average cannot contain more values than slow moving average!\n";
      exit(-1);
   }
}

void TradingSignals::checkValues(
   std::pair<std::tm, float>& pairTimePointValue,
   tOutputResult& sCurrentResult,
   std::tm& tmCurrentTime,
   float& fLowest,
   float& fHighest,
   float& fClose,
   bool& isOpen,
   bool bLastLine
)
{
   if (tmCurrentTime.tm_min != pairTimePointValue.first.tm_min)
   {
      setValues(
         pairTimePointValue,
         sCurrentResult,
         tmCurrentTime,
         fLowest,
         fHighest,
         fClose,
         isOpen
      );
   }
   if (!isOpen)
   {
      isOpen = true;
      sCurrentResult.fOpen = pairTimePointValue.second;
   }
   if (pairTimePointValue.second < fLowest)
   {
      fLowest = pairTimePointValue.second;
   }
   if (pairTimePointValue.second > fHighest)
   {
      fHighest = pairTimePointValue.second;
   }
   fClose = pairTimePointValue.second;
   if (bLastLine)
   {
      setValues(
         pairTimePointValue,
         sCurrentResult,
         tmCurrentTime,
         fLowest,
         fHighest,
         fClose,
         isOpen
      );
   }
}
void TradingSignals::setValues(
   std::pair<std::tm, float>& pairTimePointValue,
   tOutputResult& sCurrentResult,
   std::tm& tmCurrentTime,
   float& fLowest,
   float& fHighest,
   float& fClose,
   bool& isOpen
)
{
   isOpen = false;
   sCurrentResult.tmTime = tmCurrentTime;
   sCurrentResult.fHigh = fHighest;
   sCurrentResult.fLow = fLowest;
   sCurrentResult.fClose = fClose;
   vecOutputResult.push_back(sCurrentResult);
   sCurrentResult.fFastMa = generateMovingAverage(fClose, nFastMaSize);
   sCurrentResult.fSlowMa = generateMovingAverage(fClose, nSlowMaSize);
   sCurrentResult.nSignal = generateSignal(sCurrentResult.fFastMa, sCurrentResult.fSlowMa);
   vecOutputResult.at(vecOutputResult.size() - 1).fFastMa = sCurrentResult.fFastMa;
   vecOutputResult.at(vecOutputResult.size() - 1).fSlowMa = sCurrentResult.fSlowMa;
   vecOutputResult.at(vecOutputResult.size() - 1).nSignal = sCurrentResult.nSignal;

   //reset
   fHighest = 0.0;
   fLowest = std::numeric_limits<float>::max();
   tmCurrentTime = pairTimePointValue.first;
}


void TradingSignals::printError()
{
   std::cerr<<"usage: cppTool.exe --file_path FILE_PATH\nerror: the following arguments are required : "
      "--file_path\n";
   exit(-1);
}

std::vector<tOutputResult> TradingSignals::parseFile(std::string strPath)
{
   std::ifstream inFile;
   try
   {
      inFile.open(strPath);
   }
   catch (const std::exception& e)
   {
      std::cerr << e.what() << '\n';
      std::cerr << "Wrong file used, try again\n";
      printError();
   }
   tOutputResult sCurrentResult;
   std::string line;
   if (inFile.good())
   {
      getline(inFile, line, '\n');
   }
   else
   {
      std::cerr<< "Empty file detected\n";
      exit(-1);
   }

   //initialise
   auto pairTimePointValue = parseLine(line);
   std::tm tmCurrentTime = pairTimePointValue.first;

   // min possible value 0
   float fHighest = 0.0;
   float fLowest = std::numeric_limits<float>::max();
   float fClose = 0;
   bool isOpen = false;
   do
   {
      checkValues(
            pairTimePointValue,
            sCurrentResult,
            tmCurrentTime,
            fLowest,
            fHighest,
            fClose,
            isOpen
         );
      pairTimePointValue = parseLine(line);
   } while (getline(inFile, line, '\n'));
   inFile.close();
   checkValues(
      pairTimePointValue,
      sCurrentResult,
      tmCurrentTime,
      fLowest,
      fHighest,
      fClose,
      isOpen,
      LAST_LINE
   );

   return vecOutputResult;
}

std::pair<std::tm, float> TradingSignals::parseLine(std::string& strLine)
{
   std::stringstream ssLine(strLine);
   std::string strToken;
   std::vector<std::string> vecTokens;
   while (getline(ssLine, strToken, ','))
   {
      vecTokens.push_back(strToken);
   }
   if (vecTokens.size() != 2)
   {
      std::cerr<<"Error in input value\n";
      exit(-1);
   }
   //[0] -> time, [1] => value
   std::tm tmTimePoint = {};
   std::stringstream ss(vecTokens[0]);
   // seconds and micro second dont matter in the output result
   ss >> std::get_time(&tmTimePoint, "%H:%M:%S");

   return std::make_pair(tmTimePoint, std::stof(vecTokens[1]));
}

float TradingSignals::generateMovingAverage(float fValue, size_t nSize)
{
   if (vecOutputResult.size() < nSize)
   {
      return 0;
   }
   float fAvg = 0;
   for (size_t i = 0; i < nSize; ++i)
   {
      fAvg += vecOutputResult[vecOutputResult.size() - i - 1].fClose;
   }
   return (fAvg / nSize);
}

tOutputResult::tSignal TradingSignals::generateSignal(float fFastMa, float fSlowMa)
{
   if (fFastMa == 0 || fSlowMa == 0 || nSlowMaSize > vecOutputResult.size() )
   {
      return tOutputResult::tSignal::NONE;
   }
   float fCurrentDiff = fFastMa - fSlowMa;
   if (fPrevDiff * fCurrentDiff > 0)   //no sign change, no signal generated
   {
      fPrevDiff = fCurrentDiff;
      return tOutputResult::tSignal::NONE;
   }
   if (fPrevDiff == 0.0f)
   {
      fPrevDiff = fCurrentDiff;
      return tOutputResult::tSignal::NONE;
   }
   fPrevDiff = fCurrentDiff;

   if (fCurrentDiff > 0)
   {
      return tOutputResult::tSignal::BUY;
   }
   else if (fCurrentDiff < 0)
   {
      return tOutputResult::tSignal::SELL;
   }
   //very rare equal case
   return tOutputResult::tSignal::NONE;
}
