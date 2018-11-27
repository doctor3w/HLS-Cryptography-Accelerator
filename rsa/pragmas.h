#ifndef PRAGMAS_H
#define PRAGMAS_H

#if HLS_ENABLE_PRAGMAS == 1
#define _DO_PRAGMA(x) _Pragma(#x)
#define HLS_PRAGMA(x) _DO_PRAGMA(HLS x)
#else
#define HLS_PRAGMA(x)
#endif

#endif
