#include "relay6.h"
#include <Wire.h>
#include "sensesp_app.h"
#include <ShiftRegister74HC595.h>


ShiftRegister74HC595<1> *control;

bool Relay6::begin()
{

    control = new ShiftRegister74HC595<1>(DATA_PIN, CLOCK_PIN, LATCH_PIN);
    control->setAllLow();

    for (int i = 0; i < 6; i++)
    {
        relay_channels[i] = new RelayChannel(i);
    }

    return true;
}

RelayChannel *Relay6::get_relay(int index)
{
    if (index >= 0 && index < 6)
    {
        return relay_channels[index];
    }
    else
    {
        return NULL;
    }
}

void Relay6::set_red_led(bool value)
{
    control->set(RED_LED_CH, value);
    red_led_ = value;
}

void Relay6::set_green_led(bool value)
{
    control->set(GREEN_LED_CH, value);
    green_led_ = value;
}

void RelayChannel::start()
{
    output = false;
    notify();
    ReactESP::app->onRepeat(5000, [this]()
    {
        this->notify();
    });
}

void RelayChannel::set_input(bool new_value, uint8_t input_channel)
{
    control->set(RED_LED_CH, HIGH);
    debugI("Relay set_input %d = %d", relay_index_, new_value);
    control->set(relay_index_, new_value);
    emit(new_value);
    control->set(RED_LED_CH, LOW);
}