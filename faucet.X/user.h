/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#define I2C_MYADDR 0x70                   // This device I2C address

#define SWITCH_ON_DURATION                                      0xffff
#define SWITCH_ON_DURATION_MULT                                 255
//#define PWM_32K
#define PWM_500H
//#define DEBUG
#define AD_DELAY     0x100
#define AD_CUTOFF1   900
#define AD_CUTOFF2   500

#define AD_36DEG     865 // 
#define AD_30DEG     800 // 
#define AD_20DEG     500 // 

#define INIT_VECTOR  100


#define FAUCET_TIMEOUT 4000

extern unsigned char faucet_on;
extern unsigned int faucet_timeout;
extern unsigned int ticker;

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void InitApp(void);         /* I/O and Peripheral Initialization */
inline void write_tmr1(unsigned int v);
extern const unsigned char crc8_table[];
extern unsigned int sensor1;
extern unsigned int sensor2;
extern unsigned int sensor3;


