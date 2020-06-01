#include <unistd.h>
#include <cctype>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::ifstream;
using std::istream_iterator;
using std::istringstream;
using std::ostringstream;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return id_; }

enum class ProcIdStat : int {
  utime = 13,
  stime,
  cutime,
  cstime,
  starttime = 23
};

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() {
  // Get number of clock ticks per second.
  float herz = static_cast<float>(sysconf(_SC_CLK_TCK));

  // Read fields of /proc/[pid]/stat.
  ifstream ifs(string("/proc/") + to_string(id_) + string("/stat"));
  ostringstream oss;
  oss << ifs.rdbuf();
  ifs.close();
  istringstream iss(oss.str());
  vector<string> tokens(istream_iterator<string>{iss},
                        istream_iterator<string>());

  // Total time of process (seconds).
  float total_time = (stof(tokens[static_cast<int>(ProcIdStat::utime)]) +
                      stof(tokens[static_cast<int>(ProcIdStat::stime)]) +
                      stof(tokens[static_cast<int>(ProcIdStat::cutime)]) +
                      stof(tokens[static_cast<int>(ProcIdStat::cstime)])) /
                     herz;

  // Get uptime of the system.
  ifs.open("/proc/uptime");
  string line;
  getline(ifs, line, ' ');
  ifs.close();

  float uptime = static_cast<float>(stol(line));  // Uptime of system (seconds).

  // Find out when process starts after system boots.
  float starttime =
      static_cast<float>(stol(tokens[static_cast<int>(ProcIdStat::starttime)]));

  // Find how much system works.
  float seconds = uptime - starttime;

  // Compute process utilization and add to sum.
  cpu_usage_sum_ += (total_time / seconds);

  util_calls_num_++;

  // Return average of utilization.
  return cpu_usage_sum_ / util_calls_num_;
}

// DONE : Return the command that generated this process
string Process::Command() {
  // Read fields of /proc/[pid]/cmdline.
  ifstream ifs(string("/proc/") + to_string(id_) + string("/cmdline"));
  string line;
  getline(ifs, line);
  return line;
}

// TODO: Return this process's memory utilization
string Process::Ram() { return string(); }

// TODO: Return the user (name) that generated this process
string Process::User() { return string(); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return 0; }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return cpu_usage_sum_ > a.cpu_usage_sum_;
}