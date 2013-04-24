#ifndef __ALTERNATIVE_H__
#define __ALTERNATIVE_H__

#include "asm.h"

/* alternative assembly primitive: */
#define ALTERNATIVE(oldinstr, newinstr, feature)                        \
                                                                        \
      "661:\n\t" oldinstr "\n662:\n"                                    \
      ".section .altinstructions,\"a\"\n"                               \
      _ASM_ALIGN "\n"                                                   \
      _ASM_PTR "661b\n"                         /* label           */   \
      _ASM_PTR "663f\n"                         /* new instruction */   \
      "  .byte " __stringify(feature) "\n"      /* feature bit     */   \
      "  .byte 662b-661b\n"                     /* sourcelen       */   \
      "  .byte 664f-663f\n"                     /* replacementlen  */   \
      ".previous\n"                                                     \
      ".section .altinstr_replacement, \"ax\"\n"                        \
      "663:\n\t" newinstr "\n664:\n"            /* replacement     */   \
      ".previous"


/*
 * Alternative instructions for different CPU types or capabilities.
 *
 * This allows to use optimized instructions even on generic binary
 * kernels.
 *
 * length of oldinstr must be longer or equal the length of newinstr
 * It can be padded with nops as needed.
 *
 * For non barrier like inlines please define new variants
 * without volatile and memory clobber.
 */
#define alternative(oldinstr, newinstr, feature)                        \
  asm volatile (ALTERNATIVE(oldinstr, newinstr, feature) : : : "memory")

/*
 * Alternative inline assembly with input.
 *
 * Pecularities:
 * No memory clobber here.
 * Argument numbers start with 1.
 * Best is to use constraints that are fixed size (like (%1) ... "r")
 * If you use variable sized constraints like "m" or "g" in the
 * replacement make sure to pad to the worst case length.
 * Leaving an unused argument 0 to keep API compatibility.
 */
#define alternative_input(oldinstr, newinstr, feature, input...)        \
  asm volatile (ALTERNATIVE(oldinstr, newinstr, feature)          \
                : : "i" (0), ## input)

/* Like alternative_input, but with a single output argument */
#define alternative_io(oldinstr, newinstr, feature, output, input...)   \
  asm volatile (ALTERNATIVE(oldinstr, newinstr, feature)          \
                : output : "i" (0), ## input)


/*
 * Alternative inline assembly for SMP.
 *
 * The LOCK_PREFIX macro defined here replaces the LOCK and
 * LOCK_PREFIX macros used everywhere in the source tree.
 *
 * SMP alternatives use the same data structures as the other
 * alternatives and the X86_FEATURE_UP flag to indicate the case of a
 * UP system running a SMP kernel.  The existing apply_alternatives()
 * works fine for patching a SMP kernel for UP.
 *
 * The SMP alternative tables can be kept after boot and contain both
 * UP and SMP versions of the instructions to allow switching back to
 * SMP at runtime, when hotplugging in a new CPU, which is especially
 * useful in virtualized environments.
 *
 * The very common lock prefix is handled as special case in a
 * separate table which is a pure address list without replacement ptr
 * and size information.  That keeps the table sizes small.
 */

#ifdef CONFIG_SMP
#define LOCK_PREFIX                             \
  ".section .smp_locks,\"a\"\n"                 \
  _ASM_ALIGN "\n"                               \
  _ASM_PTR "661f\n" /* address */               \
  ".previous\n"                                 \
  "661:\n\tlock; "
#else
#define LOCK_PREFIX ""
#endif



#endif  // __ALTERNATIVE_H__
