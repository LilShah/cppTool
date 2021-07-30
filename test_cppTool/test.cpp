#include "pch.h"
#include "../cppTool/TradingSignals.cpp"

#include <fstream>

TEST(TestTradingApp, test_ParseLine)
{
   EXPECT_EXIT(TradingSignals tr(22, 55), ::testing::ExitedWithCode(-1), ".*");
   TradingSignals tr(55, 22);
   EXPECT_EXIT(tr.parseLine(std::string("hello")), ::testing::ExitedWithCode(-1), ".*");
   EXPECT_EXIT(tr.parseLine(std::string("09,01,02")), ::testing::ExitedWithCode(-1), ".*");
   {
      auto ret = tr.parseLine(std::string("09:1:30.596,101.05"));
      EXPECT_EQ(ret.first.tm_sec, 30);
      EXPECT_EQ(ret.first.tm_min, 1);
      EXPECT_EQ(ret.first.tm_hour, 9);
      EXPECT_FLOAT_EQ(ret.second, 101.05);
   } {
      auto ret = tr.parseLine(std::string("17:2:32.432,11.05"));
      EXPECT_EQ(ret.first.tm_sec, 32);
      EXPECT_EQ(ret.first.tm_min, 2);
      EXPECT_EQ(ret.first.tm_hour, 17);
      EXPECT_FLOAT_EQ(ret.second, 11.05);
   } {
      auto ret = tr.parseLine(std::string("25:1:30.596,101.05"));
      EXPECT_EQ(ret.first.tm_sec, 0);
      EXPECT_EQ(ret.first.tm_min, 0);
      EXPECT_EQ(ret.first.tm_hour, 0);
      EXPECT_FLOAT_EQ(ret.second, 101.05);
   } {
      auto ret = tr.parseLine(std::string("091:68:312.596,3234.054"));
      EXPECT_EQ(ret.first.tm_sec, 0);
      EXPECT_EQ(ret.first.tm_min, 0);
      EXPECT_EQ(ret.first.tm_hour, 9);
      EXPECT_FLOAT_EQ(ret.second, 3234.054);
   }
}
TEST(TestTradingApp, test_ParseFile)
{
   std::ofstream outFile;
   try
   {
      outFile.open("out.txt");
   }
   catch (std::exception& e)
   {
      std::cout << "Test file not created or found, test will not run\n";
      FAIL();
   }
   srand(1234);   //random but unchanging for debugging

   outFile << std::setfill('0');

   float fPrice = 100;
   for (int i = 0; i < 100; ++i)
   {
      int nMilliSec = rand() % 999;
      bool isHigher = rand() % 2;
      outFile << "01:" << std::setw(2) << i << '.' << std::setw(3) << nMilliSec;
      if (isHigher)
      {
         fPrice += 1.0 + 0.2;
      }
      else
      {
         fPrice -= 1.0 - 0.1;
      }
      outFile << ',' << std::to_string(fPrice) << '\n';
   }
   outFile.close();
   TradingSignals tr(15, 5);
   auto vecResult = tr.parseFile("out.txt");

   // now check values
   EXPECT_EQ(vecResult[0].tmTime.tm_hour, 1);
   EXPECT_EQ(vecResult[0].tmTime.tm_min, 0);
   EXPECT_FLOAT_EQ(vecResult[0].fOpen, 101.2);
   EXPECT_FLOAT_EQ(vecResult[0].fHigh, 101.2);
   EXPECT_FLOAT_EQ(vecResult[0].fLow, 101.2);
   EXPECT_FLOAT_EQ(vecResult[0].fClose, 101.2);
   EXPECT_FLOAT_EQ(vecResult[0].fFastMa, 0);
   EXPECT_FLOAT_EQ(vecResult[0].fSlowMa, 0);
   EXPECT_EQ(vecResult[0].nSignal, tOutputResult::tSignal::NONE);

   EXPECT_EQ(vecResult[1].tmTime.tm_hour, 1);
   EXPECT_EQ(vecResult[1].tmTime.tm_min, 1);
   EXPECT_FLOAT_EQ(vecResult[1].fOpen, 100.3);
   EXPECT_FLOAT_EQ(vecResult[1].fHigh, 100.3);
   EXPECT_FLOAT_EQ(vecResult[1].fLow, 100.3);
   EXPECT_FLOAT_EQ(vecResult[1].fClose, 100.3);
   EXPECT_FLOAT_EQ(vecResult[1].fFastMa, 0);
   EXPECT_FLOAT_EQ(vecResult[1].fSlowMa, 0);
   EXPECT_EQ(vecResult[1].nSignal, tOutputResult::tSignal::NONE);

   EXPECT_EQ(vecResult[5].tmTime.tm_hour, 1);
   EXPECT_EQ(vecResult[5].tmTime.tm_min, 5);
   EXPECT_FLOAT_EQ(vecResult[5].fOpen, 98.8);
   EXPECT_FLOAT_EQ(vecResult[5].fHigh, 98.8);
   EXPECT_FLOAT_EQ(vecResult[5].fLow, 98.8);
   EXPECT_FLOAT_EQ(vecResult[5].fClose, 98.8);
   EXPECT_FLOAT_EQ(vecResult[5].fFastMa, 100.18);
   EXPECT_FLOAT_EQ(vecResult[5].fSlowMa, 0);
   EXPECT_EQ(vecResult[5].nSignal, tOutputResult::tSignal::NONE);

   EXPECT_EQ(vecResult[15].tmTime.tm_hour, 1);
   EXPECT_EQ(vecResult[15].tmTime.tm_min, 15);
   EXPECT_FLOAT_EQ(vecResult[15].fOpen, 104.49996);
   EXPECT_FLOAT_EQ(vecResult[15].fHigh, 104.49996);
   EXPECT_FLOAT_EQ(vecResult[15].fLow, 104.49996);
   EXPECT_FLOAT_EQ(vecResult[15].fClose, 104.49996);
   EXPECT_FLOAT_EQ(vecResult[15].fFastMa, 104.2);
   EXPECT_FLOAT_EQ(vecResult[15].fSlowMa, 102.12);
   EXPECT_EQ(vecResult[15].nSignal, tOutputResult::tSignal::NONE);
}
