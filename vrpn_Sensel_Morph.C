// vrpn_Sensel_Morph.C
//
// Copyright (c) 2019, University of Minnesota
// Author: Bridger Herman <herma582@umn.edu>
//
// License: Standard VRPN.

#include <math.h>                       // for floor
#include <iostream>
#include <sstream>

#include "vrpn_Sensel_Morph.h"

#define VRPN_TIMESTAMP_MEMBER m_timestamp // Configuration required for vrpn_MessageMacros in this class.
#include "vrpn_MessageMacros.h"         // for VRPN_MSG_INFO, VRPN_MSG_WARNING, VRPN_MSG_ERROR

#undef VERBOSE

#define MAX_TCHANNELS 8	

// Defines the modes in which the box can find itself.
#define	STATUS_RESETTING	(-1)	// Resetting the box
#define	STATUS_SYNCING		(0)	// Looking for the first character of report
#define	STATUS_READING		(1)	// Looking for the rest of the report
#define MAX_TIME_INTERVAL  (2000000) // max time between reports (usec)


vrpn_Sensel_Morph::vrpn_Sensel_Morph(const char *name, vrpn_Connection *c = NULL,
                 const char *tracker_cfg_file_name = NULL) :
    vrpn_Tracker(name, c, tracker_cfg_file_name)
{
    std::cout << "Initializing Sensel Morph" << std::endl;
}

// void vrpn_Sensel_Morph::get_report() {

// }

// void vrpn_Sensel_Morph::report_changes() {

// }

// void vrpn_Sensel_Morph::report() {

// }

// This routine is called each time through the server's main loop. It will
// take a course of action depending on the current status of the Sensel Morph,
// either trying to reset it or trying to get a reading from it.
void vrpn_Sensel_Morph::mainloop(void)
{
    server_mainloop();

    // std::cout << "Sensel morph mainloop()" << std::endl;

    return;
}
