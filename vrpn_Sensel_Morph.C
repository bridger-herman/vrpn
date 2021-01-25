// vrpn_Sensel_Morph.C
//
// Copyright (c) 2019, University of Minnesota
// Author: Bridger Herman <herma582@umn.edu>
//
// License: Standard VRPN.

#include <iostream>
#include <math.h> // for floor
#include <sstream>

#include "vrpn_Sensel_Morph.h"

#define VRPN_TIMESTAMP_MEMBER                                                  \
    m_timestamp // Configuration required for vrpn_MessageMacros in this class.
#include "vrpn_MessageMacros.h" // for VRPN_MSG_INFO, VRPN_MSG_WARNING, VRPN_MSG_ERROR

// Sensel headers
#include "sensel.h"
#include "sensel_device.h"

#undef VERBOSE

const int CHANNELS_PER_CONTACT = 3;
const float TOUCH_CUTOFF = 0.0;

vrpn_Sensel_Morph::vrpn_Sensel_Morph(const char *name,
                                    int max_num_contacts,
                                     vrpn_Connection *c = NULL)
    : vrpn_Analog(name, c)
{
    std::cout << "Initializing Sensel Morph" << std::endl;

    // Set the parameters in the parent classes
    // 15 channels (x, y, pressure) for 5 fingers
    m_max_num_contacts = max_num_contacts;
    vrpn_Analog::num_channel = max_num_contacts * CHANNELS_PER_CONTACT;
    fprintf(stdout, "Max num contacts: %d (%d channels)\n", m_max_num_contacts, m_max_num_contacts * CHANNELS_PER_CONTACT);

    // Get a list of avaialble Sensel devices
    senselGetDeviceList(&m_list);
    if (m_list.num_devices == 0) {
        fprintf(stdout, "No device found\n");
        return;
    }

    // Open a Sensel device by the id in the SenselDeviceList, handle
    // initialized
    senselOpenDeviceByID(&m_handle, m_list.devices[0].idx);

    // Get the sensor info
    senselGetSensorInfo(m_handle, &m_sensor_info);

    // Set the frame content to scan force data
    senselSetFrameContent(m_handle, FRAME_CONTENT_CONTACTS_MASK);

    // Allocate a frame of data, must be done before reading frame data
    senselAllocateFrameData(m_handle, &m_frame);

    // Start scanning the Sensel device
    senselStartScanning(m_handle);

    vrpn_gettimeofday(&m_timestamp, NULL);

    m_col_per_mm = m_sensor_info.num_cols / m_sensor_info.width;
    m_row_per_mm = m_sensor_info.num_rows / m_sensor_info.height;
}

vrpn_Sensel_Morph::~vrpn_Sensel_Morph()
{
    // TODO: Doesn't actually close sensel when Ctrl-C is pressed
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

    for (int i = 0; i < m_numchannels; i++) {
        vrpn_Analog::last[i] = vrpn_Analog::channel[i];
        vrpn_Analog::channel[i] = 0.0f;
    }

    unsigned int num_frames, num_coords = 0;

    // Read all available data from the Sensel device
    senselReadSensor(m_handle);

    // Get number of frames available in the data read from the sensor
    senselGetNumAvailableFrames(m_handle, &num_frames);
    for (int f = 0; f < num_frames; f++) {
        // Read one frame of data
        senselGetFrame(m_handle, m_frame);
        // Put contact data in Analog channels array
        if (m_frame->n_contacts > 0) {
            int c = 0;
            for (c = 0; c < m_frame->n_contacts && c < m_max_num_contacts; c++) {
                channel[c * CHANNELS_PER_CONTACT + 0] = m_frame->contacts[c].x_pos;
                channel[c * CHANNELS_PER_CONTACT + 1] = m_frame->contacts[c].y_pos;
                channel[c * CHANNELS_PER_CONTACT + 2] = m_frame->contacts[c].total_force;
            }
            // Zero out the rest of the channels
            for (; c < m_max_num_contacts; c++) {
                for (int xyf = 0; xyf < CHANNELS_PER_CONTACT; xyf++) {
                    channel[c * CHANNELS_PER_CONTACT + xyf] = 0.0f;
                }
            }
        }
    }

    vrpn_Analog::report_changes();
    return;
}
