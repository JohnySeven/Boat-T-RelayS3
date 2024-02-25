#include <Arduino.h>
#include <EEPROM.h>
#include "sensesp_app.h"
#include "sensesp_app_builder.h"
#include "sensesp/signalk/signalk_output.h"
#include "sensesp/signalk/signalk_listener.h"
#include "sensesp/signalk/signalk_value_listener.h"
#include "sensesp/sensors/digital_output.h"
#include "sensesp/transforms/threshold.h"
#include "sensesp/transforms/lambda_transform.h"
#include "relay6.h"
// #include "SwitchPutRequestListener.h"
//#include "PwmPutRequestListener.h"
#include "sensesp/system/pwm_output.h"
#include "sensesp/sensors/sensor.h"

reactesp::ReactESP app;
Relay6 relay;
bool volatile button_down = false;

void initialize_relay_channel(String sk_path, String config, int index)
{
  String name = "Relay#" + String(index);
  RelayChannel *channel = relay.get_relay(index);
  channel->connect_to(new SKOutputBool(sk_path, config, new SKMetadata("bool", "Relay" + String(index))));
  channel->connect_to(new UIOutput<bool>(name));
  auto button = new UIButton("Toggle relay " + String(index), name, false);
  button->attach([channel]()
                 { channel->set_input(true); });

  auto listener = new BoolSKPutRequestListener(sk_path);
  listener->connect_to(channel);
}

// SensESP builds upon the ReactESP framework. Every ReactESP application
// defines an "app" object vs defining a "main()" method.
void setup()
{
// Some initialization boilerplate when in debug mode...
#ifndef SERIAL_DEBUG_DISABLED
  Serial.begin(115200);
  // A small arbitrary delay is required to let the
  // serial port catch up
  delay(100);
  Debug.setSerialEnabled(false);
#endif

  // Create the global SensESPApp() object.
  auto *builder = (new SensESPAppBuilder())
                      ->enable_ip_address_sensor()
                      ->set_hostname("relays_node")
                      ->set_sk_server("192.168.89.132", 3000);

  sensesp_app = builder->get_app();

  debugI("Initializing LilyGO T-Relay...");

  if (!relay.begin())
  {
    debugI("Failed to initialize Relay6!");
  }

  relay.set_red_led(true);

  initialize_relay_channel("electrical.switches.relay0", "/sk/relay0", 0);
  initialize_relay_channel("electrical.switches.relay1", "/sk/relay1", 1);
  initialize_relay_channel("electrical.switches.relay2", "/sk/relay2", 2);
  initialize_relay_channel("electrical.switches.relay3", "/sk/relay3", 3);
  initialize_relay_channel("electrical.switches.relay4", "/sk/relay4", 4);
  initialize_relay_channel("electrical.switches.relay5", "/sk/relay5", 5);

  sensesp_app->start();

  app.onRepeat(1000, []()
  {
    auto status = sensesp_app->get_system_status_controller()->get();

    if(status == SystemStatus::kWSConnected)
    {
      relay.toggle_green_led();
      relay.set_red_led(false);
    }
    else if(status == SystemStatus::kWSAuthorizing || status == SystemStatus::kWifiDisconnected || status == SystemStatus::kWSConnecting)
    {
      relay.toggle_red_led();
    }
    else
    {
      relay.set_red_led(true);
      relay.set_green_led(false);
    } 
  });
}

void loop()
{
  app.tick();
}