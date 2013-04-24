#include "types.h"
#include "console.h"
#include "linkage.h"
#include "kernel.h"
#include "do_div.h"


/* printk's without a loglevel use this.. */
#define DEFAULT_MESSAGE_LOGLEVEL 4 /* KERN_WARNING */

/* We show everything that is MORE important than this.. */
#define MINIMUM_CONSOLE_LOGLEVEL 1 /* Minimum loglevel we let people use */
#define DEFAULT_CONSOLE_LOGLEVEL 7 /* anything MORE serious than KERN_DEBUG */

int console_printk[4] = {
  DEFAULT_CONSOLE_LOGLEVEL,       /* console_loglevel */
  DEFAULT_MESSAGE_LOGLEVEL,       /* default_message_loglevel */
  MINIMUM_CONSOLE_LOGLEVEL,       /* minimum_console_loglevel */
  DEFAULT_CONSOLE_LOGLEVEL,       /* default_console_loglevel */
};

/* cpu currently holding logbuf_lock */
static volatile unsigned int printk_cpu = UINT_MAX;

#if defined(CONFIG_PRINTK_TIME)
static int printk_time = 1;
#else
static int printk_time = 0;
#endif

void register_console(struct console* console) {
}


static const char recursion_bug_msg [] =
    KERN_CRIT "BUG: recent printk recursion!\n";
static int recursion_bug;
static int new_text_line = 1;
static char printk_buf[1024];

#define __LOG_BUF_LEN (1024  * 16)
static char __log_buf[__LOG_BUF_LEN];
static char *log_buf = __log_buf;
static int log_buf_len = __LOG_BUF_LEN;
/* Number of chars produced since last read+clear operation */
static unsigned logged_chars;

#define LOG_BUF_MASK (log_buf_len-1)
#define LOG_BUF(idx) (log_buf[(idx) & LOG_BUF_MASK])

/*
 * The indices into log_buf are not constrained to log_buf_len - they
 * must be masked before subscripting
 */
/* Index into log_buf: next char to be read by syslog() */
static unsigned log_start;
/* Index into log_buf: next char to be sent to consoles */
static unsigned con_start;
/* Index into log_buf: most-recently-written-char + 1 */
static unsigned log_end;        


static void emit_log_char(char c)
{
  LOG_BUF(log_end) = c;
  log_end++;
  if (log_end - log_start > log_buf_len) {
    log_start = log_end - log_buf_len;
  }
  
  if (log_end - con_start > log_buf_len) {
    con_start = log_end - log_buf_len;
  }
  
  if (logged_chars < log_buf_len) {
    logged_chars++;
  }
}


asmlinkage int vprintk(const char *fmt, va_list args)
{
  int printed_len = 0;
  int current_log_level = default_message_loglevel;
  unsigned long flags;
  int this_cpu;
  char *p;

  /*
  boot_delay_msec();
  printk_delay();

  preempt_disable();
  // This stops the holder of console_sem just where we want him
  raw_local_irq_save(flags);
  this_cpu = smp_processor_id();
  */

  // Ouch, printk recursed into itself!
  /*
  if (unlikely(printk_cpu == this_cpu)) {
    
    // If a crash is occurring during printk() on this CPU,
    // then try to get the crash message out but make sure
    // we can't deadlock. Otherwise just return to avoid the
    // recursion and return - but flag the recursion so that
    // it can be printed at the next appropriate moment:
    if (!oops_in_progress) {
      recursion_bug = 1;
      goto out_restore_irqs;
    }
    zap_locks();
  }
  */

  /*
  lockdep_off();
  spin_lock(&logbuf_lock);
  printk_cpu = this_cpu;
  */
  /*
  if (recursion_bug) {
    recursion_bug = 0;
    strcpy(printk_buf, recursion_bug_msg);
    printed_len = strlen(recursion_bug_msg);
  }
  */
  
  /* Emit the output into the temporary buffer */
  printed_len += vscnprintf(printk_buf + printed_len,
                            sizeof(printk_buf) - printed_len, fmt, args);


  p = printk_buf;

  /* Do we have a loglevel in the string? */
  if (p[0] == '<') {
    unsigned char c = p[1];
    if (c && p[2] == '>') {
      switch (c) {
        case '0' ... '7': /* loglevel */
          current_log_level = c - '0';
          /* Fallthrough - make sure we're on a new line */
        case 'd': /* KERN_DEFAULT */
          if (!new_text_line) {
            emit_log_char('\n');
            new_text_line = 1;
          }
          /* Fallthrough - skip the loglevel */
        case 'c': /* KERN_CONT */
          p += 3;
          break;
      }
    }
  }
  /*
   * Copy the output into log_buf.  If the caller didn't provide
   * appropriate log level tags, we insert them here
   */
  for ( ; *p; p++) {
    if (new_text_line) {
      /* Always output the token */
      emit_log_char('<');
      emit_log_char(current_log_level + '0');
      emit_log_char('>');
      printed_len += 3;
      new_text_line = 0;
      /*
      if (printk_time) {
        // Follow the token with the time
        char tbuf[50], *tp;
        unsigned tlen;
        unsigned long long t;
        unsigned long nanosec_rem;

        t = cpu_clock(printk_cpu);
        nanosec_rem = do_div(t, 1000000000);
        tlen = sprintf(tbuf, "[%5lu.%06lu] ",
                       (unsigned long) t,
                       nanosec_rem / 1000);

        for (tp = tbuf; tp < tbuf + tlen; tp++)
          emit_log_char(*tp);
        printed_len += tlen;
      }
      */

      if (!*p)
        break;
    }

    emit_log_char(*p);
    if (*p == '\n')
      new_text_line = 1;
  }

  /*
   * Try to acquire and then immediately release the
   * console semaphore. The release will do all the
   * actual magic (print out buffers, wake up klogd,
   * etc).
   *
   * The acquire_console_semaphore_for_printk() function
   * will release 'logbuf_lock' regardless of whether it
   * actually gets the semaphore or not.
   */
  /*
  if (acquire_console_semaphore_for_printk(this_cpu))
    release_console_sem();

  lockdep_on();
  */
out_restore_irqs:
  /*
  raw_local_irq_restore(flags);

  preempt_enable();
  */
  return printed_len;
}


/**
 * printk - print a kernel message
 * @fmt: format string
 *
 * This is printk().  It can be called from any context.  We want it to work.
 *
 * We try to grab the console_sem.  If we succeed, it's easy - we log the output and
 * call the console drivers.  If we fail to get the semaphore we place the output
 * into the log buffer and return.  The current holder of the console_sem will
 * notice the new output in release_console_sem() and will send it to the
 * consoles before releasing the semaphore.
 *
 * One effect of this deferred printing is that code which calls printk() and
 * then changes console_loglevel may break. This is because console_loglevel
 * is inspected when the actual printing occurs.
 *
 * See also:
 * printf(3)
 *
 * See the vsnprintf() documentation for format string extensions over C99.
 */

asmlinkage int printk(const char *fmt, ...)
{
  va_list args;
  int r;

  va_start(args, fmt);
  r = vprintk(fmt, args);
  va_end(args);

  return r;
}

