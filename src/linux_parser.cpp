#include <dirent.h>
#include <unistd.h>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::regex;
using std::smatch;
using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::ProcStatus(int id, string target_key) {
  std::ifstream ifs(kProcDirectory + to_string(id) + kStatusFilename);
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

string LinuxParser::ProcStat(string target_key) {
  string line;
  std::ifstream ifs(kProcDirectory + kStatFilename);
  // Search for target_key.
  string key, val;
  do {
    std::getline(ifs, line);
    std::istringstream iss(line);
    std::getline(iss, key, ' ');
    std::getline(iss, val, ' ');
  } while (key != target_key);
  ifs.close();
  return val;
}
// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::ifstream ifs("/proc/meminfo");
  std::string line;
  float mem_total, mem_free;
  regex rx("(^[a-z,A-Z]+): +([0-9]+)");
  for (int i = 0; i < 2; i++) {
    getline(ifs, line);
    smatch sm;
    regex_search(line, sm, rx);
    string key = sm.str(1);
    string value = sm.str(2);
    if (key == "MemTotal")
      mem_total = stof(value);
    else if (key == "MemFree")
      mem_free = stof(value);
  }
  ifs.close();
  return (mem_total - mem_free) / mem_total;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream ifs(kProcDirectory + kUptimeFilename);
  string line;
  // Get uptime.
  std::getline(ifs, line, ' ');
  float x = floor(stof(line));
  ifs.close();
  return static_cast<long int>(x);
}

// NOTDONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// NOTDONE: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// NOTDONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// NOTDONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// DONE: Read and return CPU utilization
vector<long long> LinuxParser::CpuUtilization() {
  std::ifstream ifs(kProcDirectory + kStatFilename);
  string line;
  // Read the line which aggregates the numbers in all of the other "cpuN"
  // lines .
  getline(ifs, line);
  ifs.close();
  std::stringstream sst(line);

  // Get line with numbers.
  std::getline(sst, line, ' ');
  std::getline(sst, line);
  std::istringstream iss(line);

  // Store kernel activity numbers.
  vector<float> activity_numbers(std::istream_iterator<float>{iss},
                                 std::istream_iterator<float>());

  // Calculate CPU usage.
  // Idle = idle + iowait
  // NonIdle = user + nice + system + irq + softirq + steal

  long long idle = activity_numbers[kIdle_] + activity_numbers[kIOwait_];
  long long non_idle = activity_numbers[kUser_] + activity_numbers[kNice_] +
                       activity_numbers[kSystem_] + activity_numbers[kIRQ_] +
                       activity_numbers[kSoftIRQ_] + activity_numbers[kSteal_];

  return vector<long long>{idle, non_idle};
}

std::vector<float> LinuxParser::CpuUtilization(int pid) {
  // Get number of clock ticks per second.
  float herz = static_cast<float>(sysconf(_SC_CLK_TCK));

  // Read fields of /proc/[pid]/stat.
  std::ifstream ifs(kProcDirectory + to_string(pid) + kStatFilename);
  std::ostringstream oss;
  oss << ifs.rdbuf();
  ifs.close();
  std::istringstream iss(oss.str());
  vector<string> tokens(std::istream_iterator<string>{iss},
                        std::istream_iterator<std::string>());

  // Total time of process (seconds).
  float total_time = (stof(tokens[static_cast<int>(ProcIdStat::utime)]) +
                      stof(tokens[static_cast<int>(ProcIdStat::stime)]) +
                      stof(tokens[static_cast<int>(ProcIdStat::cutime)]) +
                      stof(tokens[static_cast<int>(ProcIdStat::cstime)])) /
                     herz;

  // Get uptime of the system.
  ifs.open(kProcDirectory + kUptimeFilename);
  string line;
  getline(ifs, line, ' ');
  ifs.close();

  float uptime = static_cast<float>(stol(line));  // Uptime of system (seconds).

  // Find out when process starts after system boots (seconds).
  float starttime = static_cast<float>(
                        stol(tokens[static_cast<int>(ProcIdStat::starttime)])) /
                    herz;

  // Find how long ago process starts.
  float seconds = uptime - starttime;

  return vector<float>{total_time, seconds};
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() { return std::stoi(ProcStat("processes")); }

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  return std::stoi(ProcStat("procs_running"));
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  // Read fields of /proc/[pid]/cmdline.
  std::ifstream ifs(kProcDirectory + to_string(pid) + kCmdlineFilename);
  string line;
  getline(ifs, line);
  return line;
}

string LinuxParser::Ram(int pid) {
  string val = ProcStatus(pid, "VmSize");
  float v = stol(val) / 1024.0;
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(3) << v;
  return oss.str();
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { return ProcStatus(pid, "Uid"); }

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  // Read user id from /proc/[id]/status
  long long target_uid = std::stol(Uid(pid));
  std::ifstream ifs(kPasswordPath);
  long long uid = 0;
  string line, name;
  getline(ifs, line);
  // Read line by line until target uid is found.
  do {
    string x;
    std::stringstream iss(line);
    std::getline(iss, name, ':');  // Read and safe user name.
    std::getline(iss, x, ':');     // Read password.
    std::getline(iss, x, ':');     // Read user id.
    uid = stol(x);

  } while ((uid == target_uid) && getline(ifs, line));
  ifs.close();
  return name;
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  // Get number of clock ticks per second.
  float herz = static_cast<float>(sysconf(_SC_CLK_TCK));

  // Get starttime field from /proc/[id]/stat.
  string line;
  std::ifstream ifs(kProcDirectory + to_string(pid) + kStatFilename);
  getline(ifs, line);
  ifs.close();
  std::istringstream iss(line);
  vector<string> tokens(std::istream_iterator<string>{iss},
                        std::istream_iterator<string>());

  // Get uptime of the system from /proc/uptime.
  ifs.open(kProcDirectory + kUptimeFilename);
  getline(ifs, line, ' ');
  ifs.close();
  float uptime = static_cast<float>(stol(line));  // Uptime of system (seconds).

  // Find out when process starts after system boots (seconds).
  float starttime = static_cast<float>(std::stol(
                        tokens[static_cast<int>(ProcIdStat::starttime)])) /
                    herz;

  // Find how much process works.
  float seconds = uptime - starttime;
  return seconds;
}
