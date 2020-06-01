#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // DONE: See src/processor.cpp
 private:
  float total_ = 0, idle_ = 0;
};

#endif
