
#include <stm32f0xx_ll_utils.h>
#include "Gpio.h"
#include <stdint.h>

#define F_CPU 8000000L
#define  VREFINT_CAL *((uint16_t*)0x1FFFF7BA)

const float OverVoltage = 3.65;
const float UnderVoltage = 2.7;
const float OverVoltageGist  = 3.4;
const float UnderVoltageGist = 3.0;
void Write(uint8_t data);
void AdcCalibration();
void AdcClockEnable();
void AdcEnable();
void Adc_Read();
Gpio Led(GPIOF, 1);
Gpio Button(GPIOF, 0);
Gpio Contactor(GPIOB, 1);

Gpio Tx(GPIOA, 9);
Gpio Rx(GPIOA, 10);


Gpio A0(GPIOA, 0);
Gpio A1(GPIOA, 1);
Gpio A2(GPIOA, 2);
Gpio A3(GPIOA, 3);
Gpio A4(GPIOA, 4);
Gpio A5(GPIOA, 5);
Gpio A6(GPIOA, 6);
uint16_t ADC_Result[8];
float B1, B2, B3, B4, Iakb, Vakb, bit = 0, Takb;

uint32_t VDDA, VrefintData;
uint8_t OVP=0, UVP=0;
int main(void)
{
	//DBGMCU->CR |= DBGMCU_CR_DBG_STANDBY;
	LL_InitTick(8000000, 1000);
	/*
	Tx.SetSpeed(GpioSpeed::Max);
	Tx.SetAlternateFunction(Af::Af1);
	Rx.SetSpeed(GpioSpeed::Max);
	Rx.SetAlternateFunction(Af::Af1);

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;    //Enable clock Usart
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	LL_mDelay(10);
	GPIOA->MODER |= (1 << 7) | (1 << 5);     //Set mode Alternete function
	GPIOA->AFR[0] |= (1 << 12) | (1 << 8);    // Selection Alternete function 
	LL_mDelay(10);
	USART1->BRR = 8000000 / 9600;    //Baud rate 
	USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;	
	USART1->CR1 |= (1 << 8);
	LL_mDelay(10);
	*/
	
	Button.SetMode(GpioMode::In);
	Button.SetPull(GpioPull::PullUp);
	
	Led.SetMode(GpioMode::Out);
	Contactor.SetMode(GpioMode::Out);
	
	A0.SetMode(GpioMode::Analog);
	A1.SetMode(GpioMode::Analog);
	A2.SetMode(GpioMode::Analog);
	A3.SetMode(GpioMode::Analog);
	A4.SetMode(GpioMode::Analog);
	A5.SetMode(GpioMode::Analog);
	A6.SetMode(GpioMode::Analog);
	
	AdcClockEnable();
	AdcCalibration();
    AdcEnable();
    
	
/* (1) Select HSI14 by writing 00 in CKMODE (reset value) */
/* (2) Select CHSEL0, CHSEL9, CHSEL10 andCHSEL17 for VRefInt */
/* (3) Select a sampling mode of 111 i.e. 239.5 ADC clk to be greaterthan 17.1us */
/* (4) Wake-up the VREFINT (only for VBAT, Temp sensor and VRefInt) */
	
	ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE; /* (1) */
	ADC1->CHSELR = ADC_CHSELR_CHSEL0 | ADC_CHSELR_CHSEL1 | ADC_CHSELR_CHSEL2 | ADC_CHSELR_CHSEL3 | ADC_CHSELR_CHSEL4|
	ADC_CHSELR_CHSEL5|ADC_CHSELR_CHSEL6|ADC_CHSELR_CHSEL17; /* (2) */
	ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1;//| ADC_SMPR_SMP_2; /* (3) */
	ADC->CCR |= ADC_CCR_VREFEN; /* (4) */
	
	LL_mDelay(1000);
	
	
		Adc_Read();
		if ((B1 < OverVoltage) && (B2 < OverVoltage) && (B3 < OverVoltage) && (B4 < OverVoltage) && (B1 > UnderVoltage) && (B2 > UnderVoltage) && (B3 > UnderVoltage) && (B4 > UnderVoltage))
		{
			Contactor.SetState(true);
		}	
		
	
		
		
	for (;;)
	{
		Adc_Read();
		
		if (OVP)
		{
			if ((B1 < OverVoltageGist) && (B2 < OverVoltageGist) && (B3 < OverVoltageGist) && (B4 < OverVoltageGist))
			{
				Contactor.SetState(true);
				OVP = 0;
			}
		}
		else
			if ((B1 > OverVoltage) || (B2 > OverVoltage) || (B3 > OverVoltage) || (B4 > OverVoltage))
			{
				Contactor.SetState(false);
				OVP = 1;
			}
		
		
		if (UVP)
		{
			if ((B1 > UnderVoltageGist) && (B2 > UnderVoltageGist) && (B3 > UnderVoltageGist) && (B4 > UnderVoltageGist))
			{
				Contactor.SetState(true);
				UVP = 0;
			}
		}
		else
			if ((B1 < UnderVoltage) || (B2 < UnderVoltage) || (B3 < UnderVoltage) || (B4 < UnderVoltage))
			{
				Contactor.SetState(false);
				UVP = 1;
			}
				
		Led.SetState(true);
		LL_mDelay(50);
		Led.SetState(false);
		LL_mDelay(50);	
	}
}


void Write(uint8_t data)
{
	while (!(USART1->ISR&(1 << 7)))
		continue;
	USART1->TDR = data;
}

void Adc_Read()
{
	ADC1->CR |= ADC_CR_ADSTART; 
	for (uint8_t i = 0; i < 8; i++)
	{
		while ((ADC1->ISR & ADC_ISR_EOC) == 0) {}
		ADC_Result[i] = ADC1->DR; 
	}
	VDDA = (3300 * 1536) / ADC_Result[7];
	VDDA = VDDA / 1.0179659;
	bit = (float)VDDA / 4095;
		
	B1 = ADC_Result[0] * bit * 0.00150883218;
	B2 = ADC_Result[1] * bit * 0.00150883218;
	B3 = ADC_Result[2] * bit * 0.00150883218;
	B4 = ADC_Result[3] * bit * 0.00150883218;
	Iakb = ADC_Result[4];
	Vakb = ADC_Result[5] * bit * 0.0050015;
	Takb = ADC_Result[6];
		
}
void AdcEnable()
{
	/* (1) Ensure that ADRDY = 0 */
/* (2) Clear ADRDY */
/* (3) Enable the ADC */
/* (4) Wait until ADC ready */
	if ((ADC1->ISR & ADC_ISR_ADRDY) != 0) /* (1) */
	{
		ADC1->ISR |= ADC_ISR_ADRDY; /* (2) */
	}
	ADC1->CR |= ADC_CR_ADEN; /* (3) */
	while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) /* (4) */
	{
		/* For robust implementation, add here time-out management */
	}
}
void AdcCalibration()
{
	/* (1) Ensure that ADEN = 0 */
/* (2) Clear ADEN by setting ADDIS*/
/* (3) Clear DMAEN */
/* (4) Launch the calibration by setting ADCAL */
/* (5) Wait until ADCAL=0 */
	if ((ADC1->CR & ADC_CR_ADEN) != 0) /* (1) */
	{
		ADC1->CR |= ADC_CR_ADDIS; /* (2) */
	}
	while ((ADC1->CR & ADC_CR_ADEN) != 0)
	{
		/* For robust implementation, add here time-out management */
	}
	ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN; /* (3) */
	ADC1->CR |= ADC_CR_ADCAL; /* (4) */
	while ((ADC1->CR & ADC_CR_ADCAL) != 0) /* (5) */
	{
		/* For robust implementation, add here time-out management */
	}
}

void AdcClockEnable()
{
/* This code selects the HSI14 as clock source. */
/* (1) Enable the peripheral clock of the ADC */
/* (2) Start HSI14 RC oscillator */
/* (3) Wait HSI14 is ready */
/* (4) Select HSI14 by writing 00 in CKMODE (reset value) */
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; /* (1) */
	RCC->CR2 |= RCC_CR2_HSI14ON; /* (2) */
	while ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0) /* (3) */
	{
		/* For robust implementation, add here time-out management */
	}
	
}