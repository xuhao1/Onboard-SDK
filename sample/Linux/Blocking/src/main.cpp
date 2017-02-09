/*! @file main.cpp
 *  @version 3.1.8
 *  @date Aug 05 2016
 *
 *  @brief
 *  New Linux App for DJI Onboard SDK. 
 *  Provides a number of convenient abstractions/wrappers around core API calls.
 *
 *  Calls are blocking; the calling thread will sleep until the
 *  call returns. Use Core API calls or another sample if you 
 *  absolutely need non-blocking calls. 
 *
 *  @copyright
 *  2016 DJI. All rights reserved.
 * */

//System Headers
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>

//DJI Linux Application Headers
#include "LinuxSerialDevice.h"
#include "LinuxThread.h"
#include "LinuxSetup.h"
#include "LinuxCleanup.h"
#include "ReadUserConfig.h"
#include "LinuxMobile.h"
#include "LinuxFlight.h"
#include "LinuxInteractive.h"
#include "LinuxWaypoint.h"
#include "LinuxCamera.h"

//DJI OSDK Library Headers
#include <DJI_Follow.h>
#include <DJI_Flight.h>
#include <DJI_Version.h>
#include <DJI_WayPoint.h>

//Local Mission Planning Suite Headers
//#include <MissionplanHeaders.h>

using namespace std;
using namespace DJI;
using namespace DJI::onboardSDK;
uint8_t from_mobile_data[1024] = {0};

int get_data_after_header(Header * header,uint8_t * data)
{
  int package_min = sizeof(Header) + sizeof(uint32_t) + 2;
  int data_len = header->length - package_min;
  memcpy(data,header + sizeof(Header) + 2,data_len);
  for (int  i = 0;i< header->length;i++)
  {
    std::cout << "char " << i << " " << *((uint8_t*)header +  i ) << std::endl;
  }
  return data_len;
}
void from_mobile_callback(DJI::onboardSDK::CoreAPI * coreAPI, Header * header, UserData userData)
{
  int len = get_data_after_header(header,from_mobile_data);
  std::cout << "Receive Mobile Data!!! len:" << len << std::endl;
//  key_mouse_event event;
//  memcpy(&event,from_mobile_data,len);
//  std::cout<< "data is |"<<(int)event.event_type<< "| "<<event.data[0]<<" "<<event.data[1]<<" "<<event.data[2] << "$"<< std::endl;

}
int setup_keyboard_event(CoreAPI * coreAPI)
{
    coreAPI->setFromMobileCallback(from_mobile_callback);
  std::cout << "Setup callback!" << std::endl;
  return 1;
}
//! Main function for the Linux sample. Lightweight. Users can call their own API calls inside the Programmatic Mode else on Line 68. 
int main(int argc, char *argv[])
{
  //! Instantiate a serialDevice, an API object, flight and waypoint objects and a read thread.
  LinuxSerialDevice* serialDevice = new LinuxSerialDevice(UserConfig::deviceName, UserConfig::baudRate);
  CoreAPI* api = new CoreAPI(serialDevice);
  Flight* flight = new Flight(api);
  LinuxThread read(api, 2);

  //! Setup
  int setupStatus = setup(serialDevice, api, &read);
  if (setupStatus == -1)
  {
    std::cout << "This program will exit now. \n";
//    return 0;
  }
//  unsigned short broadcastAck = api->setBroadcastFreqDefaults(1);
//  usleep(500000);
//  ! Mobile Mode
//  api->getDroneVersion(1);
//  activateNonBlocking(api);
//  usleep(100000);

  if (setup_keyboard_event(api) != 0 )
  {
    while(true) {
      sleep(1);
    }
  }
  //! Cleanup
  int cleanupStatus = cleanup(serialDevice, api, flight, &read);
  if (cleanupStatus == -1)
  {
    std::cout << "Unable to cleanly destroy OSDK infrastructure. There may be residual objects in the system memory.\n";
    return 0;
  }
  std::cout << "Program exited successfully." << std::endl;

  return 0;
}

