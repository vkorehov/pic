/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#define I2C_WRITE 0
#define I2C_READ 1

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
