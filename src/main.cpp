#include "stm32g031xx.h" 
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_gpio.h"

void delay(volatile uint32_t count) {
    while(count--) {
        __NOP(); // Пустая операция, чтобы компилятор не удалил цикл
    }
}
int main(void) {
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
    while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2);
    LL_RCC_HSI_Enable();
    while(LL_RCC_HSI_IsReady() != 1);
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_1, 8, LL_RCC_PLLR_DIV_2);
    LL_RCC_PLL_Enable();
    LL_RCC_PLL_EnableDomain_SYS();
    while(LL_RCC_PLL_IsReady() != 1);
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);//включаем тактирование gpioa
    // Установка режима (Input, Output, Alternate Function, Analog)
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_OUTPUT);
    // Установка типа выхода (Push-Pull или Open-Drain)
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_4, LL_GPIO_OUTPUT_PUSHPULL);
    // Установка подтяжки (No Pull, Pull-up, Pull-down)
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_4, LL_GPIO_PULL_UP);
    // Установка скорости (Low, Medium, High, Very High)
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_4, LL_GPIO_SPEED_FREQ_HIGH);
    while(1) {
        LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_4);
        // 5. Ждем. При 64 МГц значение 1 000 000 даст примерно 0.1-0.2 сек
        delay(64000000);
    }
}