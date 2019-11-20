// vrpn_Sensel_Morph.C
//
// Copyright (c) 2019, University of Minnesota
// Author: Bridger Herman <herma582@umn.edu>

#pragma once
#include "vrpn_Tracker.h"               // for vrph_Tracker
#include <string>

class VRPN_API vrpn_Sensel_Morph: public vrpn_Tracker
{
 public:
   vrpn_Sensel_Morph(const char *name, vrpn_Connection *c,
                 const char *tracker_cfg_file_name);

   ~vrpn_Sensel_Morph()  {};

    // Called once through each main loop iteration to handle
    // updates.
    virtual void mainloop (void);
    
 protected:
    int m_status;
    int m_numchannels;	// How many analog channels to open

    std::string m_buffer; // Characters read from the device.
    struct timeval m_timestamp;	// Time of the last report from the device

    // virtual int get_report(void);	// Try to read a report from the device

    // // send report iff changed
    // virtual void report_changes (vrpn_uint32 class_of_service = vrpn_CONNECTION_LOW_LATENCY);
    // // send report whether or not changed
    // virtual void report (vrpn_uint32 class_of_service = vrpn_CONNECTION_LOW_LATENCY);

    // NOTE:  class_of_service is only applied to vrpn_Analog
    //  values, not vrpn_Button or vrpn_Dial

    void clear_values();
    int reset();

 private:
};
