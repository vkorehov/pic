/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
// 58	blinds	5
// xx	blinds	6 broken
// 5A	blinds	3
// 15	blinds	2 - short
// 5D	blinds	1 - Inverted
// 59	blinds	4 - Inverted

#define I2C_MYADDR 0x58                  // This device I2C address
//#define I2C_MYADDR 0x58                  // This device I2C address
//#define I2C_MYADDR 0x59                  // This device I2C address
//#define I2C_MYADDR 0x5a                  // This device I2C address
//#define I2C_MYADDR 0x5b                  // This device I2C address
//#define I2C_MYADDR 0x5d                  // This device I2C address
//#define I2C_MYADDR 0x5e                  // This device I2C address

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void InitApp(void);         /* I/O and Peripheral Initialization */
void on(void);
void off(void);
extern const unsigned char crc8_table[];
unsigned char eeprom_read(unsigned char address);
void eeprom_write(unsigned char address, unsigned char data);
inline void write_ticks(unsigned int val);
inline unsigned int read_ticks();

extern unsigned int position;
extern unsigned int next_position;
extern unsigned int command_position;
extern unsigned char state;
#define STATE_CCV  1
#define STATE_CV   2
#define STATE_IDLE 0

