/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define I2C_WRITE 0
#define I2C_READ 1
#define I2C_MYADDR 0x10                    // This device I2C address
#define I2C_SLAVE 0x40                    // Slave device I2C address
#define I2C_RETRY_DELAY  713
#define I2C_RETRIES 3

extern unsigned short cps0_value;
extern unsigned short cps1_value;
extern unsigned short cps2_value;
extern unsigned short cps3_value;
extern unsigned char i2c_error;
extern unsigned char i2c_ack;
/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
unsigned char i2c_command8_and_read(unsigned char command);
void i2c_command16(unsigned short command);
void i2c_command8(unsigned char command);
void i2c_init();
void i2c_destroy();
void InitApp(void);         /* I/O and Peripheral Initialization */
