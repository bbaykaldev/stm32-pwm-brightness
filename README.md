# STM32 PWM Brightness Controller

A simple STM32F103C8T6 (Blue Pill or big board) project using libopencm3 and PlatformIO.  
Generates PWM on pin **PB14** using TIM1_CH2N to control LED brightness. A push-button on **PB13** steps through 6 brightness levels.

## 🛠️ Requirements

- STM32F103C8T6 dev board (Blue Pill or similar)
- PlatformIO
- ST-Link or USB-UART programmer
- Oscilloscope (optional)

## 🧪 Features

- PWM at 1kHz
- Duty cycle steps: 0%, 20%, 40%, 60%, 80%, 100%
- Button interrupt using EXTI13

## ⚙️ Pinout

| Function | Pin   |
|----------|-------|
| LED PWM  | PB14  |
| Button   | PB13  |
