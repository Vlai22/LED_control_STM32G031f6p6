#include "stm32g031xx.h" 
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_tim.h"

uint16_t brig_lamp_1 = 0;
bool onOff_lamp_1  = false;
uint32_t time_dt_on_lamp_1 = 0;
uint32_t time_dt_off_lamp_1 = 0;
uint32_t time_dt_brig_lamp_1 = 0;
uint16_t brig_lamp_2 = 0;
bool onOff_lamp_2  = false;
uint32_t time_dt_on_lamp_2 = 0;
uint32_t time_dt_off_lamp_2 = 0;
uint32_t time_dt_brig_lamp_2 = 0;
uint16_t brig_lamp_R = 0;
uint16_t brig_lamp_G = 0;
uint16_t brig_lamp_B = 0;
bool onOff_lamp_1  = false;
uint32_t time_dt_on_lamp_1 = 0;
uint32_t time_dt_off_lamp_1 = 0;
uint32_t time_dt_brig_lamp_R = 0;
uint32_t time_dt_brig_lamp_G = 0;
uint32_t time_dt_brig_lamp_B = 0;
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

    //настройка шим
    //настройка тактирования всего
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);//включаем тактирование gpioa
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM14);//включаем тактирование таймера 2
    //настройка таймеров 
    LL_TIM_SetPrescaler(TIM14, 639);//настраиваем делитель частоты на таймер
    LL_TIM_SetAutoReload(TIM14, 999);//устанвливает значение автоперезагрузки
    LL_TIM_SetCounterMode(TIM14, LL_TIM_COUNTERMODE_UP);//устанавливаем счёт вверх 
    //настраиваем пин 4 для показа значений на светодиоде
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ALTERNATE);//настройка альтернативной функции
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_4, LL_GPIO_AF_4);//настройка альтернативной функции 2 
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_4, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_4, LL_GPIO_OUTPUT_PUSHPULL);
    //настройка шим канала 1 
    LL_TIM_OC_SetMode(TIM14, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetCompareCH1(TIM14, 500);
    LL_TIM_CC_EnableChannel(TIM14, LL_TIM_CHANNEL_CH1);
    LL_TIM_EnableCounter(TIM14); // Устанавливает бит CEN в TIM2_CR1
    while(1) {
    }
}