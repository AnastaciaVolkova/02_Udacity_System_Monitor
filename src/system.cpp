#include <dirent.h>
#include <unistd.h>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using namespace std;
using std::regex;
using std::set;
using std::size_t;
using std::smatch;
using std::string;
using std::vector;

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// DONE: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  DIR* dirp = opendir("/proc/");
  struct dirent* dp;

  std::regex pat("[0-9]+");
  std::smatch sm;

  while ((dp = readdir(dirp)) != NULL) {
    string s(dp->d_name);
    if (std::regex_match(s, sm, pat))
      processes_.push_back(Process(std::stoi(string(sm[0].str()))));
  }
  return processes_;
}

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() {
  ifstream ifs("/proc/version");
  string line;
  getline(ifs, line);
  ifs.close();
  istringstream iss(line);
  vector<string> tokens(istream_iterator<string>{iss},
                        istream_iterator<string>());

  return tokens[2];
}

// DONE: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// DONE: Return the operating system name
std::string System::OperatingSystem() {
  ifstream ifs("/etc/os-release");
  string key = "", val = "";
  // Find string with operating system name.
  do {
    string line;
    getline(ifs, line);
    istringstream iss(line);
    getline(iss, key, '=');
    getline(iss, val, '=');
  } while (key != "PRETTY_NAME");
  ifs.close();
  // Trim quotes.
  if (val[0] == '\"') val.erase(0, 1);
  if (val[val.size() - 1] == '\"') val.erase(val.size() - 1, 1);
  return val;
}

int System::ReadProcStat(string target_key) {
  string line;
  ifstream ifs("/proc/stat");
  // Search for "procs_running".
  string key, val;
  do {
    getline(ifs, line);
    istringstream iss(line);
    getline(iss, key, ' ');
    getline(iss, val, ' ');
  } while (key != target_key);
  ifs.close();
  return stoi(val);
}

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() { return ReadProcStat("procs_running"); }

// DONE: Return the total number of processes on the system
int System::TotalProcesses() { return ReadProcStat("processes"); }

// DONE: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }