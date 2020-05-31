#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
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
  float idle = 0, non_idle = 0;
  for (int i = 0; i < window_size; i++) {
    ifstream ifs("/proc/stat");
    string line;
    // Read the line which aggregates the numbers in all of the other "cpuN"
    // lines .
    getline(ifs, line);
    ifs.close();
    stringstream sst(line);

    // Get line with numbers.
    getline(sst, line, ' ');
    getline(sst, line);
    istringstream iss(line);

    // Store kernel activity numbers.
    vector<float> activity_numbers(istream_iterator<float>{iss},
                                   istream_iterator<float>());

    // Calculate CPU usage.
    /*
    PrevIdle = previdle + previowait
    Idle = idle + iowait

    PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq +
    prevsteal NonIdle = user + nice + system + irq + softirq + steal

    PrevTotal = PrevIdle + PrevNonIdle
    Total = Idle + NonIdle

    # differentiate: actual value minus the previous one
    totald = Total - PrevTotal
    idled = Idle - PrevIdle
    */
    idle += activity_numbers[static_cast<int>(FieldsNames::idle)] +
            activity_numbers[static_cast<int>(FieldsNames::iowait)];
    non_idle += activity_numbers[static_cast<int>(FieldsNames::user)] +
                activity_numbers[static_cast<int>(FieldsNames::nice)] +
                activity_numbers[static_cast<int>(FieldsNames::system)] +
                activity_numbers[static_cast<int>(FieldsNames::irq)] +
                activity_numbers[static_cast<int>(FieldsNames::softirq)] +
                activity_numbers[static_cast<int>(FieldsNames::steal)];
    this_thread::sleep_for(chrono::milliseconds(1));
  }
  // Get average of CPU usage parameters.
  idle /= window_size;
  non_idle /= window_size;
  return non_idle / (idle + non_idle);
}