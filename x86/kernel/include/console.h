#ifndef __CONSOLE_H__
#define __CONSOLE_H__

/*
 * The interface for a console, or any other device that wants to capture
 * console messages (printer driver?)
 *
 * If a console driver is marked CON_BOOT then it will be auto-unregistered
 * when the first real console is registered.  This is for early-printk drivers.
 */

#define CON_PRINTBUFFER (1)
#define CON_CONSDEV     (2) /* Last on the command line */
#define CON_ENABLED     (4)
#define CON_BOOT        (8)
#define CON_ANYTIME     (16) /* Safe to call when cpu is offline */
#define CON_BRL         (32) /* Used for a braille device */


struct console {
  char name[16];
  void (*write)(struct console* , const char*, unsigned);
  int (*read)(struct console*, char*, unsigned);
  /* tty driver */
  void (*unblank)(void);
  int (*setup)(struct console* , int *);
  int (*early_setup)(void);
  short flags;
  short index;
  int cflag;
  void *data;
  struct console* next;
};

extern void register_console(struct console* );
extern int  unregister_console(struct console* );

#endif  // __CONSOLE_H__
