/*
 * Rcc.cpp
 *
 *  Created on: Dec 22, 2021
 *      Author: TKA128
 */
#include "Rcc.h"

	Rcc::Rcc(const SrcClock & Clock)
		{
			FLASH->ACR|=FLASH_ACR_LATENCY;

			RCC->CR|= static_cast<uint32_t>(Clock);
			while(!(RCC->CR & (static_cast<uint32_t>(Clock)<<1))) {/*timeout */}

			switch(static_cast<uint32_t>(Clock))
			{
				case static_cast<uint32_t>(SrcClock::HSE):
					RCC->CFGR|= RCC_CFGR_SW_HSE;
					while(!(RCC->CFGR & RCC_CFGR_SWS_HSE)){}
				break;
				case static_cast<uint32_t>(SrcClock::HSI):
					RCC->CFGR|= RCC_CFGR_SW_HSI;
					while(!(RCC->CFGR & RCC_CFGR_SWS_HSI)){}
				break;
			}

		}


		void Rcc::PllOn(const PllSrcClok & SrcPll, const PllxInFreq & Pllx )
		{
			switch(static_cast<uint8_t>(SrcPll))
			{
				case static_cast<uint8_t>(PllSrcClok::HSI_DIV2):
				RCC->CFGR&=~RCC_CFGR_PLLSRC_HSE_PREDIV;
				break;
				case static_cast<uint8_t>(PllSrcClok::HSE_PREDIV):
				RCC->CFGR|= RCC_CFGR_PLLSRC_HSE_PREDIV;
				break;
			}
			RCC->CFGR&=~(0xf<<18);
			RCC->CFGR|= (static_cast<uint8_t>(Pllx) << 18);

			RCC->CR&=~RCC_CR_PLLON;
			RCC->CR|=RCC_CR_PLLON;
			while(!(RCC->CR & RCC_CR_PLLRDY)){}

			RCC->CFGR&=~RCC_CFGR_SW;
			RCC->CFGR|=RCC_CFGR_SW_PLL;
			while(!(RCC->CFGR & RCC_CFGR_SWS_PLL)){}
		}




