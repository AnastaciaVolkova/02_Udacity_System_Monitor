#include <iomanip>
#include <sstream>
#include <string>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  // s + 60*min + 3600*h
  long hours = seconds / 3600;
  hours = hours % 24;  // Get rid of days.
  seconds = seconds % 3600;
  long minutes = seconds / 60;
  seconds = seconds % 60;
  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(2) << std::to_string(hours) << ":"
      << std::setfill('0') << std::setw(2) << std::to_string(minutes) << ":"
      << std::setfill('0') << std::setw(2) << std::to_string(seconds);
  return oss.str();
}