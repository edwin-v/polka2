#ifndef _POLKA_DEFS_H_
#define _POLKA_DEFS_H_

#include <sigc++/signal.h>

// common signal definitions
typedef sigc::signal<void, int> IntSignal;
typedef sigc::signal<void, int, int> IntIntSignal;
typedef sigc::signal<void> VoidSignal;


#endif // _POLKA_DEFS_H_
