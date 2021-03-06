#ifndef DEFS_H
#define DEFS_H

#ifdef USING_SOURCE_SF
	#define WXDLLIMPEXP_SF
    #define WXDLLIMPEXP_DATA_SF(type) type
#elif defined( LIB_USINGDLL )
    #define WXDLLIMPEXP_SF
    #define WXDLLIMPEXP_DATA_SF(type)
#elif defined( WXMAKINGDLL_SF )
    #define WXMAKINGDLL
    #define WXDLLIMPEXP_SF WXEXPORT
    #define WXDLLIMPEXP_DATA_SF(type) WXEXPORT type
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_SF WXIMPORT
    #define WXDLLIMPEXP_DATA_SF(type) WXIMPORT type
#else // not making nor using DLL
    #define WXDLLIMPEXP_SF
    #define WXDLLIMPEXP_DATA_SF(type) type
#endif

#endif//DEFS_H
