// vrpn_Sensel_Morph.h
//
// Copyright (c) 2019, University of Minnesota
// Author: Bridger Herman <herma582@umn.edu>
//
// NOTE: the sensel library MUST be installed to use this! Follow the
// instructions under `vrpn/sensel-api/sensel-install` to do this.
//
// Inspiration from Sensel example 3 (forces), in
// `vrpn/sensel-api/sensel-examples/sensel-c`

#pragma once
#include "vrpn_Analog.h"               // for vrpn_Analog
#include <string>

// Sensel headers
#include "sensel.h"
#include "sensel_device.h"

class VRPN_API vrpn_Sensel_Morph: public vrpn_Analog
{
    public:
        vrpn_Sensel_Morph(const char *name, int max_num_contacts, vrpn_Connection *c);

        ~vrpn_Sensel_Morph();

        // Called once through each main loop iteration to handle
        // updates.
        virtual void mainloop (void);

    protected:
        int m_status;
        int m_numchannels;	// How many analog channels to open

        //Handle that references a Sensel device
        SENSEL_HANDLE m_handle = NULL;

        //List of all available Sensel devices
        SenselDeviceList m_list;

        //Sensor info from the Sensel device
        SenselSensorInfo m_sensor_info;

        //SenselFrame data that will hold the forces
        SenselFrameData *m_frame = NULL;

        // Maximum number of contacts to send to VRPN Analog
        int m_max_num_contacts;

        // Rows and columns per physical millimeter on the device
        float m_col_per_mm;
        float m_row_per_mm;

        struct timeval m_timestamp;	// Time of the last report from the device

        // send report iff changed
        virtual void report_changes (vrpn_uint32 class_of_service = vrpn_CONNECTION_LOW_LATENCY);
        // send report whether or not changed
        virtual void report (vrpn_uint32 class_of_service = vrpn_CONNECTION_LOW_LATENCY);

        // NOTE:  class_of_service is only applied to vrpn_Analog
        //  values, not vrpn_Button or vrpn_Dial
    private:
};
