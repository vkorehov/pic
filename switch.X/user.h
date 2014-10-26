/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#define I2C_MYADDR 0x14                    // This device I2C address

#define SWITCH_ON_DURATION                                      0xfff0
extern unsigned int switch_count;
extern unsigned char temperature;
extern unsigned char humidity;

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void InitApp(void);         /* I/O and Peripheral Initialization */
void on(void);