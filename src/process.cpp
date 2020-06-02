#include <unistd.h>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <regex>
#include <sstream>
#include <string>
#include <vector>
#include "linux_parser.h"
#include "process.h"

using std::ifstream;
using std::istream_iterator;
using std::istringstream;
using std::ostringstream;
using std::regex;
using std::smatch;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return id_; }

// Initialize Processs with cpu utilizaton.
Process::Process(int id) : id_(id) { CpuUtilization(); }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() {
  vector<float> x(2);
  x = LinuxParser::CpuUtilization(id_);
  float total_time = x[0];
  float seconds = x[1];
  // Compute process utilization and add to sum.
  cpu_usage_sum_ += (total_time / seconds);

  util_calls_num_++;

  // Return average of utilization.
  return cpu_usage_sum_ / util_calls_num_;
}

// DONE : Return the command that generated this process
string Process::Command() { return LinuxParser::Command(id_); }

// DONE: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(id_); }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(id_); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(id_); }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return (static_cast<float>(cpu_usage_sum_) / util_calls_num_) >
         (static_cast<float>(a.cpu_usage_sum_) / a.util_calls_num_);
}
