#ifdef WINDOWS
#include <windows.h>
#endif
#include <stdlib.h>

#ifdef WINDOWS
#include <conio.h>
#endif

#include <stdio.h>
#include <iostream.h>
#include <malloc.h>
#include <lbInterfaces.h>

#ifndef __LB_CONFIG_HOOK__
#define __LB_CONFIG_HOOK__

#ifdef LINUX
#define HINSTANCE void*
#endif

#define DLLEXPORT

#ifdef HOOK_DLL
#undef DLLEXPORT
#ifdef WINDOWS
#define DLLEXPORT LB_DLLEXPORT
#endif

#endif

#ifndef HOOK_DLL
#undef DLLEXPORT
#ifdef WINDOWS
#define DLLEXPORT LB_DLLIMPORT
#endif

#endif


// Object tracking
DLLEXPORT void LB_STDCALL set_trackObject(char* track);
DLLEXPORT char* LB_STDCALL get_trackObject();
DLLEXPORT void LB_STDCALL track_Object(lb_I_Unknown* o, char* msg);

DLLEXPORT HINSTANCE LB_STDCALL getModuleHandle();
DLLEXPORT HINSTANCE LB_STDCALL getLBModuleHandle();

DLLEXPORT void LB_STDCALL setModuleHandle(HINSTANCE h);
DLLEXPORT void LB_STDCALL setLBModuleHandle(HINSTANCE h);

/**
 * Platform independend module loader
 */
/*...slbErrCodes LB_STDCALL lbLoadModule\40\const char\42\ name\44\ HINSTANCE \38\ hinst\41\:0:*/
lbErrCodes DLLEXPORT LB_STDCALL lbLoadModule(const char* name, HINSTANCE & hinst);
/*...e*/
/*...slbErrCodes LB_STDCALL lbGetFunctionPtr\40\const char\42\ name\44\ const HINSTANCE \38\ hinst\44\ void\42\\42\ pfn\41\:0:*/
lbErrCodes DLLEXPORT LB_STDCALL lbGetFunctionPtr(const char* name, HINSTANCE hinst, void** pfn);
/*...e*/


/**
 * A moduleinstance is a factory object for all other further instances
 * of registered modules.
 */

/*...slb_I_Module\42\ LB_STDCALL getModuleInstance\40\\41\:0:*/
DLLEXPORT lb_I_Module* LB_STDCALL getModuleInstance();
/*...e*/
/*...slbErrCodes LB_STDCALL releaseInstance\40\lb_I_Unknown\42\ inst\41\:0:*/
lbErrCodes LB_STDCALL releaseInstance(lb_I_Unknown* inst);
/*...e*/
/*...svoid LB_STDCALL unHookAll\40\\41\:0:*/
DLLEXPORT void LB_STDCALL unHookAll();
/*...e*/

/*...sLogging macros:0:*/
DLLEXPORT lb_I_Log *log;
DLLEXPORT int isInitializing;

#ifdef bla
#ifndef LOG_DEFINED
/*...sCL_LOG:0:*/
#define CL_LOG(msg) \
{ \
	char *datei = strrchr(__FILE__, '\\'); \
	if (datei == NULL) { \
		datei = __FILE__; \
	} \
	else { \
		datei++; \
	} \
	cout << "File: " << datei << ", Line: " << __LINE__ << ", Msg: " << msg << endl; \
}
/*...e*/
#endif
#ifdef LOG_DEFINED
/*...sCL_LOG:0:*/
#define CL_LOG(msg) \
{ \
	char *datei = strrchr(__FILE__, '\\'); \
	if (datei == NULL) { \
		datei = __FILE__; \
	} \
	else { \
		datei++; \
	} \
	cout << "File: " << datei << ", Line: " << __LINE__ << ", Msg: " << msg << endl; \
	if (log != NULL) log->log(msg, __LINE__, __FILE__); \
}
/*...e*/
#endif
#endif

#ifdef bla
#define CL_LOG(msg) \
{ \
	char *datei = strrchr(__FILE__, '\\'); \
	char *buf = NULL; \
	buf = (char*) malloc(1000); \
	if (datei == NULL) { \
		datei = __FILE__; \
	} \
	else { \
		datei++; \
	} \
	sprintf(buf, "File: %s, Line: %d, Msg: %s\n", datei, __LINE__, msg); \
	CL_doLog("c:\\log\\lbDMF.log", buf); \
	free((void*) buf); \
}
#endif

//#ifdef bla
#define CL_LOG(msg) \
{ \
} 
//#endif

/*...sGET_LOG_INSTANCE:0:*/
#define GET_LOG_INSTANCE \
			if (log == NULL) { \
				isInitializing = 1; \
				lb_I_Module* modMan = getModuleInstance(); \
				\
				if (modMan != NULL) { \
					lb_I_Unknown *Unknown = NULL; \
					modMan->initialize(); \
					lbErrCodes err = modMan->request("lb_I_Log", &Unknown); \
					\
					if (Unknown != NULL) { \
						Unknown->queryInterface("lb_I_Log", (void**) &log, __FILE__, __LINE__); \
						if (log == NULL) { \
							CL_LOG("Unknown object has no interface for lb_I_Log"); \
							exit (1); \
						} else { \
						} \
					} else { \
						char buf[1000] = ""; \
						sprintf(buf, "%s %d %s", "Instance could not be created, errcode is ", err, "."); \
						CL_LOG(buf); \
						getch(); \
						exit(1); \
					} \
				} else { \
					CL_LOG("Module manager could not be created"); \
					getch(); \
					exit(1); \
				} \
			} \
			isInitializing = 0;
/*...e*/

/*...sLOG:0:*/
#define LOG(msg)	\
			if (isInitializing != 0) { \
				cout << "Tried to log while initializing the logger." << \
				"Msg: " << msg << " File: " << __FILE__ << " Line: " << __LINE__ << endl; \
			} else { \
				GET_LOG_INSTANCE \
				log->log(msg, __LINE__, __FILE__); \
			}
/*...e*/
/*...sLOGENABLE:0:*/
#define LOGENABLE       \
			if (isInitializing != 0) { \
				cout << "Tried to log while initializing the logger." << endl; \
			} else { \
				GET_LOG_INSTANCE \
				log->enable(); \
			}
/*...e*/
/*...sLOGDISABLE:0:*/
#define LOGDISABLE      \
			if (isInitializing != 0) { \
				cout << "Tried to log while initializing the logger." << endl; \
			} else { \
				GET_LOG_INSTANCE \
				log->disable(); \
			}
/*...e*/
/*...sLOGSTART:0:*/
#define LOGSTART        \
			if (isInitializing != 0) { \
				cout << "Tried to log while initializing the logger." << endl; \
			} else { \
				GET_LOG_INSTANCE \
				log->event_begin(); \
			}
/*...e*/
/*...sLOGEND:0:*/
#define LOGEND          \
			if (isInitializing != 0) { \
				cout << "Tried to log while initializing the logger." << endl; \
			} else { \
				GET_LOG_INSTANCE \
				log->event_end(); \
			}
/*...e*/
/*...sLOGPREFIX:0:*/
#define LOGPREFIX(a)    \
			if (isInitializing != 0) { \
				cout << "Tried to log while initializing the logger." << endl; \
			} else { \
				GET_LOG_INSTANCE \
				log->setPrefix(a); \
			}
/*...e*/
/*...e*/

DLLEXPORT void LB_STDCALL CL_doLog(char* f, char* msg);

#endif // __LB_CONFIG_HOOK__
