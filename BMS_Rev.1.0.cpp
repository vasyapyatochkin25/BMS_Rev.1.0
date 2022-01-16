#include <stm32f0xx_ll_utils.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "Gpio.h"
#include "Rcc.h"
#include "Adc.h"


#define CLK   48000000UL  // частота тактировани€ usart



const float OverVoltage = 3.65;
const float UnderVoltage = 2.7;
const float OverVoltageGist  = 3.4;
const float UnderVoltageGist = 3.0;


void USART_Init(uint32_t baudRate);
void USART_Str_Print(char *data);
void expRunningAverage(float newVal, float *filVal, float k);
void Adc_Read();
Rcc rcc(SrcClock::HSI);
Adc adc(ADC1, AdcClkSource::Hsi14mhz);

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
float B1, B2, B3, B4, Iakb, Vakb, bit = 0, Takb, I;
float ofset;
float B1filt;
float VDDA;
uint8_t OVP=0, UVP=0;
char strTx[64];
void USART1_IRQHandler(void)
{
	//Led.ToggleState();
}

int main(void)
{
	LL_InitTick(48000000, 1000);
	
	rcc.PllOn(PllSrcClok::HSI_DIV2, PllxInFreq::Pllx_12);
	

	Tx.SetSpeed(GpioSpeed::Max);
	//Tx.SetAlternateFunction(Af::Af1);
	GPIOA->AFR[1] |= 0x0110;
	Tx.SetMode(GpioMode::Af);
	Rx.SetSpeed(GpioSpeed::Max);
	//Rx.SetAlternateFunction(Af::Af1);
	Rx.SetMode(GpioMode::Af);
	
	Rx.SetPull(GpioPull::PullUp);
	//Tx.SetPull(GpioPull::PullUp);
	
	USART_Init(115200);
	
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
	LL_mDelay(10);
	adc.Calibration();
	LL_mDelay(10);
	adc.Enable();
	adc.SetChannel(AdcChannel::Ch0);
	adc.SetChannel(AdcChannel::Ch1);
	adc.SetChannel(AdcChannel::Ch2);
	adc.SetChannel(AdcChannel::Ch3);
	adc.SetChannel(AdcChannel::Ch4);
	adc.SetChannel(AdcChannel::Ch5);
	adc.SetChannel(AdcChannel::Ch6);
	adc.SetChannel(AdcChannel::Ch17);
	
	adc.SamplingTime(AdcSamplinTime::cycle71_5);
	adc.EnableVrefint();
	

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
		sprintf(strTx, "Vakb=%f", Vakb);
		USART_Str_Print(strTx);
		USART_Str_Print((char*)"\r\n");
		Led.ToggleState();
		LL_mDelay(1);	
		
	}
}



// бегущее среднее
void expRunningAverage(float newVal, float *filVal, float k) {    // коэффициент фильтрации, 0.0-1.0
	*filVal += (newVal - *filVal) * k;
}

void Write(uint8_t data)
{
	while (!(USART1->ISR&(1 << 7)))
		continue;
	USART1->TDR = data;
}

void Adc_Read()
{
	
	const uint16_t null = 2450;
	const float Ikoef=0.10416;
	adc.ReadSelectedChannel(ADC_Result);
	
	
/*
    VDDA = (3300 * 1536) / ADC_Result[7];
	B1 = ADC_Result[0] * bit * 0.00150883218;
	B2 = ADC_Result[1] * bit * 0.00150883218;
	B3 = ADC_Result[2] * bit * 0.00150883218;
	B4 = ADC_Result[3] * bit * 0.00150883218;
	Iakb = ADC_Result[4] * bit;
	Vakb = ADC_Result[5] * bit * 0.0050015;
	Takb = ADC_Result[6];
*/
	
	expRunningAverage(((3300 * VREFINT_CAL) / ADC_Result[7]), &VDDA, 0.1);
	bit = (float)VDDA / 4095;
	expRunningAverage((ADC_Result[0] * bit * 0.00150883218), &B1,0.1);
	expRunningAverage((ADC_Result[1] * bit * 0.00150883218), &B2, 0.1);
	expRunningAverage((ADC_Result[2] * bit * 0.00150883218), &B3, 0.1);
	expRunningAverage((ADC_Result[3] * bit * 0.00150883218), &B4, 0.1);
	
	expRunningAverage((ADC_Result[4] * bit), &I, 0.1);
	expRunningAverage((ADC_Result[5] * bit * 0.0050015), &Vakb, 0.1);
	expRunningAverage((ADC_Result[6] * bit), &Takb, 0.1);

	 
	ofset = I - null;
	Iakb = ofset*Ikoef;
	
	
	
	
}
void USART_Init(uint32_t baudRate)
{
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;   //Enable clock Usart
	//RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	//¬ключаем тактирование GPIOA
	//¬ключаем тактирование USART1
/*
  GPIOA->MODER   |= GPIO_MODER_MODER9_1;          
  GPIOA->MODER   |= GPIO_MODER_MODER10_1;        
 GPIOA->PUPDR |= GPIO_PUPDR_PUPDR10_0;
  GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9;      
  GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10;
  GPIOA->AFR[1]  |= 0x0110; 
*/
	
	//RCC->CFGR3 &= ~RCC_CFGR3_USART1SW;
	//RCC->CFGR3 |=  RCC_CFGR3_USART1SW;    //HSI selected as USART1 clock
  
                //¬ключаем USART1
	USART1->BRR = (CLK /  2) /baudRate;
	USART1->CR1 |= USART_CR1_TE;                //¬ключаем передатчик USART1
	USART1->CR1 |= USART_CR1_RE;                //¬ключаем приемник USART1
	USART1->CR1 |= USART_CR1_UE;
	//USART1->CR1  |= USART_CR1_RXNEIE;        //прерывание по приему данных
	//NVIC_EnableIRQ(USART1_IRQn);            //разрешить прерывани€ от USART1
  
}


void USART_Str_Print(char *data)
{		
	unsigned char size = strlen(data);
	
	for (uint8_t i = 0; i < (size); i++)
	{
		while (!(USART1->ISR&(1 << 7)))
			continue;
		USART1->TDR = data[i];
	}
}

