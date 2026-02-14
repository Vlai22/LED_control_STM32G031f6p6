#include "stm32g031xx.h" 
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_tim.h"

uint64_t microsec = 0;
void TIM3_IRQHandler(void) {
    if(LL_TIM_IsActiveFlag_UPDATE(TIM3)){
        LL_TIM_ClearFlag_UPDATE(TIM3);
        microsec++;
    }
}

void delay(volatile uint32_t count) {
    while(count--) {
        __NOP(); // Пустая операция, чтобы компилятор не удалил цикл
    }
}
int main(void) {
    //переменные лампы
    uint32_t brightness_1 = 0;
    uint32_t brightness_2 = 0;
    bool click_1 = false;
    uint32_t time_dt = 0;
    uint32_t time_start = 0;
    uint32_t time_click_dw_1 = 0;
    uint32_t time_click_up_1 = 0;
    //настройка RCC 
    //настройка памяти на высокую частоту
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
    //проверка произошло 
    while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2);
    //включаем систему HSI 
    LL_RCC_HSI_Enable();
    //проверяем HSI
    while(LL_RCC_HSI_IsReady() != 1);
    //настройка PLL
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_1, 8, LL_RCC_PLLR_DIV_2);
    //включение PLL
    LL_RCC_PLL_Enable();
    //включаем домен SYS не знаю что
    LL_RCC_PLL_EnableDomain_SYS();
    //проверяем готов ли pll
    while(LL_RCC_PLL_IsReady() != 1);
    //ставим делитель на AHB 
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    //испрользуемдля системных часов PLL
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    //проверяем переключился ли системный источник
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);
    //устанавливает делитель на шину
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    //настройка шим
    //настройка тактирования всего
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);//включаем тактирование gpioa
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);//включаем тактирование таймера 2
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);//включаем тактирование таймера 2
    //настройка таймеров 
    LL_TIM_SetPrescaler(TIM2, 63);//настраиваем делитель частоты на таймер
    LL_TIM_SetAutoReload(TIM2, 127);//устанвливает значение автоперезагрузки
    LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);//устанавливаем счёт вверх 
    //настройка таймера милисекунд для программной логики
    LL_TIM_SetPrescaler(TIM3, 63);
    LL_TIM_SetAutoReload(TIM3, 1);
    LL_TIM_SetCounterMode(TIM3, LL_TIM_COUNTERMODE_UP);
    LL_TIM_EnableIT_UPDATE(TIM3);
    NVIC_EnableIRQ(TIM3_IRQn);
    __enable_irq();
    //настраиваем пин 1 для показа значений на светодиоде
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_ALTERNATE);//настройка альтернативной функции
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_1, LL_GPIO_AF_2);//настройка альтернативной функции 2 
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_1, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_1, LL_GPIO_OUTPUT_PUSHPULL);
    //настраиваем пин 2 для показа значений на светодиоде
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_ALTERNATE);//настройка альтернативной функции
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_2, LL_GPIO_AF_2);//настройка альтернативной функции 2 
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_2, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_2, LL_GPIO_OUTPUT_PUSHPULL);
    //настройка шим канала 1 
    LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetCompareCH2(TIM2, brightness_1);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH2);
    LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH3, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetCompareCH3(TIM2, brightness_2);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH3);
    LL_TIM_EnableCounter(TIM2); // Устанавливает бит CEN в TIM2_CR1
    while(1) {
        time_start = microsec;
        LL_TIM_OC_SetCompareCH2(TIM2, brightness_1);
        LL_TIM_OC_SetCompareCH3(TIM2, brightness_2);
        if(click_1){
            time_click_dw_1 += time_dt;
        }else if(!click_1 && time_click_dw_1 >= 0 && time_click_up_1 >= 700){
            time_click_dw_1 = 0;
            time_click_up_1 += time_dt;
        }
        time_dt = microsec - time_start;
    }
}