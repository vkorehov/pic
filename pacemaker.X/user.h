/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define I2C_WRITE 0
#define I2C_READ 1
#define I2C_MYADDR 0x11                    // This device I2C address
#define I2C_SLAVE 0x00                    // General Call (broadcast)
#define I2C_RETRY_DELAY  509
#define I2C_RETRIES 1

extern unsigned char i2c_error;
extern unsigned char i2c_ack;
/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void i2c_command0();
void i2c_init();
void i2c_destroy();
void InitApp(void);         /* I/O and Peripheral Initialization */
