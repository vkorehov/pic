/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#define I2C_MYADDR 0x13                    // This device I2C address
extern unsigned char i2c_master;
extern unsigned char notify_state;

extern const unsigned char crc8_table[];
extern int steps;
extern unsigned int sensor_values[2];
extern unsigned char pulses[4];
extern unsigned char pulse_rates[4];
extern unsigned long switch_timeouts[4];
extern unsigned long pwm_timeout;
extern unsigned short ticks;
#define SWITCH_TIMEOUT 0x16000
#define PWM_TIMEOUT 0x10000
//#define UNITTEST
extern unsigned char kp;
extern unsigned char kd;
extern unsigned char ki;
extern unsigned char pid_enabled;
extern unsigned int target;
extern unsigned char last_ctrl;
extern int last_error;

void on(unsigned char sw);
void off(unsigned char sw);
void pwm_en(void);
void pwm_duty(unsigned char duty1_l, unsigned char duty1_h, unsigned char duty2_l, unsigned char duty2_h);
#define WAIT_SLOW (0xffff - 32000)
#define WAIT_MED (0xffff - 16000)
#define WAIT_FAST (0xffff - 10000)
#define STEPS_SLOW 48
#define STEPS_MED 128

unsigned char eeprom_read(unsigned char address);
void eeprom_write(unsigned char address, unsigned char data);

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void i2c_init();
void InitApp(void);         /* I/O and Peripheral Initialization */

inline unsigned int read_tmr1();
inline void write_tmr1(unsigned int v);
