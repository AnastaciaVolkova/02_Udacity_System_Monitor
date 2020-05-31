#include <unistd.h>
#include <cstddef>
#include <fstream>
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

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() { return string(); }

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return 0.0; }

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

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return 0; }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return 0; }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return 0; }