


#define	__CODE32_CS    0x08
#define    __DATA32_DS    0x10
#define	__VIDEO_BASE	0x0B8000

#define	PIC_MASTER_CMD 0x20
#define	PIC_MASTER_IMR 0x21
#define	PIC_MASTER_ISR PIC_MASTER_CMD
#define	PIC_SLAVE_CMD  0xa0
#define	PIC_SLAVE_IMR  0xa1
#define	PIC_CASCADE_IR 2
#define	MASTER_ICW4_DEFAULT 0x01
#define	SLAVE_ICW4_DEFAULT 0x01
#define	PIC_ICW4_AEOI 0x2

#define	PIT_MODE 0x43
#define	PIT_CH0 0x40
#define	PIT_CH2 0x42


#define HZ     100
#define CLOCK_TICK_RATE 1193182
#define LATCH  ((CLOCK_TICK_RATE + HZ/2) / HZ)


#define IDT_ADDR 0x9000
#define IDT_VEC_ITEM 50

#define I8253_MASTER_INT 0x20
#define I8253_SLAVE_INT  0x28
