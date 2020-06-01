#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include "linux_parser.h"
#include "processor.h"

using namespace std;

enum class FieldsNames : int {
  user = 0,
  nice,
  system,
  idle,
  iowait,
  irq,
  softirq,
  steal,
  quest,
  quest_nice
};

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
  /*PrevIdle = previdle + previowait
  Idle = idle + iowait

  PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq +
  prevsteal NonIdle = user + nice + system + irq + softirq + steal

  PrevTotal = PrevIdle + PrevNonIdle
  Total = Idle + NonIdle

  # differentiate: actual value minus the previous one
  totald = Total - PrevTotal
  idled = Idle - PrevIdle

  CPU_Percentage = (totald - idled)/totald
  */
  float idle_prev = idle_;
  float total_prev = total_;

  vector<long long> x = LinuxParser::CpuUtilization();
  idle_ = static_cast<float>(x[0]);
  total_ = idle_ + static_cast<float>(x[1]);

  float totald = total_ - total_prev;
  float idled = idle_ - idle_prev;
  float d = (totald - idled) / totald;
  return d;
};
