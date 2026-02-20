#include "stm32g031xx.h" 
#include <stdbool.h>
#include <math.h>
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_tim.h"

volatile uint32_t microsec = 0;
void TIM3_IRQHandler(void){
    if(LL_TIM_IsActiveFlag_UPDATE(TIM3)){
        LL_TIM_ClearFlag_UPDATE(TIM3);
        microsec += 1;
    }
}
int main(void) {
    //переменные лампы
    uint32_t time_dt = 0;
    uint32_t time_start = 0;

    uint8_t brig_lamp_1 = 0;
    uint8_t brig_now_lamp_1 = 126;
    //биты какой что обозначает: включение | запуск события старт | пауза нажатия | нажатие на кнопку лампы | нажатие на кнопку изменения яркости
    uint8_t Lamp1Status = 0x00000000;
    uint32_t time_dt_on_lamp_1 = 0;
    uint32_t time_dt_start_lamp_1 = 0;
    uint32_t time_dt_brig_lamp_1 = 0;
    int8_t StepLamp1 = 1;

    uint8_t brig_lamp_2 = 0;
    uint8_t brig_now_lamp_2 = 126;
    //биты какой что обозначает: включение | запуск события старт | пауза нажатия | нажатие на кнопку лампы | нажатие на кнопку изменения яркости
    uint8_t Lamp2Status = 0x00000000;
    uint32_t time_dt_on_lamp_2 = 0;
    uint32_t time_dt_start_lamp_2 = 0;
    uint32_t time_dt_brig_lamp_2 = 0;
    int8_t StepLamp2 = 1;

    uint8_t brig_lamp_R = 0;
    uint8_t brig_lamp_G = 0;
    uint8_t brig_lamp_B = 0;
    uint8_t brig_now_lamp_R = 0;
    uint8_t brig_now_lamp_G = 0;
    uint8_t brig_now_lamp_B = 0;
    //биты какой что обозначает: включение | запуск события старт | пауза нажатия | нажатие на кнопку лампы | нажатие на кнопку изменения яркости
    uint8_t LampRGBStatus = 0x00000000;
    uint32_t time_dt_on_lamp_rgb = 0;
    uint32_t time_dt_start_lamp_rgb = 0;
    uint32_t time_dt_brig_lamp_R = 0;
    uint32_t time_dt_brig_lamp_G = 0;
    uint32_t time_dt_brig_lamp_B = 0;
    int8_t StepLampR = 1;
    int8_t StepLampG = 1;
    int8_t StepLampB = 1;
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
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);//включение тактирования порта C
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);//включаем тактирование таймера 2
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);//включаем тактирование таймера 3
    LL_APB1_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM14);//включаем тактирование таймера 14
    //настройка таймеров 
    LL_TIM_SetPrescaler(TIM2, 63);//настраиваем делитель частоты на таймер
    LL_TIM_SetAutoReload(TIM2, 127);//устанвливает значение автоперезагрузки
    LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);//устанавливаем счёт вверх 
    LL_TIM_SetPrescaler(TIM14, 63);//настраиваем делитель частоты на таймер
    LL_TIM_SetAutoReload(TIM14, 127);//устанвливает значение автоперезагрузки
    LL_TIM_SetCounterMode(TIM14, LL_TIM_COUNTERMODE_UP);//устанавливаем счёт вверх 
    LL_TIM_EnableAllOutputs(TIM14);//включение вывода с таймера 14
    //настройка таймера милисекунд для программной логики
    LL_TIM_SetPrescaler(TIM3, 63);
    LL_TIM_SetAutoReload(TIM3, 999);
    LL_TIM_SetCounterMode(TIM3, LL_TIM_COUNTERMODE_UP);
    LL_TIM_EnableIT_UPDATE(TIM3);
    NVIC_SetPriority(TIM3_IRQn, 1);
    NVIC_EnableIRQ(TIM3_IRQn);
    __enable_irq();
       // настройка пинов вывода
    //настраиваем пин 0 для показа значений на светодиоде
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ALTERNATE);//настройка альтернативной функции
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_0, LL_GPIO_AF_2);//настройка альтернативной функции 2 
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_0, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_0, LL_GPIO_OUTPUT_PUSHPULL);
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
    //настраиваем пин 3 для показа значений на светодиоде
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_3, LL_GPIO_MODE_ALTERNATE);//настройка альтернативной функции
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_3, LL_GPIO_AF_2);//настройка альтернативной функции 2 
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_3, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_3, LL_GPIO_OUTPUT_PUSHPULL);
    //настраиваем пин 4 для показа значений на светодиоде
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ALTERNATE);//настройка альтернативной функции
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_4, LL_GPIO_AF_4);//настройка альтернативной функции 4
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_4, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_4, LL_GPIO_OUTPUT_PUSHPULL);
    //настройка дефолтного состаяния шим всех выводом
    // канал 1 таймера 2 пина A0 
    LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetCompareCH1(TIM2, brig_lamp_1); 
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1);
    // канал 2 таймера 2 пина A1 
    LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetCompareCH2(TIM2, brig_lamp_2);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH2);
    // канал 3 таймера 2 пина A2
    LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH3, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetCompareCH3(TIM2, brig_lamp_R);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH3);
    // канал 4 таймера 2 пина A3
    LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH4, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetCompareCH4(TIM2, brig_lamp_G);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH4);
    // канал 1 таймера 14 пина A4 
    LL_TIM_OC_SetMode(TIM14, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetCompareCH1(TIM14, brig_lamp_B);
    LL_TIM_CC_EnableChannel(TIM14, LL_TIM_CHANNEL_CH1);
    //выключение всех счётчиков таймеров
    LL_TIM_EnableCounter(TIM2);
    LL_TIM_EnableCounter(TIM14);
    LL_TIM_EnableCounter(TIM3);
    //настройка пинов ввода
    // настройка пина gpioa 5 
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_5, LL_GPIO_PULL_DOWN);
    // настройка пина gpioa 6
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_6, LL_GPIO_PULL_DOWN);
    // настройка пина gpioa 7 
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_7, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_7, LL_GPIO_PULL_DOWN);
    // настройка пина gpioa 8 
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_8, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_8, LL_GPIO_PULL_DOWN);
    // настройка пина gpioa 11
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_11, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_11, LL_GPIO_PULL_DOWN);
    // настройка пина gpioa 12 
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_12, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_12, LL_GPIO_PULL_DOWN);
    // настройка пина gpioс 14
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_14, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_14, LL_GPIO_PULL_DOWN);
    // настройка пина gpioa 15
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_15, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_15, LL_GPIO_PULL_DOWN);
    while(1) {
        //включение первой лампы 
        if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_5)){
            if(time_dt_on_lamp_1 == 0){
                time_dt_on_lamp_1 = microsec;
            }
            if(microsec - time_dt_on_lamp_1 > 150 && !(Lamp1Status & (1 << 3))){
                Lamp1Status |= (1 << 3);
                if(Lamp1Status & (1 << 1)){
                    brig_lamp_1 = 0;
                    Lamp1Status &= ~(1 << 1);
                    LL_TIM_OC_SetCompareCH1(TIM2, brig_lamp_1);
                }else{
                    Lamp1Status |= (1 << 2);
                }
            }
        }else{
            Lamp1Status &= ~(1 << 3);
            time_dt_on_lamp_1 = 0;
        }
        if(Lamp1Status & (1 << 2)){
            if(time_dt_start_lamp_1 == 0){
                time_dt_start_lamp_1 = microsec;
            }
            if(microsec - time_dt_start_lamp_1 > 10 && brig_lamp_1 < brig_now_lamp_1){
                brig_lamp_1 += 1;
                LL_TIM_OC_SetCompareCH1(TIM2, brig_lamp_1);
                time_dt_start_lamp_1 = 0;
            }
            if(brig_lamp_1 >= brig_now_lamp_1){
                Lamp1Status &= ~(1 << 2);
                Lamp1Status |= (1 << 1);
            }
        }
        //изменение яркости лампы 1
        if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_6) && (Lamp1Status & (1 << 1))){
            if(time_dt_brig_lamp_1 == 0){
                time_dt_brig_lamp_1 = microsec;
            }
            if(microsec - time_dt_brig_lamp_1 > 20){
                if(brig_lamp_1 >= 127){
                    StepLamp1 = -1;
                }else if(brig_lamp_1 <= 0){
                    StepLamp1 = 1;
                }
                brig_lamp_1 += StepLamp1;
                LL_TIM_OC_SetCompareCH1(TIM2, brig_lamp_1);
                time_dt_brig_lamp_1 = 0;
            } 
        }else{
            time_dt_brig_lamp_1 = 0;
        }

        //включение второй лампы 
        if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_7)){
            if(time_dt_on_lamp_2 == 0){
                time_dt_on_lamp_2 = microsec;
            }
            if(microsec - time_dt_on_lamp_2 > 150 && !(Lamp2Status & (1 << 3))){
                Lamp2Status |= (1 << 3);
                if(Lamp2Status & (1 << 1)){
                    brig_lamp_2 = 0;
                    Lamp2Status &= ~(1 << 1);
                    LL_TIM_OC_SetCompareCH2(TIM2, brig_lamp_2);
                }else{
                    Lamp2Status |= (1 << 2);
                }
            }
        }else{
            Lamp2Status &= ~(1 << 3);
            time_dt_on_lamp_2 = 0;
        }
        if(Lamp2Status & (1 << 2)){
            if(time_dt_start_lamp_2 == 0){
                time_dt_start_lamp_2 = microsec;
            }
            if(microsec - time_dt_start_lamp_2 > 10 && brig_lamp_2 < brig_now_lamp_2){
                brig_lamp_2 += 1;
                LL_TIM_OC_SetCompareCH2(TIM2, brig_lamp_2);
                time_dt_start_lamp_2 = 0;
            }
            if(brig_lamp_2 >= brig_now_lamp_2){
                Lamp2Status &= ~(1 << 2);
                Lamp2Status |= (1 << 1);
            }
        }
        //изменение яркости лампы 2
        if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_8) && (Lamp2Status & (1 << 1))){
            if(time_dt_brig_lamp_2 == 0){
                time_dt_brig_lamp_2 = microsec;
            }
            if(microsec - time_dt_brig_lamp_2 > 20){
                if(brig_lamp_2 >= 127){
                    StepLamp2 = -1;
                }else if(brig_lamp_2 <= 0){
                    StepLamp2 = 1;
                }
                brig_lamp_2 += StepLamp2;
                LL_TIM_OC_SetCompareCH2(TIM2, brig_lamp_2);
                time_dt_brig_lamp_2 = 0;
            } 
        }else{
            time_dt_brig_lamp_2 = 0;
        }


        //включение RGB лампы 
        if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_11)){
            if(time_dt_on_lamp_rgb == 0){
                time_dt_on_lamp_rgb = microsec;
            }
            if(microsec - time_dt_on_lamp_rgb > 150 && !(LampRGBStatus & (1 << 3))){
                LampRGBStatus |= (1 << 3);
                if(LampRGBStatus & (1 << 1)){
                    brig_lamp_R = 0;
                    brig_lamp_G = 0;
                    brig_lamp_B = 0;
                    LampRGBStatus &= ~(1 << 1);
                    LL_TIM_OC_SetCompareCH3(TIM2, brig_lamp_R);
                    LL_TIM_OC_SetCompareCH4(TIM2, brig_lamp_G);
                    LL_TIM_OC_SetCompareCH1(TIM14, brig_lamp_B);
                }else{
                    LampRGBStatus |= (1 << 2);
                }
            }
        }else{
            LampRGBStatus &= ~(1 << 3);
            time_dt_on_lamp_rgb = 0;
        }
        if(LampRGBStatus & (1 << 2)){
            if(time_dt_start_lamp_rgb == 0){
                time_dt_start_lamp_rgb = microsec;
            }
            if(microsec - time_dt_start_lamp_rgb > 10 && brig_lamp_R < brig_now_lamp_R && brig_lamp_G < brig_now_lamp_G && brig_lamp_B < brig_now_lamp_B){
                brig_lamp_R += 1;
                brig_lamp_B += 1;
                brig_lamp_G += 1;
                LL_TIM_OC_SetCompareCH3(TIM2, brig_lamp_R);
                LL_TIM_OC_SetCompareCH4(TIM2, brig_lamp_G);
                LL_TIM_OC_SetCompareCH1(TIM14, brig_lamp_B);
                time_dt_start_lamp_rgb = 0;
            }
            if(brig_lamp_R >= brig_now_lamp_R && brig_lamp_B >= brig_now_lamp_B && brig_lamp_G >= brig_now_lamp_G){
                LampRGBStatus &= ~(1 << 2);
                LampRGBStatus |= (1 << 1);
            }
        }
        //изменение яркости лампы R
        if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_11) && (LampRGBStatus & (1 << 1))){
            if(time_dt_brig_lamp_R == 0){
                time_dt_brig_lamp_R = microsec;
            }
            if(microsec - time_dt_brig_lamp_R > 20){
                if(brig_lamp_R >= 127){
                    StepLampR = -1;
                }else if(brig_lamp_R <= 0){
                    StepLampR = 1;
                }
                brig_lamp_R += StepLampR;
                LL_TIM_OC_SetCompareCH3(TIM2, brig_lamp_R);
                time_dt_brig_lamp_R = 0;
            } 
        }else{
            time_dt_brig_lamp_R = 0;
        }
        //изменение яркости лампы G
        if(LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_15) && (LampRGBStatus & (1 << 1))){
            if(time_dt_brig_lamp_G == 0){
                time_dt_brig_lamp_G = microsec;
            }
            if(microsec - time_dt_brig_lamp_G > 20){
                if(brig_lamp_G >= 127){
                    StepLampG = -1;
                }else if(brig_lamp_G <= 0){
                    StepLampG = 1;
                }
                brig_lamp_G += StepLampG;
                LL_TIM_OC_SetCompareCH4(TIM2, brig_lamp_G);
                time_dt_brig_lamp_G = 0;
            } 
        }else{
            time_dt_brig_lamp_G = 0;
        }
        //изменение яркости лампы B
        if(LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_14) && (LampRGBStatus & (1 << 1))){
            if(time_dt_brig_lamp_B == 0){
                time_dt_brig_lamp_B = microsec;
            }
            if(microsec - time_dt_brig_lamp_B > 20){
                if(brig_lamp_B >= 127){
                    StepLampB = -1;
                }else if(brig_lamp_B <= 0){
                    StepLampB = 1;
                }
                brig_lamp_B += StepLampB;
                LL_TIM_OC_SetCompareCH1(TIM14, brig_lamp_B);
                time_dt_brig_lamp_B = 0;
            } 
        }else{
            time_dt_brig_lamp_B = 0;
        }
    }
} 