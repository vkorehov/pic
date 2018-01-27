/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define I2C_WRITE 0
#define I2C_READ 1
// This device I2C address
#define I2C_MYADDR 0x64
//#define DEBUG
#define I2C_SLAVE 0x40                    // Slave device I2C address
#define I2C_RETRY_DELAY  713
#define I2C_RETRIES 1

#define SCANS_PER_DECODE 300
#define INIT_VECTOR 0x100
#define TIMEOUT_STEPS 64


//#define DEBUG
extern unsigned int tripped_readings[8];
extern unsigned int last_readings[8];
extern unsigned int readings[8];
extern unsigned int readings_counter;
extern unsigned int beep;
extern unsigned char state;
extern const unsigned char crc8_table[];
/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void InitApp(void);         /* I/O and Peripheral Initialization */
void dpot_increment(unsigned char count);
void dpot_decrement(unsigned char count);
