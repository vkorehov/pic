/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#define I2C_MYADDR 0x14                    // This device I2C address

extern int steps;
extern unsigned char sensor_average_every;
extern unsigned int sensor_values[4];
extern unsigned int sensor_values_averages[4];
extern unsigned char move_direction[2];
extern unsigned int steps_to_move[2];
extern unsigned int current_steps[2];
extern unsigned int new_steps[2];

extern unsigned char port_c_switches;
extern unsigned long long switch_timeouts[4];
#define SWITCH_TIMEOUT 0x12000
void on(unsigned char sw);
void off(unsigned char sw);

#define WAIT_SLOW (0xffff - 32000)
#define WAIT_MED (0xffff - 16000)
#define WAIT_FAST (0xffff - 10000)
#define STEPS_SLOW 48
#define STEPS_MED 128

#define EEPROM_COLD_ADDR 0
#define EEPROM_HOT_ADDR 2
unsigned char eeprom_read(unsigned char address);
void eeprom_write(unsigned char address, unsigned char data);
/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void i2c_init();
void InitApp(void);         /* I/O and Peripheral Initialization */

inline unsigned int read_tmr1();
inline void write_tmr1(unsigned int v);
