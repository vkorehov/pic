/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#define I2C_MYADDR 0x14                    // This device I2C address

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void InitApp(void);         /* I/O and Peripheral Initialization */
extern unsigned int d6f_val;
// 1.024V
#define ADC_MIN_BAND 0b01
// 4.096V
#define ADC_MAX_BAND 0b11
#define ADC_BAND_SWITCH_HISTERESIS 0x20

extern unsigned char adc_band;
unsigned int d6f_lerp(unsigned int voltage);
void d6f_init(void);