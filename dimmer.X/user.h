/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#define I2C_MYADDR 0x0b                    // This device I2C address
// pickit serial communication states
#define I2C_NO_TRANSACTION					0
#define I2C_SLAVE_ADDRESS_RECEIVED			1
#define I2C_WORD_ADDRESS_RECEIVED			2
#define I2C_READ_ADDRESS					3
#define I2C_READ_DATA						4
#define I2C_MASTER_NACK						5

extern unsigned int switch_count;
extern unsigned char state;
extern unsigned char dim;

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void i2c_init();
void do_i2c_tasks(void);

void InitApp(void);         /* I/O and Peripheral Initialization */

inline unsigned int read_tmr1();
inline void write_tmr1(unsigned int v);
