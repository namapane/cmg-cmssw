#ifndef ThroughputService_h
#define ThroughputService_h

// C++ headers
#include <string>
#include <chrono>
#include <functional>

// TBB headers
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_unordered_set.h>

// ROOT headers
#include <TH1F.h>

// CMSSW headers
#include "FWCore/ServiceRegistry/interface/ActivityRegistry.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/ServiceRegistry/interface/SystemBounds.h"
#include "FWCore/ServiceRegistry/interface/StreamContext.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Provenance/interface/EventID.h"
#include "DataFormats/Provenance/interface/Timestamp.h"
#include "DataFormats/Provenance/interface/ModuleDescription.h"
#include "HLTrigger/Timer/interface/FastTimer.h"

// based on clock_gettime(CLOCK_MONOTONIC, ...)
struct clock_gettime_monotonic
{
  typedef std::chrono::nanoseconds                                      duration;
  typedef duration::rep                                                 rep;
  typedef duration::period                                              period;
  typedef std::chrono::time_point<clock_gettime_monotonic, duration>    time_point;

  static constexpr bool is_steady = true;
  static const     bool is_available;

  static time_point now() noexcept
  {
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);

    return time_point( std::chrono::seconds(t.tv_sec) + std::chrono::nanoseconds(t.tv_nsec) );
  }

};

class ThroughputService {
public:
  ThroughputService(const edm::ParameterSet &, edm::ActivityRegistry & );
  ~ThroughputService();

private:
  void preallocate(edm::service::SystemBounds const & bounds);
  void preStreamBeginRun(edm::StreamContext const & sc);
  void postStreamEndLumi(edm::StreamContext const& sc);
  void postStreamEndRun(edm::StreamContext const & sc);
  void preSourceEvent(edm::StreamID sid);
  void postEvent(edm::StreamContext const & sc);

public:
  static void fillDescriptions(edm::ConfigurationDescriptions & descriptions);

private:

  struct stream_histograms
  {
    stream_histograms() :
      sourced_events(nullptr),
      retired_events(nullptr)
    { }

    TH1F * sourced_events;
    TH1F * retired_events;
  };

  std::vector<stream_histograms>        m_stream_histograms;
  
  clock_gettime_monotonic::time_point   m_startup;

  // histogram-related data members
  double                                m_time_range;
  double                                m_time_resolution;

  // DQM service-related data members
  unsigned int                          m_module_id;                // pseudo module id for the FastTimerService, needed by the thread-safe DQMStore 
  std::string                           m_dqm_path;
};

#endif // ! ThroughputService_h
