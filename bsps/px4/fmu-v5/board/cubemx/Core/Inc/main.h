/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SPI4_SCK_SENSOR2_Pin GPIO_PIN_2
#define SPI4_SCK_SENSOR2_GPIO_Port GPIOE
#define UART8_TX_Pin GPIO_PIN_1
#define UART8_TX_GPIO_Port GPIOE
#define UART8_RX_Pin GPIO_PIN_0
#define UART8_RX_GPIO_Port GPIOE
#define I2C1_SCL_GPS1_Pin GPIO_PIN_8
#define I2C1_SCL_GPS1_GPIO_Port GPIOB
#define SPI6_MOSI_EXTERNAL2_Pin GPIO_PIN_5
#define SPI6_MOSI_EXTERNAL2_GPIO_Port GPIOB
#define USART6_TX_TELEM3_Pin GPIO_PIN_14
#define USART6_TX_TELEM3_GPIO_Port GPIOG
#define SPI6_SCK_EXTERNAL2_Pin GPIO_PIN_13
#define SPI6_SCK_EXTERNAL2_GPIO_Port GPIOG
#define FMU_CAP2_Pin GPIO_PIN_3
#define FMU_CAP2_GPIO_Port GPIOB
#define SPI1_MOSI_SENSOR1_Pin GPIO_PIN_7
#define SPI1_MOSI_SENSOR1_GPIO_Port GPIOD
#define SDMMC1_CLK_Pin GPIO_PIN_12
#define SDMMC1_CLK_GPIO_Port GPIOC
#define CAN3_TX_Pin GPIO_PIN_15
#define CAN3_TX_GPIO_Port GPIOA
#define JTAG_SWCLK_Pin GPIO_PIN_14
#define JTAG_SWCLK_GPIO_Port GPIOA
#define JTAG_SWDIO_Pin GPIO_PIN_13
#define JTAG_SWDIO_GPIO_Port GPIOA
#define BUZZER_1_Pin GPIO_PIN_5
#define BUZZER_1_GPIO_Port GPIOE
#define SPI4_MOSI_SENSOR2_Pin GPIO_PIN_6
#define SPI4_MOSI_SENSOR2_GPIO_Port GPIOE
#define I2C1_SDA_GPS1_Pin GPIO_PIN_9
#define I2C1_SDA_GPS1_GPIO_Port GPIOB
#define USART1_RX_GPS1_Pin GPIO_PIN_7
#define USART1_RX_GPS1_GPIO_Port GPIOB
#define USART1_TX_GPS1_Pin GPIO_PIN_6
#define USART1_TX_GPS1_GPIO_Port GPIOB
#define USART6_CTS_TELEM3_Pin GPIO_PIN_15
#define USART6_CTS_TELEM3_GPIO_Port GPIOG
#define SPI6_MISO_EXTERNAL2_Pin GPIO_PIN_12
#define SPI6_MISO_EXTERNAL2_GPIO_Port GPIOG
#define SPI1_SCK_SENSOR1_Pin GPIO_PIN_11
#define SPI1_SCK_SENSOR1_GPIO_Port GPIOG
#define USART2_RX_TELEM1_Pin GPIO_PIN_6
#define USART2_RX_TELEM1_GPIO_Port GPIOD
#define UART4_RX_I2C2_Pin GPIO_PIN_0
#define UART4_RX_I2C2_GPIO_Port GPIOD
#define SDMMC1_D3_Pin GPIO_PIN_11
#define SDMMC1_D3_GPIO_Port GPIOC
#define SDMMC1_D2_Pin GPIO_PIN_10
#define SDMMC1_D2_GPIO_Port GPIOC
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define RC_INPUT_Pin GPIO_PIN_5
#define RC_INPUT_GPIO_Port GPIOI
#define USART6_RX_TELEM3_Pin GPIO_PIN_9
#define USART6_RX_TELEM3_GPIO_Port GPIOG
#define USART2_TX_TELEM1_Pin GPIO_PIN_5
#define USART2_TX_TELEM1_GPIO_Port GPIOD
#define UART4_TX_I2C2_Pin GPIO_PIN_1
#define UART4_TX_I2C2_GPIO_Port GPIOD
#define SPI2_MOSI_FRAM_Pin GPIO_PIN_3
#define SPI2_MOSI_FRAM_GPIO_Port GPIOI
#define SPI2_MISO_FRAM_Pin GPIO_PIN_2
#define SPI2_MISO_FRAM_GPIO_Port GPIOI
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USART2_RTS_TELEM1_Pin GPIO_PIN_4
#define USART2_RTS_TELEM1_GPIO_Port GPIOD
#define USART2_CTS_TELEM1_Pin GPIO_PIN_3
#define USART2_CTS_TELEM1_GPIO_Port GPIOD
#define SDMMC1_CMD_Pin GPIO_PIN_2
#define SDMMC1_CMD_GPIO_Port GPIOD
#define SPI2_SCK_FRAM_Pin GPIO_PIN_1
#define SPI2_SCK_FRAM_GPIO_Port GPIOI
#define FMU_CH2_Pin GPIO_PIN_10
#define FMU_CH2_GPIO_Port GPIOA
#define I2C2_SDA_UART4_Pin GPIO_PIN_0
#define I2C2_SDA_UART4_GPIO_Port GPIOF
#define CAN1_TX_Pin GPIO_PIN_13
#define CAN1_TX_GPIO_Port GPIOH
#define VBUS_Pin GPIO_PIN_9
#define VBUS_GPIO_Port GPIOA
#define CAN1_SILENT_S0_Pin GPIO_PIN_2
#define CAN1_SILENT_S0_GPIO_Port GPIOH
#define SDMMC1_D1_Pin GPIO_PIN_9
#define SDMMC1_D1_GPIO_Port GPIOC
#define CAN3_RX_Pin GPIO_PIN_8
#define CAN3_RX_GPIO_Port GPIOA
#define SDMMC1_D0_Pin GPIO_PIN_8
#define SDMMC1_D0_GPIO_Port GPIOC
#define nLED_BLUE_Pin GPIO_PIN_7
#define nLED_BLUE_GPIO_Port GPIOC
#define I2C2_SCL_UART4_Pin GPIO_PIN_1
#define I2C2_SCL_UART4_GPIO_Port GPIOF
#define USART6_RTS_TELEM3_Pin GPIO_PIN_8
#define USART6_RTS_TELEM3_GPIO_Port GPIOG
#define nLED_GREEN_Pin GPIO_PIN_6
#define nLED_GREEN_GPIO_Port GPIOC
#define VDD_3V3_SD_CARD_EN_Pin GPIO_PIN_7
#define VDD_3V3_SD_CARD_EN_GPIO_Port GPIOG
#define SPI5_SCK_EXTERNAL1_Pin GPIO_PIN_7
#define SPI5_SCK_EXTERNAL1_GPIO_Port GPIOF
#define UART7_RX_DEBUG_Pin GPIO_PIN_6
#define UART7_RX_DEBUG_GPIO_Port GPIOF
#define UI_LED_BLUE_Pin GPIO_PIN_12
#define UI_LED_BLUE_GPIO_Port GPIOH
#define SPI5_MOSI_EXTERNAL1_Pin GPIO_PIN_9
#define SPI5_MOSI_EXTERNAL1_GPIO_Port GPIOF
#define SPI5_MISO_EXTERNAL1_Pin GPIO_PIN_8
#define SPI5_MISO_EXTERNAL1_GPIO_Port GPIOF
#define UI_LED_GREEN_Pin GPIO_PIN_11
#define UI_LED_GREEN_GPIO_Port GPIOH
#define UI_LED_RED_Pin GPIO_PIN_10
#define UI_LED_RED_GPIO_Port GPIOH
#define SCALED_V5_Pin GPIO_PIN_0
#define SCALED_V5_GPIO_Port GPIOC
#define SCALED_VDD_3V3_SENSORS_Pin GPIO_PIN_1
#define SCALED_VDD_3V3_SENSORS_GPIO_Port GPIOC
#define HW_VER_SENSE_Pin GPIO_PIN_2
#define HW_VER_SENSE_GPIO_Port GPIOC
#define HW_REV_SENSE_Pin GPIO_PIN_3
#define HW_REV_SENSE_GPIO_Port GPIOC
#define SD_DETECT_Pin GPIO_PIN_2
#define SD_DETECT_GPIO_Port GPIOB
#define FMU_CH7_Pin GPIO_PIN_6
#define FMU_CH7_GPIO_Port GPIOH
#define I2C3_SDA_EXTERNAL3_Pin GPIO_PIN_8
#define I2C3_SDA_EXTERNAL3_GPIO_Port GPIOH
#define FMU_CH8_Pin GPIO_PIN_9
#define FMU_CH8_GPIO_Port GPIOH
#define FMU_CH6_Pin GPIO_PIN_14
#define FMU_CH6_GPIO_Port GPIOD
#define FMU_CH5_Pin GPIO_PIN_13
#define FMU_CH5_GPIO_Port GPIOD
#define BAT1_I_Pin GPIO_PIN_1
#define BAT1_I_GPIO_Port GPIOA
#define BAT1_V_Pin GPIO_PIN_0
#define BAT1_V_GPIO_Port GPIOA
#define ADC1_SPARE_2_Pin GPIO_PIN_4
#define ADC1_SPARE_2_GPIO_Port GPIOA
#define ADC1_SPARE_1_Pin GPIO_PIN_4
#define ADC1_SPARE_1_GPIO_Port GPIOC
#define I2C3_SCL_EXTERNAL3_Pin GPIO_PIN_7
#define I2C3_SCL_EXTERNAL3_GPIO_Port GPIOH
#define USART3_RTS_TELEM2_Pin GPIO_PIN_12
#define USART3_RTS_TELEM2_GPIO_Port GPIOD
#define USART3_CTS_TELEM2_Pin GPIO_PIN_11
#define USART3_CTS_TELEM2_GPIO_Port GPIOD
#define BAT2_V_Pin GPIO_PIN_2
#define BAT2_V_GPIO_Port GPIOA
#define SPI1_MISO_SENSOR1_Pin GPIO_PIN_6
#define SPI1_MISO_SENSOR1_GPIO_Port GPIOA
#define FMU_CAP1_Pin GPIO_PIN_5
#define FMU_CAP1_GPIO_Port GPIOA
#define I2C4_SDA_EXTERNAL2_Pin GPIO_PIN_15
#define I2C4_SDA_EXTERNAL2_GPIO_Port GPIOF
#define UART7_TX_DEBUG_Pin GPIO_PIN_8
#define UART7_TX_DEBUG_GPIO_Port GPIOE
#define FMU_CH4_Pin GPIO_PIN_9
#define FMU_CH4_GPIO_Port GPIOE
#define FMU_CH3_Pin GPIO_PIN_11
#define FMU_CH3_GPIO_Port GPIOE
#define FMU_CH1_Pin GPIO_PIN_14
#define FMU_CH1_GPIO_Port GPIOE
#define USART3_RX_TELEM2_Pin GPIO_PIN_9
#define USART3_RX_TELEM2_GPIO_Port GPIOD
#define USART3_TX_TELEM2_Pin GPIO_PIN_8
#define USART3_TX_TELEM2_GPIO_Port GPIOD
#define BAT2_I_Pin GPIO_PIN_3
#define BAT2_I_GPIO_Port GPIOA
#define HEATER_Pin GPIO_PIN_7
#define HEATER_GPIO_Port GPIOA
#define nLED_RED_Pin GPIO_PIN_1
#define nLED_RED_GPIO_Port GPIOB
#define RSSI_IN_Pin GPIO_PIN_0
#define RSSI_IN_GPIO_Port GPIOB
#define I2C4_SCL_EXTERNAL2_Pin GPIO_PIN_14
#define I2C4_SCL_EXTERNAL2_GPIO_Port GPIOF
#define FMU_CAP3_Pin GPIO_PIN_11
#define FMU_CAP3_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
