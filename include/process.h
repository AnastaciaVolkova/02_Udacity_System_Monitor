#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int id) : id_(id){};

  int Pid();
  std::string User();
  std::string Command();
  float CpuUtilization();
  std::string Ram();
  long int UpTime();
  bool operator<(Process const& a) const;

 private:
  int id_;
  float cpu_usage_sum_ = 0;  // Processor utilization sum.
  int util_calls_num_ = 0;   // Number of CpuUtilization calls.
};

#endif
