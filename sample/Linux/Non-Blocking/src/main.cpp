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
#include "LinuxInteractiveNonBlocking.h"
#include "LinuxWaypoint.h"

//DJI OSDK Library Headers
#include <DJI_Follow.h>
#include <DJI_Flight.h>
#include <DJI_Version.h>
#include <DJI_WayPoint.h>

#include <keyboard_event.h>

using namespace std;
using namespace DJI;
using namespace DJI::onboardSDK;

uint8_t from_mobile_data[1024] = {0};

int get_data_after_header(Header * header,uint8_t * data)
{
  int package_min = sizeof(Header) + sizeof(uint32_t) + 2;
  int data_len = header->length - package_min;
  memcpy(data,(uint8_t*)header + sizeof(Header) + 2,data_len);

  return data_len;
}
void handle_mouse_keyboard_event(key_mouse_event * event)
{
  switch (event->event_type)
  {
    case KEYBOARD_EVENT:
      keyboard_event(event->data);
          break;
    case MOUSE_MOVE_EVENT:
      mouse_move_event(event->data[0],event->data[1]);
          break;
    case MOUSE_PRESS_EVENT:
      mouse_press_event(event->data[0],event->data[1],event->data[2]);
      break;

  }
}
void from_mobile_callback(DJI::onboardSDK::CoreAPI * coreAPI, Header * header, UserData userData)
{
  int len = get_data_after_header(header,from_mobile_data);
  key_mouse_event event;
  memcpy(&event,from_mobile_data,len);
    handle_mouse_keyboard_event(&event);
  std::cout<< "data is |"<<(int)event.event_type<< "| "<<event.data[0]<<" "<<event.data[1]<<" "<<event.data[2] << "$"<< std::endl;
}
int setup_keyboard_event(CoreAPI * coreAPI)
{
    coreAPI->setFromMobileCallback(from_mobile_callback);
  init_keyboard_event();
  std::cout << "Setup callback!" << std::endl;
  return 1;
}
//! Main function for the Linux sample. Lightweight. Users can call their own API calls inside the Programmatic Mode else on Line 68. 
int main(int argc, char *argv[])
{
  printf("Hello,World\n");
  //! Instantiate a serialDevice, an API object, flight and waypoint objects and a read thread.
  LinuxSerialDevice* serialDevice = new LinuxSerialDevice(UserConfig::deviceName, UserConfig::baudRate);
  CoreAPI* api = new CoreAPI(serialDevice);
  Flight* flight = new Flight(api);

  //! Enable non-blocking callback thread mechanism
  api->nonBlockingCBThreadEnable = true;

  //! Initializes the read thread and the call back thread.
  LinuxThread readThread(api,2);
  LinuxThread CallbackThread(api,3);
  std::cout << "Init program" << std::endl;

  //! Setup
  int setupStatus = setupNonBlocking(serialDevice, api, &readThread, &CallbackThread);
  if (setupStatus == -1)
  {
    std::cout << "This program will exit now. \n";
    return 0;
  }

  api->getDroneVersion(1);
  activateNonBlocking(api);
  usleep(100000);
  //! We are successfully activated. Try to take control.

  if (setup_keyboard_event(api) != 0 )
  {

    while(true)
    {
      sleep(1);
    }
  }

  //! Cleanup
  int cleanupStatus = cleanupNonBlocking(serialDevice, api, flight, &readThread , &CallbackThread);
  if (cleanupStatus == -1)
  {
    std::cout << "Unable to cleanly destroy OSDK infrastructure. There may be residual objects in the system memory.\n";
    return 0;
  }
  std::cout << "Program exited successfully." << std::endl;

  return 0;
}

