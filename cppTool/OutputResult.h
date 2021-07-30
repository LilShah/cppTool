#pragma once
#include <iostream>
#include <iomanip>

struct tOutputResult
{
   /// enum storing signals, print when BUY and SELL are hit
   enum class tSignal
   {
      SELL,
      BUY,
      NONE
   };

   std::tm tmTime;   ///< variable to store time info, saves till minute
   float fOpen;      ///< opening stock price for the given minute
   float fHigh;      ///< highest stock price within a given minute
   float fLow;       ///< lowest stock price within a given minute
   float fClose;     ///< closing stock price, at the end of the minute
   float fFastMa;    ///< fast moving average (21 values in application)
   float fSlowMa;    ///< slow moving average (55 values in application)
   tSignal nSignal;  ///< which of the signal to generate from the enum above

   /**
    * ostream overload.
    *
    * \param os ostream reference
    * \param sOutRes object reference
    * \return ostream reference
    */
   friend std::ostream& operator<<(std::ostream& os, const tOutputResult& sOutRes)
   {
      //save ostream state
      std::ios oldState(nullptr);
      oldState.copyfmt(std::cout);

      os << std::setfill('0')
         << std::setw(2)
         << sOutRes.tmTime.tm_hour << ':'
         << std::setw(2)
         << sOutRes.tmTime.tm_min;

      //restore ostream, mostly for setw and setfill
      os.copyfmt(oldState);

      os << std::fixed
         << std::setprecision(2)
         << ',' << sOutRes.fOpen
         << ',' << sOutRes.fHigh
         << ',' << sOutRes.fLow
         << ',' << sOutRes.fClose
         << std::setprecision(4)
         << ',' << sOutRes.fFastMa
         << ',' << sOutRes.fSlowMa
         << ',';

      switch (sOutRes.nSignal)
      {
      case tSignal::BUY:
         os << "BUY";
         break;
      case tSignal::SELL:
         os << "SELL";
         break;
      }
      os << "\n";
      return os;
   }

};