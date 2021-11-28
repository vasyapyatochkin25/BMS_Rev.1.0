#include <stm32f0xx_ll_bus.h>
#include <stm32f0xx_ll_gpio.h>
#include <stm32f0xx_ll_utils.h>
#include "Gpio.h"


Gpio Led(GPIOA,4);
Gpio Button(GPIOA, 3);

int main(void)
{
	LL_InitTick(8000000, 1000);
	Button.SetMode(GpioMode::In);
	Button.SetPull(GpioPull::PullUp);
	
	
	for (;;)
	{
		if (!Button.GetState())
		{
			Led.SetState(true);
			LL_mDelay(100);
			Led.SetState(false);
			LL_mDelay(100);	
		}
	}
}
