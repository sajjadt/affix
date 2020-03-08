#pragma once
#include <chrono>
#include <map>
#include <iostream>
#include <vector>

enum EventType{
  CUT_WRITE_STARTED, 
  CUT_WRITE_FINISHED, 
  
  CUT_READ_STARTED, 
  CUT_READ_FINISHED, 
  
  CUT_READWRITE_STARTED, 
  CUT_READWRITE_FINISHED, 
  
  CUT_COMP_STARTED, 
  CUT_COMP_FINISHED,
  
  THREAD_COMP_STARTED, 
  THREAD_COMP_FINISHED,

  DATA_REORDER_STARTED,
  DATA_REORDER_FINISHED
};

struct Event{
  Event(EventType type, std::chrono::time_point<std::chrono::high_resolution_clock> time_point, int cut_index):
  type(type), time_point(time_point), cut_index(cut_index) {}

  EventType type;
  int cut_index;
  int thread_index;
  std::chrono::time_point<std::chrono::high_resolution_clock> time_point;
  friend std::ostream& operator<<(std::ostream& os, const Event& dt);  
};

template <typename Enumeration>
auto as_integer(Enumeration const value)
    -> typename std::underlying_type<Enumeration>::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

class Perf{
public:
  Perf(){}
  static void record_event(const Event&);
  static void dump_events();
private:
  static std::vector<Event> events; // Map from cut_no
};
