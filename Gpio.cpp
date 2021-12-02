#include "Gpio.h"


Gpio::Gpio(GPIO_TypeDef* Gpio, uint8_t PinNumber)
{
	gpio = Gpio;
	pin = PinNumber;
	if (Gpio == GPIOA) RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	if (Gpio == GPIOB) RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	if (Gpio == GPIOC) RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	if (Gpio == GPIOD) RCC->AHBENR |= RCC_AHBENR_GPIODEN;
	if (Gpio == GPIOF) RCC->AHBENR |= RCC_AHBENR_GPIOFEN;
	
	SetMode(GpioMode::Out);
	SetOutType(GpioType::PushPull);
	SetSpeed(GpioSpeed::High);
	SetPull(GpioPull::NoPull);
	SetState(false);
}
	
Gpio::~Gpio()
{
	
}

	
void Gpio::SetMode(GpioMode  Mode)
{
gpio->MODER &= ~(BITMASK << pin * 2);
gpio->MODER |= (static_cast<uint8_t>(Mode) << pin * 2);
}
	
void Gpio::SetSpeed(GpioSpeed Speed)
{
	gpio->OSPEEDR &= ~(BITMASK << pin * 2);
	gpio->OSPEEDR |= (static_cast<uint8_t>(Speed) << pin * 2);
}
void Gpio::SetOutType(GpioType Type)
{
	gpio->OTYPER &= ~(BITMASK << pin);
	gpio->OTYPER |= (static_cast<uint8_t>(Type) << pin);
}

void Gpio::SetPull(GpioPull Pull)
{
	gpio->PUPDR &= ~(BITMASK << pin * 2);
	gpio->PUPDR |= (static_cast<uint8_t>(Pull) << pin * 2);
}
void Gpio::SetState(bool state)
{
	state ? gpio->ODR |= (1 << pin) : gpio->ODR &= ~(1 << pin);
}
void Gpio::ToggleState()
{
	if (GetState())
	{
		SetState(false);
	}
	else
	{
		SetState(true);
	}
}
bool Gpio::GetState()
{
	return gpio->IDR & (1 << pin); 
}
void Gpio::SetAlternateFunction(Af af)
{
	if (pin <= 7)
	{
		gpio->AFR[0] &= ~0xf << (pin * 4); 
		gpio->AFR[0] |= static_cast<uint32_t>(af) << 4*pin;
	}
	if (pin > 7)
	{
		gpio->AFR[1] &= ~0xf << ((pin>>0xf) * 4); 
		gpio->AFR[1] |= static_cast<uint32_t>(af) << (4*(pin>>0xf));
	}
}
	
