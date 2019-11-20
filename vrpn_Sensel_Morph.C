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

const int NUM_SENSEL_CHANNELS = 3;

vrpn_Sensel_Morph::vrpn_Sensel_Morph(const char *name, vrpn_Connection *c = NULL) :
    vrpn_Analog(name, c)
{
    std::cout << "Initializing Sensel Morph" << std::endl;

    // Set the parameters in the parent classes
    // 3 channels (x, y, pressure)
    vrpn_Analog::num_channel = NUM_SENSEL_CHANNELS;

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

    vrpn_gettimeofday(&m_timestamp, NULL);
}

vrpn_Sensel_Morph::~vrpn_Sensel_Morph() {
    senselClose(m_handle);
}

void vrpn_Sensel_Morph::report_changes(vrpn_uint32 class_of_service)
{
    vrpn_Analog::timestamp = m_timestamp;

    vrpn_Analog::report_changes(class_of_service);
}

void vrpn_Sensel_Morph::report(vrpn_uint32 class_of_service)
{
    vrpn_Analog::timestamp = m_timestamp;

    vrpn_Analog::report(class_of_service);
}

// This routine is called each time through the server's main loop. It will
// take a course of action depending on the current status of the Sensel Morph,
// either trying to reset it or trying to get a reading from it.
void vrpn_Sensel_Morph::mainloop(void)
{
    server_mainloop();

    vrpn_gettimeofday(&m_timestamp, NULL);

    for (int i = 0; i < NUM_SENSEL_CHANNELS; i++) {
        vrpn_Analog::last[i] = vrpn_Analog::channel[i];
        vrpn_Analog::channel[i] = 42.0;
    }

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
        channel[2] = total_force;
        // fprintf(stdout, "Total Force : %f\n", total_force);
    }

    vrpn_Analog::report_changes();
    return;
}
