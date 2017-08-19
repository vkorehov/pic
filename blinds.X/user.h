/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#define I2C_MYADDR 0x55                   // This device I2C address

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void InitApp(void);         /* I/O and Peripheral Initialization */
void on(void);
void off(void);
extern const unsigned char crc8_table[];
unsigned char eeprom_read(unsigned char address);
void eeprom_write(unsigned char address, unsigned char data);
extern unsigned int position;
extern unsigned int next_position;
