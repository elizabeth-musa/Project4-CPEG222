/********************************************************************************************
 * CPEG 222 Project 4, 10/9/2026
 * Author: E. MUSA
 * NucleoF466ZE CMSIS STM32F4xx Sine Wave Generator
 ********************************************************************************************/
#include "stm32f4xx.h" // Include the STM32F4xx CMSIS header file
#include "ssd.h"
#include <stdint.h>

// Digital Inputs
#define SW1_PORT GPIOC
#define SW1_PIN 8
#define UP_PORT GPIOF

//Digital Outputs
#define SEG_AD GPIOG
#define SEG_BCF GPIOF
#define DP GPIOF
#define SEG_G GPIOB
#define SEG_E GPIOE

#define CA12 GPIOE
#define CA34 GPIOB
#define AUDIO_SD_PORT GPIOB

//Analog Output
#define DAC_1 GPIOA

// Analog Input
#define V_POT GPIOC

#define ANALOG_PIN 2
#define ANALOG_PORT GPIOC
#define ADC_CHANNEL 12 // ADC Channel for PC2
#define ADC_SAMPLES 16 // Number of samples for averaging

#
#define SINE_SAMPLES 100 
#define FREQUENCY 16000000UL // 16 MHz
#define Audio_SD_PIN 6 // Audio amplifier SD shutdown pin

 const uint16_t sine_lut[SINE_SAMPLES] = {
    2048, 2110, 2173, 2235, 2296, 2357, 2416, 2474, 2530, 2584,
    2636, 2685, 2732, 2777, 2818, 2857, 2892, 2924, 2953, 2978,
    2999, 3016, 3030, 3040, 3046, 3048, 3046, 3040, 3030, 3016,
    2999, 2978, 2953, 2924, 2892, 2857, 2818, 2777, 2732, 2685,
    2636, 2584, 2530, 2474, 2416, 2357, 2296, 2235, 2173, 2110,
    2048, 1985, 1922, 1860, 1799, 1738, 1679, 1621, 1565, 1511,
    1459, 1410, 1363, 1318, 1277, 1238, 1203, 1171, 1142, 1117,
    1096, 1079, 1065, 1055, 1049, 1048, 1049, 1055, 1065, 1079,
    1096, 1117, 1142, 1171, 1203, 1238, 1277, 1318, 1363, 1410,
    1459, 1511, 1565, 1621, 1679, 1738, 1799, 1860, 1922, 1985
};


#define NOTE_COUNT 25

const uint16_t birthday_notes[NOTE_COUNT] = {
    392, 392, 440, 392, 523, 494,
    392, 392, 440, 392, 587, 523,
    392, 392, 784, 659, 523, 494, 440,
    698, 698, 659, 523, 587, 523
};

const uint16_t birthday_durations[NOTE_COUNT] = {
    300, 150, 450, 450, 450, 900,
    300, 150, 450, 450, 450, 900,
    300, 150, 450, 450, 450, 450, 900,
    300, 150, 450, 450, 450, 900
};

void Delay_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms * 1000U; i++) {
    }
}

void Play_Happy_Birthday(void)
{
    for (int i = 0; i < NOTE_COUNT; i++) {
        uint32_t arr =
            (FREQUENCY / (SINE_SAMPLES * (uint32_t)birthday_notes[i])) - 1U;

        TIM6->ARR = arr;
        TIM6->EGR = TIM_EGR_UG;

        Delay_ms(birthday_durations[i]);
        Delay_ms(50);
    }
}

int main(void)
{

   
    /*Enable GPIO Clocks*/
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;

    /* Set SW1 and UP as digital inputs */

    GPIOC->MODER &= ~(0x3 << (SW1_PIN * 2)); // S1: PC8
     UP_PORT->MODER &= ~(0x3U << (7 * 2));
    UP_PORT->PUPDR &= ~(0x3U << (7 * 2));  // No internal pull

    /* Set SSD segment pins as digital outputs */

    SEG_AD->MODER &= ~(0x3 << (9 * 2));   // Segment A: PG9
    SEG_AD->MODER |=  (0x1 << (9 * 2));

    SEG_AD->MODER &= ~(0x3 << (14 * 2));  // Segment D: PG14
    SEG_AD->MODER |=  (0x1 << (14 * 2));

    SEG_BCF->MODER &= ~(0x3 << (12 * 2)); // Segment B: PF12
    SEG_BCF->MODER |=  (0x1 << (12 * 2));

    SEG_BCF->MODER &= ~(0x3 << (13 * 2)); // Segment C: PF13
    SEG_BCF->MODER |=  (0x1 << (13 * 2));

    SEG_BCF->MODER &= ~(0x3 << (15 * 2)); // Segment F: PF15
    SEG_BCF->MODER |=  (0x1 << (15 * 2));

    DP->MODER &= ~(0x3 << (14 * 2));      // Decimal point: PF14
    DP->MODER |=  (0x1 << (14 * 2));

    SEG_G->MODER &= ~(0x3 << (4 * 2));    // Segment G: PB4
    SEG_G->MODER |=  (0x1 << (4 * 2));

    SEG_E->MODER &= ~(0x3 << (8 * 2));    // Segment E: PE8
    SEG_E->MODER |=  (0x1 << (8 * 2));

    /* Set SSD digit-select pins as digital outputs */

    CA12->MODER &= ~(0x3 << (10 * 2));    // CA1: PE10
    CA12->MODER |=  (0x1 << (10 * 2));

    CA12->MODER &= ~(0x3 << (7 * 2));     // CA2: PE7
    CA12->MODER |=  (0x1 << (7 * 2));

    CA34->MODER &= ~(0x3 << (5 * 2));     // CA3: PB5
    CA34->MODER |=  (0x1 << (5 * 2));

    CA34->MODER &= ~(0x3 << (3 * 2));     // CA4: PB3
    CA34->MODER |=  (0x1 << (3 * 2));

    /* Set amplifier shutdown pin as digital output */

    AUDIO_SD_PORT->MODER &= ~(0x3 << (6 * 2)); // Audio_SD: PB6
    AUDIO_SD_PORT->MODER |=  (0x1 << (6 * 2));
    AUDIO_SD_PORT->BSRR = (1U << Audio_SD_PIN);

    /* Set DAC output pin to analog mode */

    DAC_1->MODER &= ~(0x3 << (4 * 2));    // DAC1: PA4
    DAC_1->MODER |=  (0x3 << (4 * 2));

    /* Set potentiometer pin to analog mode */

    V_POT->MODER &= ~(0x3 << (2 * 2));    // V_POT: PC2
    V_POT->MODER |=  (0x3 << (2 * 2));

    /*Initialize SSD array and TIM5 multiplexing*/
    SSD_Init();

     // Configure ADC1 for the potentiometer
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    ADC1->SQR1 = 0;                 // One conversion
    ADC1->SQR3 = ADC_CHANNEL;       // Channel 12: PC2

    // Set ADC sample time for channel 12
    ADC1->SMPR1 &= ~(7U << (3 * (ADC_CHANNEL - 10)));
    ADC1->SMPR1 |=  (7U << (3 * (ADC_CHANNEL - 10)));

    ADC1->CR2 = ADC_CR2_ADON;       // Enable ADC1

    // Configure DMA1 Stream 5 Channel 7
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    DMA1_Stream5->CR &= ~DMA_SxCR_EN;
    while (DMA1_Stream5->CR & DMA_SxCR_EN);

    DMA1->HIFCR = (1U << 6) | (0xFU << 8);

    DMA1_Stream5->CR = (7U << DMA_SxCR_CHSEL_Pos);
    DMA1_Stream5->CR |= (1U << DMA_SxCR_MSIZE_Pos)
                      | (1U << DMA_SxCR_PSIZE_Pos);
    DMA1_Stream5->CR |= DMA_SxCR_MINC;
    DMA1_Stream5->CR |= (1U << DMA_SxCR_DIR_Pos);
    DMA1_Stream5->CR |= DMA_SxCR_CIRC;

    DMA1_Stream5->NDTR = SINE_SAMPLES;
    DMA1_Stream5->PAR = (uint32_t)&DAC->DHR12R1;
    DMA1_Stream5->M0AR = (uint32_t)sine_lut;

    // Configure DAC Channel 1
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    DAC->CR &= ~DAC_CR_TSEL1;
    DAC->DHR12R1 = sine_lut[0];
    DAC->CR |= DAC_CR_TEN1;
    DAC->CR |= DAC_CR_DMAEN1;
    DAC->CR |= DAC_CR_EN1;

    // Configure TIM6 as the DAC Trigger
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = 0;
    TIM6->ARR = (FREQUENCY / (SINE_SAMPLES * 440U)) - 1U;

    TIM6->CR2 &= ~TIM_CR2_MMS;
    TIM6->CR2 |= (2U << TIM_CR2_MMS_Pos);

    TIM6->EGR = TIM_EGR_UG;
    TIM6->SR = 0;

    // Enable DMA and Start TIM6
    DMA1_Stream5->CR |= DMA_SxCR_EN;
    TIM6->CR1 |= TIM_CR1_CEN;

    uint32_t up_idle = (UP_PORT->IDR >> 7) & 1U;  // level when button is not pressed
    while (1)
    {
     if (((UP_PORT->IDR >> 7) & 1U) != up_idle) {
            Play_Happy_Birthday();

            // Wait for release (bounded so it can never lock up)
            uint32_t t = 2000000;
            while ((((UP_PORT->IDR >> 7) & 1U) != up_idle) && --t) {
            }
        }
        uint32_t total = 0;

    for (int i = 0; i < ADC_SAMPLES; i++)
    {
        ADC1->CR2 |= ADC_CR2_SWSTART;

        while (!(ADC1->SR & ADC_SR_EOC))
        {
        }

        total += ADC1->DR;
    }

    uint16_t average = total / ADC_SAMPLES;

    // Convert ADC reading to frequency from 100 Hz to 2000 Hz
    uint16_t frequency =
        100 + ((uint32_t)average * 1900) / 4095;

     // Update sine-wave frequency
        uint32_t arr = (FREQUENCY / (SINE_SAMPLES * (uint32_t)frequency)) - 1U;
        TIM6->ARR = arr;

        SSD_DisplayValue(frequency);

        // Control Audio Amplifier with S1
        if (SW1_PORT->IDR & (1U << SW1_PIN)) {
            AUDIO_SD_PORT->BSRR = (1U << Audio_SD_PIN);
        }
        else {
            AUDIO_SD_PORT->BSRR = (1U << (Audio_SD_PIN + 16));
        }
    }

    return 0;
}