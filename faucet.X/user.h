/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#define I2C_MYADDR 0x70                   // This device I2C address
//#define MOVEMENT_ENABLED

#define SWITCH_ON_DURATION                                      0xffff
#define SWITCH_ON_DURATION_MULT                                 255
//#define PWM_32K
#define PWM_500H
//#define DEBUG
#define AD_DELAY     0x1
#define AD_CUTOFF    512
#define INIT_VECTOR  100


#define FAUCET_TIMEOUT 100

extern unsigned char switch_dur_mult;
extern unsigned char movement_on_dim;
extern unsigned char movement_state;
extern unsigned char faucet_on;
extern unsigned int faucet_timeout;
extern unsigned char hit_a3;
extern unsigned char hit_a5;

#define DHT22_MAX_BYTES 5
#define DHT22_CUTOFF_TIME 0x18
extern unsigned char dht22_state;
extern unsigned char dht22_index;
extern unsigned char dht22_bit_index;
extern unsigned char dht22_bits[DHT22_MAX_BYTES];

void start_read_dht22(void);
void start_read_dht22_pullup(void);
void dht22_abort(void);

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void InitApp(void);         /* I/O and Peripheral Initialization */
void pwm_init(void);
void dht22_init(void);
void on(unsigned char dim);
void off(void);
inline void write_tmr1(unsigned int v);
extern const unsigned char crc8_table[];
extern unsigned int sensor1;
extern unsigned int sensor2;


