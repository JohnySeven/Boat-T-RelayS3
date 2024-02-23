#pragma once
#include <Arduino.h>
#include "sensesp/signalk/signalk_put_request_listener.h"
#include "sensesp/system/valueconsumer.h"
#include "sensesp/system/valueproducer.h"
#include "sensesp/system/startable.h"
using namespace sensesp;

#define DATA_PIN                    7
#define CLOCK_PIN                   5
#define LATCH_PIN                   6
#define GREEN_LED_CH                6
#define RED_LED_CH                  7

class RelayChannel : public Startable,
                     public BoolProducer,
                     public BoolConsumer
{
    public:
        RelayChannel(int index)
        {
            relay_index_ = index;
        }
        void start() override;
        void set_input(bool new_value, uint8_t input_channel = 0) override;
    private:
        int relay_index_ = 0;
};

class Relay6
{
    public:
        bool begin();
        RelayChannel *get_relay(int index);
        void set_red_led(bool value);
        void set_green_led(bool value);

        void toggle_red_led()
        {
            set_red_led(!red_led_);
        }

        void toggle_green_led()
        {
            set_green_led(!green_led_);
        }
    private:
        RelayChannel *relay_channels[6];
        bool red_led_;
        bool green_led_;
};