/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define I2C_WRITE 0
#define I2C_READ 1
#define I2C_MYADDR 0x67                    // This device I2C address
#define I2C_SLAVE 0x40                    // Slave device I2C address
#define I2C_RETRY_DELAY  713
#define I2C_RETRIES 1

//#define LED0 PORTCbits.RC6
//#define LED2 PORTCbits.RC7
//#define LED4 PORTCbits.RC5

#define LED4_BIT (1 << 5)
#define LED2_BIT (1 << 7)
#define LED0_BIT (1 << 6)

extern int average[8];
extern unsigned char average_every[8];
extern unsigned char button_trigger_on[8];
extern unsigned char skip_cps;
extern unsigned char sequential_press[8];

extern unsigned char i2c_error;
extern unsigned char i2c_ack;
extern unsigned int i2c_collisions;
extern unsigned short beep;
extern unsigned char command_to_send;

extern unsigned char debug_byte0;
extern unsigned char debug_byte1;
extern unsigned char debug_byte2;
extern unsigned char debug_byte3;
extern unsigned char debug;
extern unsigned int tmr2_ticks;

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void i2c_command16(unsigned short command);
void i2c_command8(unsigned char command);
void i2c_init();
void i2c_destroy();
void InitApp(void);         /* I/O and Peripheral Initialization */
void dpot_increment(unsigned char count);
void dpot_decrement(unsigned char count);
void i2c_dbg(unsigned char b1, unsigned char b2, unsigned char b3, unsigned char b4);