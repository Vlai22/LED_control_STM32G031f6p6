#include "stm32g031xx.h" 
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_rcc.h"

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

    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    // 3. Настраиваем PA4 на выход (Mode: Output)
    // Регистр MODER: для PA4 это биты 8 и 9. 
    GPIOA->MODER &= ~(GPIO_MODER_MODE4_Msk); // Сначала очищаем
    GPIOA->MODER |= (1 << GPIO_MODER_MODE4_Pos); // Устанавливаем 01
    while(1) {
        GPIOA->ODR ^= GPIO_ODR_OD4;

        // 5. Ждем. При 64 МГц значение 1 000 000 даст примерно 0.1-0.2 сек
        delay(6400000);
    }
}