#include <unistd.h>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"

using namespace std;
using std::set;
using std::size_t;
using std::string;
using std::vector;

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { return processes_; }

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
float System::MemoryUtilization() {
  ifstream ifs("/proc/meminfo");
  string line;
  float mem_total, mem_free;
  regex rx("(^[a-z,A-Z]+): +([0-9]+)");
  for (int i = 0; i < 2; i++) {
    getline(ifs, line);
    smatch sm;
    regex_search(line, sm, rx);
    string key = sm.str(1);
    string value = sm.str(2);
    if (key == "MemTotal") mem_total = stof(value);
    if (key == "MemFree") mem_free = stof(value);
  }
  ifs.close();
  return mem_total - mem_free;
}

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

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() {
  string line;
  ifstream ifs("/proc/stat");
  // Search for "procs_running".
  string key, val;
  do {
    getline(ifs, line);
    istringstream iss(line);
    getline(iss, key, ' ');
    getline(iss, val, ' ');
  } while (key != "procs_running");
  ifs.close();
  return stoi(val);
}

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return 0; }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return 0; }