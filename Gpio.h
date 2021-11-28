#pragma once

#include "stm32f0xx.h"
#define BITMASK 0x3
enum  class GpioMode : uint8_t { In = 0b00, Out = 0b01, Af = 0b10, Analog = 0b11 };
enum class GpioSpeed : uint8_t { Low = 0b00, Medium = 0b01, High = 0b10, Max = 0b11 };
enum class GpioPull : uint8_t { NoPull = 0b00, PullUp = 0b01, PullDown = 0b10 };
enum class GpioType : uint8_t { PushPull = 0b00, OpenDrain = 0b01 };

enum class Af:uint32_t {Af0 = 0x0, Af1 = 0x1, Af2 = 0x2, Af3 = 0x3, Af4 = 0x4, Af5 = 0x5, Af6 = 0x6, Af7 = 0x7 };
class Gpio
{
	GPIO_TypeDef* gpio;
	uint8_t pin;
	
public:
	Gpio(GPIO_TypeDef* Gpio, uint8_t PinNumber);
	~Gpio();
	void SetMode(GpioMode  Mode);
	void SetSpeed(GpioSpeed Speed);
	void SetOutType(GpioType Type);
	void SetPull(GpioPull Pull);
	void SetState(bool state);
	void ToggleState();
	bool GetState();
	void SetAlternateFunction(Af af);

private:
	
};