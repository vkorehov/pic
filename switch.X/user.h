/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#define I2C_MYADDR 0x34                   // This device I2C address


#define SWITCH_ON_DURATION                                      0xffff
#define SWITCH_ON_DURATION_MULT                                 40

extern unsigned char switch_dur_mult;

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
