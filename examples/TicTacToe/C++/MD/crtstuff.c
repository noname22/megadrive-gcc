/* C++ CTOR / DTOR handling */

#include <stdlib.h>


/* Used by exit procs */
void *__dso_handle = 0;
extern void __call_exitprocs (int code, void *ptr);

typedef void (*func_ptr) (void);

extern func_ptr __CTOR_LIST__[];
extern func_ptr __DTOR_LIST__[];

/* Do all constructors. */
static void __attribute__((used)) __do_global_ctors (void)
{
    do
    {
        unsigned int i, n = (unsigned int) __CTOR_LIST__[0];
        for (i = n; i >= 1; i--)
            __CTOR_LIST__[i] ();
    } while (0);
}

/* Do all destructors. */
static void __attribute__((used)) __do_global_dtors (void)
{
    do
    {
        unsigned int i, n = (unsigned int) __DTOR_LIST__[0];
        for (i = 0; i < n; i++)
            __DTOR_LIST__[i + 1] ();
    } while (0);
}

/* Add function to .init section.  */
static void __attribute__((used, section (".init"))) __std_startup (void)
{
    atexit (__do_global_dtors);         /* First added, last called.  */
    __do_global_ctors ();               /* Do all constructors. */
}

/* Add function to .fini section.  */
static void __attribute__((used, section (".fini"))) __std_cleanup (void)
{
    __call_exitprocs (0, NULL);
}

