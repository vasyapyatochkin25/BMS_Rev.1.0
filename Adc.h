#pragma once
#include "stm32f0xx.h"
#define  VREFINT_CAL *((uint16_t*)0x1FFFF7BA)
enum class AdcChannel :uint32_t {
	Ch0  = 0x01,
	Ch1  = 0x02,
	Ch2  = 0x04,
	Ch3  = 0x08,
	Ch4  = 0x10,
	Ch5  = 0x20,
	Ch6  = 0x40,
	Ch7  = 0x80,
	Ch8  = 0x100,
	Ch9  = 0x200,
	Ch10 = 0x400,
	Ch11 = 0x800,
	Ch12 = 0x1000,
	Ch13 = 0x2000,
	Ch14 = 0x4000,
	Ch15 = 0x8000,
	Ch16 = 0x10000,
	Ch17 = 0x20000
 };
enum class AdcClkSource : uint8_t { Hsi14mhz = 0x0, Pclk_2 = 0x01, Pclk_4 = 0x02 };
enum class AdcSamplinTime : uint8_t{ cycle1_5 = 0x0, cycle7_5 = 0x1, cycle13_5 = 0x2, cycle28_5 = 0x3, cycle41_5 = 0x4, cycle55_5 = 0x5, cycle71_5 = 0x6, cycle239_5 = 0x7 };


class Adc
{
	ADC_TypeDef* adc;
	
public:
	Adc(ADC_TypeDef* _adc, const AdcClkSource & clk);
	~Adc();
	void Enable();
	void Disable();
	void Calibration();
	void SetChannel(const AdcChannel & channel);
	void ResetChannel(const AdcChannel & channel);
	void SamplingTime(const AdcSamplinTime& Time);
	void EnableVrefint();
	void EnableTsens();
	void SelectMode();
	void ReadSelectedChannel(uint16_t *analogData);
private:
	uint8_t countAdcChannel = 0;
};


