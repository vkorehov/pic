/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#define I2C_WRITE 0
#define I2C_READ 1
#define I2C_MYADDR 0x10                    // This device I2C address
#define I2C_SLAVE 0x40                    // Slave device I2C address
#define I2C_RETRY_DELAY  713
#define I2C_RETRIES 3

extern unsigned char i2c_error;

extern unsigned int syncperiod;
extern unsigned short hz100_period;
extern unsigned short hz100_bres;
extern unsigned short hz100_command;

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void i2c_check_error();
void i2c_init();
void i2c_destroy();
unsigned short i2c_on_command(unsigned char* command);

void i2c_Wait(void);
void i2c_Write(unsigned char data);
void i2c_Address(unsigned char address, unsigned char mode);

void hz100_clock();
void hz100_sync();
void hz100_1mhz();

void InitApp(void);         /* I/O and Peripheral Initialization */
