/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define I2C_WRITE 0
#define I2C_READ 1
#define I2C_MYADDR 0x10                    // This device I2C address
#define I2C_SLAVE 0x40                    // Slave device I2C address
#define I2C_RETRY_DELAY  713
#define I2C_RETRIES 3

extern unsigned int raw[];
extern unsigned int average[];
extern unsigned int trip[];
extern unsigned char state[];

extern unsigned char i2c_error;
extern unsigned char i2c_ack;
extern unsigned int i2c_collisions;

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