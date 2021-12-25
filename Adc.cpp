#include "Adc.h"

enum class AdcChannel :uint8_t{ Ch0 = 0x01, Ch1 = 0x02, Ch2 = 0x04, Ch3 = 0x08, Ch4 = 0x10, Ch5 = 0x20, Ch6 = 0x40, Ch7 = 0x80};
enum class AdcClkSource : uint8_t { Hsi14mhz=0x0, Pclk_2=0x01, Pclk_4=0x02 };

class Adc
{
	ADC_TypeDef* adc;
	
public:
	Adc(ADC_TypeDef* _adc, const AdcClkSource & clk)
	{
		adc = _adc;
		
		RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; 
		if (static_cast<uint8_t>(clk) == 0x0)
		{
			RCC->CR2 |= RCC_CR2_HSI14ON; 
			while ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0){
				/* For robust implementation, add here time-out management */
			}
		}
		adc->CFGR2 &= ~0x3 << 30;
		adc->CFGR2 |= static_cast<uint8_t>(clk) << 30;
		
	}
	
	~Adc()
	{
		Disable();
	}
	
	void Enable()
	{
		/* (1) Ensure that ADRDY = 0 */
		/* (2) Clear ADRDY */
		/* (3) Enable the ADC */
		/* (4) Wait until ADC ready */
		if ((adc->ISR & ADC_ISR_ADRDY) != 0) /* (1) */
			adc->ISR |= ADC_ISR_ADRDY; /* (2) */
		adc->CR |= ADC_CR_ADEN; /* (3) */
		while ((adc->ISR & ADC_ISR_ADRDY) == 0) /* (4) */{/* For robust implementation, add here time-out management */	}
	}
	void Disable()
	{
		/* (1) Stop any ongoing conversion */
		/* (2) Wait until ADSTP is reset by hardware i.e. conversion is stopped */
		/* (3) Disable the ADC */
		/* (4) Wait until the ADC is fully disabled */
		adc->CR |= ADC_CR_ADSTP; /* (1) */
		while ((adc->CR & ADC_CR_ADSTP) != 0) /* (2) */	{/* For robust implementation, add here time-out management */}
		adc->CR |= ADC_CR_ADDIS; /* (3) */
		while ((adc->CR & ADC_CR_ADEN) != 0) /* (4) */{	/* For robust implementation, add here time-out management */}
	}
	void Calibration()
	{
		/* (1) Ensure that ADEN = 0 */
		/* (2) Clear ADEN by setting ADDIS*/
		/* (3) Clear DMAEN */
		/* (4) Launch the calibration by setting ADCAL */
		/* (5) Wait until ADCAL=0 */
		if ((adc->CR & ADC_CR_ADEN) != 0) /* (1) */
			adc->CR |= ADC_CR_ADDIS; /* (2) */
		while ((adc->CR & ADC_CR_ADEN) != 0) {/* For robust implementation, add here time-out management */}
		adc->CFGR1 &= ~ADC_CFGR1_DMAEN; /* (3) */
		adc->CR |= ADC_CR_ADCAL; /* (4) */
		while ((adc->CR & ADC_CR_ADCAL) != 0) /* (5) */{/* For robust implementation, add here time-out management */}
	}
	
	void SetChannel(AdcChannel & channel)
	{
		adc->CHSELR |= static_cast<uint8_t>(channel);
	}
	void ResetChannel(AdcChannel & channel)
	{
		adc->CHSELR &= ~static_cast<uint8_t>(channel);
	}
	void SamplingTime()
	{
		
	}
	void SelectMode()
	{
	}
	
	
private:
				
	
	
};

