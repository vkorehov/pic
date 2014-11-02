/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#define I2C_MYADDR 0x14                    // This device I2C address
#define ALLOW_TIMEOUT 10

extern unsigned int tick_count;
extern unsigned char state;
extern unsigned char timeout;

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void InitApp(void);         /* I/O and Peripheral Initialization */
inline void leak(void);
inline void allow(void);
inline void release(void);
inline void reset(void);
