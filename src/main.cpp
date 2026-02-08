#include "stm32g031xx.h" 

void RCC_64Mgh(){
    RCC->CR |= RCC_CR_HSION;//включение HSI16 встроенный RC 16Mhg
    while(!(RCC->CR & RCC_CR_HSIRDY));//убеждаемся в включеии
    RCC->CR &= ~RCC_CR_HSIDIV_Msk;//делитель HSI на HSISYS 1 то есть без деления

    /*RCC->APBENR1 |= RCC_APBENR1_PWREN; // Включаем тактирование PWR
    PWR->CR1 = (PWR->CR1 & ~PWR_CR1_VOS_Msk) | (1 << PWR_CR1_VOS_Pos); // Range 1
    while (PWR->SR2 & PWR_SR2_VOSF); // Ждем готовности питания
    */
    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY_Msk) | (2 << FLASH_ACR_LATENCY_Pos);
    while((FLASH->ACR & FLASH_ACR_LATENCY_Msk) != FLASH_ACR_LATENCY_2); // Ждем применения

    RCC->CR &= ~RCC_CR_PLLON;//выключаем PLL для настройки
    while (RCC->CR & RCC_CR_PLLRDY);//убеждаемся в выключении PLL
    
    //присваеем регистру  сброженые биты по маске и с помощью опенрации или устанавливаем 10 а позицию битов в регистре
    RCC->PLLCFGR =  (2 << RCC_PLLCFGR_PLLSRC_Pos)//устанапвливаем источник тактирования PLL HSI16
                    | (4 << RCC_PLLCFGR_PLLM_Pos) // делитель входной частоты 
                    | (16 << RCC_PLLCFGR_PLLN_Pos)//устанавливаем умножение входной частоты на 8 что соотвестует 128 Mgh
                    | (1 << RCC_PLLCFGR_PLLQ_Pos)//включаем делитель PLLQ на 2 что бы частота не превышала 128Mgh и равна 64 MHg
                    | (1 << RCC_PLLCFGR_PLLR_Pos)//назначаем делитель 2 по сколльку частота PLLR не может превышать 64 Mgh 
                    | (1 << RCC_PLLCFGR_PLLP_Pos);
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLPEN;//выключаем тактирования PLLPCLK по скольку нет необходимости в нем
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLQEN;//включаем тактирования PLLQCLK по скольку от него тактируются часть таймеров    

    RCC->CR |= RCC_CR_PLLON;//включаем PLL для запуска SYSCLK
    while (!(RCC->CR & RCC_CR_PLLRDY));//убеждаемся в включении PLL

    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW_Msk) | (3 << RCC_CFGR_SW_Pos);//устанавливаем источник SW PLLRCLK
    while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != (3 << RCC_CFGR_SWS_Pos)); //дожидаемся преерключания системного тактирования  
    RCC->CFGR &= ~RCC_CFGR_HPRE_Msk;//устанавливаем на линию AHB делитель 1 в случае уменьшения напряжения питания нужно уменьшить частоту до 16Mgh
    RCC->CFGR &= ~RCC_CFGR_PPRE_Msk;//устанавливаем на линию APB делитель 1 для максимальной чапстоты
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOSEL_Msk) | (2 << RCC_CFGR_MCOSEL_Pos);//устанавливем тактирование MCO через SYSCLK
    RCC->CFGR &= ~RCC_CFGR_MCOPRE_Msk;//устанавливаем делитель на 1 для достижения максимальной частоты MCO

}
void delay(volatile uint32_t count) {
    while(count--) {
        __NOP(); // Пустая операция, чтобы компилятор не удалил цикл
    }
}
int main(void) {
    RCC_64Mgh();
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    // 3. Настраиваем PA4 на выход (Mode: Output)
    // Регистр MODER: для PA4 это биты 8 и 9. 
    GPIOA->MODER &= ~(GPIO_MODER_MODE4_Msk); // Сначала очищаем
    GPIOA->MODER |= (1 << GPIO_MODER_MODE4_Pos); // Устанавливаем 01
    while(1) {
        GPIOA->ODR ^= GPIO_ODR_OD4;

        // 5. Ждем. При 64 МГц значение 1 000 000 даст примерно 0.1-0.2 сек
        delay(400000);
    }
}