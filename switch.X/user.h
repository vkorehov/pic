/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#define I2C_MYADDR 0x14                    // This device I2C address

#define SWITCH_ON_DURATION                                      0xfff0
extern unsigned int switch_count;
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
void on(void);