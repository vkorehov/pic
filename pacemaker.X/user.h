/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define I2C_WRITE 0
#define I2C_READ 1
#define I2C_SLAVE 0x00                    // General Call (broadcast)
#define I2C_RETRIES 1

extern unsigned char i2c_error;
extern unsigned char i2c_ack;
extern unsigned char i2c_own_bus;
extern unsigned char i2c_retry;
extern unsigned short i2c_collisions;
extern unsigned char pacemaker_state;
extern unsigned int sync_every; // 1 second/10mS (50Hz half-period)

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void i2c_command0();
void i2c_init();
void i2c_destroy();
void InitApp(void);         /* I/O and Peripheral Initialization */
inline unsigned int read_tmr1();
inline void write_tmr1(unsigned int v);
