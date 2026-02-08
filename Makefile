# 1. Настройки инструментов
COMPILER = arm-none-eabi-g++
CC       = arm-none-eabi-gcc
AS = arm-none-eabi-gcc
OBJCOPY  = arm-none-eabi-objcopy

# 2. Настройки архитектуры для STM32G031 (ядро Cortex-M0+)
# ВАЖНО: у G031 ядро m0plus, а не m4!
FLAGS  = -mcpu=cortex-m0plus -mthumb -O2  -DSTM32G031xx  

# 3. Пути к заголовочным файлам (INCLUDE)
# Здесь мы говорим компилятору, где искать .h файлы CMSIS
INCLUDES = -I./include -ID:\Projects\Embedded\CMSIS\CMSIS_6\CMSIS\Core\Include -ID:\Projects\Embedded\SCR_LIBS\cmsis-device-g0-master\Include

C++FLAGS = $(FLAGS) $(INCLUDES) -std=c++20 -fno-exceptions
CFLAGS   = $(FLAGS) $(INCLUDES)
LDFLAGS  = -T STM32G031F8PX_FLASH.ld --specs=nosys.specs

# 4. Поиск исходных файлов
# Ищем все .cpp в src и файл стартапа .s (ассемблер)
SRCS_CPP = $(wildcard src/*.cpp)
SRCS_C   = $(wildcard src/*.c)
SRCS_S   = $(wildcard src/*.s)

# Превращаем все типы исходников в .o
OBJS = $(SRCS_CPP:.cpp=.o) $(SRCS_C:.c=.o) $(SRCS_S:.s=.o)

all: led_control.bin

# Шаг 3: Конвертация в BIN
led_control.bin: led_control.elf
	$(OBJCOPY) -O binary $< $@

# Шаг 2: Линковка
led_control.elf: $(OBJS)
	$(COMPILER) $(OBJS) -mcpu=cortex-m0plus -mthumb $(LDFLAGS) -o $@

# Шаг 1: Правила компиляции для разных типов файлов
%.o: %.cpp
	$(COMPILER) $(C++FLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(shell find . -name "*.o") *.elf *.bin