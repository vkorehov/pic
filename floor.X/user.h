/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#define I2C_MYADDR 0x71                   // This device I2C address

//#define DEBUG
#define AD_DELAY     0x700
#define AD_CUTOFF    801
#define AD_CUTOFF_HIST 5

#define FL_TIMEOUT 32400
#define FL_RECOVERY_TIMEOUT 42400

#define INIT_VECTOR  100


extern unsigned int  floor_timer;
extern unsigned int  floor_timeout;
extern unsigned char timeout_count;
extern unsigned char enabled;

extern unsigned int  floor_recovery_timeout;
extern unsigned int  reading;
extern unsigned int  state;

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void InitApp(void);         /* I/O and Peripheral Initialization */
inline void on(void);
inline void off(void);
inline void write_tmr1(unsigned int v);
extern const unsigned char crc8_table[];
extern unsigned int sensor1;


