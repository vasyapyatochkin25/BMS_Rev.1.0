/*
 * Rcc.h
 *
 *  Created on: Dec 22, 2021
 *      Author: TKA128
 */

#ifndef RCC_H_
#define RCC_H_

#include "stm32f030x6.h"

enum class SrcClock: uint32_t {HSI = 0x1, HSE = 0x10000};
enum class PllSrcClok:uint8_t{HSI_DIV2,HSE_PREDIV};
enum class PllxInFreq:uint8_t{Pllx_2, Pllx_3, Pllx_4, Pllx_5, Pllx_6, Pllx_7, Pllx_8, Pllx_9, Pllx_10, Pllx_11, Pllx_12, Pllx_13,Pllx_14,Pllx_15,Pllx_16};


class Rcc{

	public:

		Rcc(const SrcClock & Clock);
		void PllOn(const PllSrcClok & SrcPll, const PllxInFreq & Pllx );

	private:

};










#endif /* RCC_H_ */
