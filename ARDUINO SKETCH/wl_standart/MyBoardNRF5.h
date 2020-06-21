#ifndef _MYBOARDNRF5_H_
#define _MYBOARDNRF5_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


#define PINS_COUNT           (32u)
#define NUM_DIGITAL_PINS     (32u)
#define NUM_ANALOG_INPUTS    (8u)
#define NUM_ANALOG_OUTPUTS   (8u)

#define PIN_LED1                (27)
#define PIN_LED2                (25)
#define PIN_LED3                (26)
#define RED_LED                 (PIN_LED1)
#define GREEN_LED            	  (PIN_LED2)
#define BLUE_LED			          (PIN_LED3)
#define LED_BUILTIN             (PIN_LED1)

#define PIN_BUTTON             (14)
#define W_L_SENS               (8)

#define POWER_PIN               (7)



static const uint8_t A0  = ADC_A0;
static const uint8_t A1  = ADC_A1;
static const uint8_t A2  = ADC_A2;
static const uint8_t A3  = ADC_A3;
static const uint8_t A4  = ADC_A4;
static const uint8_t A5  = ADC_A5;
static const uint8_t A6  = ADC_A6;
static const uint8_t A7  = ADC_A7;


#define PIN_SERIAL_RX       (12)
#define PIN_SERIAL_TX       (11)


#ifdef __cplusplus
}
#endif

#endif
