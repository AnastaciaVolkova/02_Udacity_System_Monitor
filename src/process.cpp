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

string Process::ReadProcStatus(string target_key) {
  ifstream ifs(string("/proc/") + to_string(id_) + string("/status"));
  string key = "", val = "", line;
  regex pat("([a-z,A-Z]+):[\t, ]+([0-9]+).+");
  smatch sm;
  getline(ifs, line);
  do {
    if (regex_match(line, sm, pat)) {
      key = sm[1].str();
      val = sm[2].str();
    }
  } while (getline(ifs, line) && (key != target_key));
  ifs.close();
  return val;
}

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

  // Find out when process starts after system boots (seconds).
  float starttime = static_cast<float>(
                        stol(tokens[static_cast<int>(ProcIdStat::starttime)])) /
                    herz;

  // Find how much system works.
  float seconds = uptime - starttime;

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
long int Process::UpTime() {
  // Get number of clock ticks per second.
  float herz = static_cast<float>(sysconf(_SC_CLK_TCK));

  // Get starttime field from /proc/[id]/stat.
  string line;
  ifstream ifs(string("/proc/") + to_string(id_) + "/stat");
  getline(ifs, line);
  ifs.close();
  istringstream iss(line);
  vector<string> tokens(istream_iterator<string>{iss},
                        istream_iterator<string>());

  // Get uptime of the system.
  ifs.open("/proc/uptime");
  getline(ifs, line, ' ');
  ifs.close();
  float uptime = static_cast<float>(stol(line));  // Uptime of system (seconds).

  // Find out when process starts after system boots (seconds).
  float starttime = static_cast<float>(
                        stol(tokens[static_cast<int>(ProcIdStat::starttime)])) /
                    herz;

  // Find how much process works.
  float seconds = uptime - starttime;

  return seconds;
}

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return cpu_usage_sum_ > a.cpu_usage_sum_;
}