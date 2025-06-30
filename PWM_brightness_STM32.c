

#include <stdint.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/exti.h>

//Port and pin configurations
#define LED_PORT        GPIOB
#define LED_PIN         GPIO14            

#define BTN_PORT        GPIOB
#define BTN_PIN_MASK    GPIO13            
#define BTN_EXTI_LINE   EXTI13            

//These are the PWM timer configurations 
//A prescale 71 corresponds to a 1MHz timer clock
//A period of 999 has 1000 counts which is a 1kHz PWM
#define TIM_PRESCALER   71                
#define TIM_PERIOD      999               

//Levels of brightness, the PWM cycle steps
static const uint16_t duty_table[] = {0, 200, 400, 600, 800, 999};
#define NUM_LEVELS (sizeof duty_table / sizeof duty_table[0])
static volatile uint8_t level = 0;


static inline void pwm_set_level(uint8_t idx)
{
    if (idx >= NUM_LEVELS) idx = 0;
    timer_set_oc_value(TIM1, TIM_OC2, duty_table[idx]);   
}

//Just a set up  function, sets the system clock to 72MHz
//Also enables GPIO B, AFIO(needed for EXTI), and TIM1
static void clock_setup(void)
{

    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);

    
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_AFIO);
    rcc_periph_clock_enable(RCC_TIM1);
}


static void gpio_exti_setup(void)
{
//Sets LED to push pull config so timer will control it    
    gpio_set_mode(LED_PORT, GPIO_MODE_OUTPUT_2_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, LED_PIN);

//Button configuration, set as an input with internal pullup.  
    gpio_set(BTN_PORT, BTN_PIN_MASK);   
    gpio_set_mode(BTN_PORT, GPIO_MODE_INPUT,
                  GPIO_CNF_INPUT_PULL_UPDOWN, BTN_PIN_MASK);

//Triggers a EXTI interrupt on the falling edge(a button press)
    exti_select_source(BTN_EXTI_LINE, BTN_PORT);
    exti_set_trigger(BTN_EXTI_LINE, EXTI_TRIGGER_FALLING);  
    exti_enable_request(BTN_EXTI_LINE);
    nvic_enable_irq(NVIC_EXTI15_10_IRQ);
}

//Function to set up tim1 to generate PWM
static void tim1_pwm_setup(void)
{

    timer_disable_counter(TIM1);
    timer_generate_event(TIM1, TIM_EGR_UG);   

    timer_set_prescaler(TIM1, TIM_PRESCALER);
    timer_set_period   (TIM1, TIM_PERIOD);

    timer_set_oc_mode      (TIM1, TIM_OC2, TIM_OCM_PWM1);
    timer_enable_oc_preload(TIM1, TIM_OC2);
    pwm_set_level(level);                      

    timer_enable_oc_output (TIM1, TIM_OC2N);   
    timer_enable_break_main_output(TIM1);      

    timer_enable_counter(TIM1);
}

//Interrupt flag, checks if interrupt is triggered, resets, then changes brightness level.
void exti15_10_isr(void)
{
    if (exti_get_flag_status(BTN_EXTI_LINE)) {
        exti_reset_request(BTN_EXTI_LINE);
        level = (level + 1) % NUM_LEVELS;
        pwm_set_level(level);
    }
}

// Call all setups and wait for interrupt trigger
int main(void)
{
    clock_setup();
    gpio_exti_setup();
    tim1_pwm_setup();

    while (1) {
        __asm__("wfi");   
    }
}
