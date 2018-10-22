/*
 * @TAG(OTHER_MIT)
 */
/*
 * Copyright © 2005-2014 Rich Felker, et al.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <sel4/arch/constants.h>
#include <thread.h>

#if ((__ARM_ARCH_6K__ || __ARM_ARCH_6ZK__) && !__thumb__) \
 || __ARM_ARCH_7A__ || __ARM_ARCH_7R__ || __ARM_ARCH >= 7

static inline thread_t *__sel4runtime_tls_self()
{
    char *p;
    __asm__ __volatile__ ( "mrc p15,0,%0,c13,c0,3" : "=r"(p) );
    return (void *)(p - sizeof(thread_t));
}

#elif defined(CONFIG_IPC_BUF_GLOBALS_FRAME)

/*
 * In the case of early versions of ARMv6, there are no hardware
 * registers provided for thread-local identifiers. seL4 resolves this
 * by placing the IPC buffer address and thread pointer in a
 * `GlobalsFrame` mapped at the same address in all virtual address
 * spaces. The IPC buffer and thread pointer occupy the first two words
 * in this frame respectively.
 */
static inline thread_t *__sel4runtime_tls_self()
{
    void **globals_frame = (void **)seL4_GlobalsFrame;
    return (void *)(globals_frame[1] - sizeof(thread_t));
}

#else

#error "ARM architectures below ARMv6 are unsupported"

#endif

#define TLS_ABOVE_TP
#define GAP_ABOVE_TP 8
#define TP_ADJ(p) ((char *)(p) + sizeof(thread_t))