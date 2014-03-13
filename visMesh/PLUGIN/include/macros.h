#ifndef _MACROS
#define _MACROS

// Debugging info
#define DBGPRINT(MSG)                               \
	if(debug) {                                     \
	    cout << "visMesh [debug]: " << MSG << endl; \
	}                                               \

// Error checking
#define CHECKERROR(STAT,MSG)                        \
    if (STAT != MS::kSuccess) {                     \
        cerr << "visMesh [ERROR]: " << MSG << endl; \
        return MS::kFailure;                        \
    }

#define CHECKERRORNORET(STAT,MSG)                   \
    if (STAT != MS::kSuccess) {                     \
        cerr << "visMesh [ERROR]: " << MSG << endl; \
    }
#endif //_MACROS