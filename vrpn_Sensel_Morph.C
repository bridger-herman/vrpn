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

// Sensel headers
#include "sensel.h"
#include "sensel_device.h"

#undef VERBOSE

vrpn_Sensel_Morph::vrpn_Sensel_Morph(const char *name, vrpn_Connection *c = NULL,
        const char *tracker_cfg_file_name = NULL) :
    vrpn_Tracker(name, c, tracker_cfg_file_name)
{
    std::cout << "Initializing Sensel Morph" << std::endl;
    //Get a list of avaialble Sensel devices
    senselGetDeviceList(&m_list);
    if (m_list.num_devices == 0)
    {
        fprintf(stdout, "No device found\n");
        fprintf(stdout, "Press Enter to exit example\n");
        getchar();
    }

    //Open a Sensel device by the id in the SenselDeviceList, handle initialized 
    senselOpenDeviceByID(&m_handle, m_list.devices[0].idx);

    //Get the sensor info
    senselGetSensorInfo(m_handle, &m_sensor_info);

    //Set the frame content to scan force data
    senselSetFrameContent(m_handle, FRAME_CONTENT_PRESSURE_MASK);

    //Allocate a frame of data, must be done before reading frame data
    senselAllocateFrameData(m_handle, &m_frame);

    //Start scanning the Sensel device
    senselStartScanning(m_handle);
}

// int vrpn_Sensel_Morph::get_report() {
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

    float total_force = 0.0f;
    unsigned int num_frames = 0;
    //Read all available data from the Sensel device
    senselReadSensor(m_handle);
    //Get number of frames available in the data read from the sensor
    senselGetNumAvailableFrames(m_handle, &num_frames);
    for (int f = 0; f < num_frames; f++)
    {
        //Read one frame of data
        senselGetFrame(m_handle, m_frame);
        //Calculate the total force
        total_force = 0;
        for (int i = 0; i < m_sensor_info.num_cols*m_sensor_info.num_rows; i++)
        {
            total_force = total_force + m_frame->force_array[i];
        }
        fprintf(stdout, "Total Force : %f\n", total_force);
    }
    return;
}
