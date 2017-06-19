/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define I2C_WRITE 0
#define I2C_READ 1
#define I2C_MYADDR 0x65                    // This device I2C address
#define I2C_SLAVE 0x40                    // Slave device I2C address
#define I2C_RETRY_DELAY  713
#define I2C_RETRIES 1

//#define SIMULATOR

#if  I2C_MYADDR == 0x65
#define LOW_LIGHT 20
#define AVERAGE_DIV 16
#define PCT_ON 7
#define PCT_OFF 5 // 2% hyst
#define PCT_2_ON 11
#define PCT_2_OFF 9 // 2% hyst
#define TRI_BUTTONS
#define LATCH_CH_2
#define LED4_BIT (1 << 5)
#define LED2_BIT (1 << 7)
#define LED0_BIT (1 << 6)
#endif

#if  I2C_MYADDR == 0x67
#define LOW_LIGHT 20
#define AVERAGE_DIV 16
#define PCT_ON 7
#define PCT_OFF 5 // 2% hyst
#define PCT_2_ON 11
#define PCT_2_OFF 9 // 2% hyst
#define TRI_BUTTONS
#define LATCH_CH_2
#define LED4_BIT (1 << 5)
#define LED2_BIT (1 << 7)
#define LED0_BIT (1 << 6)
#endif
#if  I2C_MYADDR == 0x69
#define LOW_LIGHT 20
#define AVERAGE_DIV 16
#define PCT_ON 16
#define PCT_OFF 13// 2% hyst
#define PCT_2_ON 16
#define PCT_2_OFF 13 // 2% hyst
#define TRI_BUTTONS
#define LATCH_CH_2
#define LED4_BIT (1 << 5)
#define LED2_BIT (1 << 7)
#define LED0_BIT (1 << 6)
#endif


#if  I2C_MYADDR == 0x63
#define LOW_LIGHT 20
// RC5 and RC7 CPS3(ch == 0) CPS5(ch==2) only
#define CH2_CH4_BUTTONS
#define AVERAGE_DIV 16
#define PCT_ON 16
#define PCT_OFF 13 // 4% hyst
#define PCT_0_ON 16
#define PCT_0_OFF 13 // 4% hyst
#define LATCH_CH_4
#define LED4_BIT (1 << 5)
#define LED2_BIT (1 << 7)
#define LED0_BIT (1 << 6)
#endif

#if  I2C_MYADDR == 0x64
// RC5 and RC7 CPS3(ch == 0) CPS5(ch==2) only
#define LOW_LIGHT 1
#define CH2_CH4_BUTTONS
#define AVERAGE_DIV 16
#define PCT_ON 10
#define PCT_OFF 8 // 4% hyst
#define PCT_0_ON 10
#define PCT_0_OFF 8 // 4% hyst
#define LATCH_CH_4
#define LED4_BIT (1 << 5)
#define LED2_BIT (1 << 7)
#define LED0_BIT (1 << 6)
#endif

#ifdef SIMULATOR
void run_tests();
#endif
inline void process_cps(unsigned char ch);
extern int average[8];
extern int raw[8];
extern unsigned int percent[8];
extern unsigned char average_every[8];
extern unsigned char button_trigger_on[8];
extern unsigned char skip_cps;
extern unsigned char sequential_press[8];

//extern unsigned char i2c_error;
//extern unsigned char i2c_ack;
//extern unsigned int i2c_collisions;
extern unsigned short beep;
extern unsigned char command_to_send;

//extern unsigned char debug_byte0;
//extern unsigned char debug_byte1;
//extern unsigned char debug_byte2;
//extern unsigned char debug_byte3;
//extern unsigned char debug;
extern unsigned int tmr2_ticks;
extern unsigned char latching;
extern unsigned char percents_on[8];
extern unsigned char percents_off[8];
extern unsigned char bright_on;
extern unsigned char bright_off;

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
//void i2c_command16(unsigned short command);
//void i2c_command8(unsigned char command);
//void i2c_init();
//void i2c_destroy();
void InitApp(void);         /* I/O and Peripheral Initialization */
void dpot_increment(unsigned char count);
void dpot_decrement(unsigned char count);
//void i2c_dbg(unsigned char b1, unsigned char b2, unsigned char b3, unsigned char b4);
