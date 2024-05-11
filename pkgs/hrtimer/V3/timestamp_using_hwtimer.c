/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/


#include "hrtimer.h"
#include "board.h"

/*
 * Period of the free-running counter, in microseconds.
 */
#define HRT_COUNTER_PERIOD 65536

/*
 * Scaling factor(s) for the free-running counter; convert an input
 * in counts to a time in microseconds.
 */
#define HRT_COUNTER_SCALE(_c) (_c)

/*
 * HRT clock must be a multiple of 1MHz greater than 1MHz
 */
#if (HRT_TIMER_CLOCK % 1000000) != 0
#   error HRT_TIMER_CLOCK must be a multiple of 1MHz
#endif
#if HRT_TIMER_CLOCK <= 1000000
#   error HRT_TIMER_CLOCK must be greater than 1MHz
#endif

static volatile uint64_t _timebase_us = 0;

extern TIM_HandleTypeDef htim13;

void MX_TIM13_Init(void) {
    TIM_OC_InitTypeDef sConfigOC = {0};

    /* USER CODE BEGIN TIM13_Init 1 */

    /* USER CODE END TIM13_Init 1 */
    htim13.Instance               = TIM13;
    htim13.Init.Prescaler         = 239;
    htim13.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim13.Init.Period            = 0xffff;
    htim13.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim13.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    if (HAL_TIM_Base_Init(&htim13) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIM_OC_Init(&htim13) != HAL_OK) {
        Error_Handler();
    }
    sConfigOC.OCMode     = TIM_OCMODE_TIMING;
    sConfigOC.Pulse      = 1000;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_OC_ConfigChannel(&htim13, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }

    /* USER CODE BEGIN TIM13_Init 2 */
    HAL_TIM_Base_MspInit(&htim13);

    HAL_TIM_OC_Start_IT(&htim13, TIM_CHANNEL_1);

    HAL_TIM_Base_Start_IT(&htim13);
    /* USER CODE END TIM13_Init 2 */
}

void TIM8_UP_TIM13_IRQHandler(void) {
    /* enter interrupt */
    rt_interrupt_enter();
    /* Capture compare 1 event */
    if (__HAL_TIM_GET_FLAG(&htim13, TIM_FLAG_CC1) != RESET) {
        uint8_t status = 0;
        hrt_tim_isr(&status);
        __HAL_TIM_SET_COUNTER(&htim13, 0);
        if (__HAL_TIM_GET_IT_SOURCE(&htim13, TIM_IT_CC1) != RESET) {
            __HAL_TIM_CLEAR_IT(&htim13, TIM_IT_CC1);
            htim13.Channel = HAL_TIM_ACTIVE_CHANNEL_1;
        }
    }

    /* TIM Update event */
    if (__HAL_TIM_GET_FLAG(&htim13, TIM_FLAG_UPDATE) != RESET) {
        if (__HAL_TIM_GET_IT_SOURCE(&htim13, TIM_IT_UPDATE) != RESET) {
            _timebase_us   += HRT_COUNTER_PERIOD;
            uint8_t status  = 1;
            hrt_tim_isr(&status);
            __HAL_TIM_CLEAR_IT(&htim13, TIM_IT_UPDATE);
#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)
            htim->PeriodElapsedCallback(htim);
#else
            HAL_TIM_PeriodElapsedCallback(&htim13);
#endif /* USE_HAL_TIM_REGISTER_CALLBACKS */
        }
    }
    /* leave interrupt */
    rt_interrupt_leave();
}

hrt_abstime hrt_absolute_time(void) {
    return _timebase_us + __HAL_TIM_GET_COUNTER(&htim13);
}

/**
 * Initialise the timer we are going to use.
 *
 * We expect that we'll own one of the reduced-function STM32 general
 * timers, and that we can use channel 1 in compare mode.
 */
static int hrt_tim_init() {
    MX_TIM13_Init();
    return 0;
}

INIT_COMPONENT_EXPORT(hrt_tim_init);
