/**
 *******************************************************************************
 * @file       peri.h
 * @version    V1.0
 * @date       2014.12.31
 * @brief      Peripherals header file
 * @details    This file including some defines and declares related to Peripherals.
 *******************************************************************************
 * @copy
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *      * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution.
 *      * Neither the name of the <ORGANIZATION> nor the names of its
 *  contributors may be used to endorse or promote products derived
 *  from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGE.
 *
 * COPYRIGHT 2014 Fuzhou Rockchip Electronics Co., Ltd
 *******************************************************************************
 */

#ifndef __PERI_H
#define __PERI_H

#include <io.h>
#include <sizes.h>
#include <CoOS.h>
#include <stdio.h>

#include <sram.h>
#include <irq.h>
#include <uart.h>
#include <cru.h>
#include <mailbox.h>
#include <ddr_rk3368.h>

typedef enum IRQn {
	SysTick_IRQn		= -1,
#ifdef RK3368
	MBOX0_IRQn		= 138,
#elif RK3366
	MBOX0_IRQn		= 128,
#elif RK3399
	MBOX0_IRQn		= 140,
#else

#endif
} IRQn_Type;

#ifdef RK3399
#define IRQ_MBOX		17
#endif

#define __NVIC_PRIO_BITS	4

#include <core_cm3.h>

#endif
