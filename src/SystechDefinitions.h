#include <stdio.h>

#define DEBUG_LEVEL_INFO		0x0F	// 0x1000b
#define DEBUG_LEVEL_WARN		0x07	// 0x0100b
#define DEBUG_LEVEL_ERROR		0x03	// 0x0010b
#define DEBUG_LEVEL_CRIT		0x01	// 0x0001b

#ifndef DebugLevel
  #define DebugLevel DEBUG_LEVEL_WARN
#endif


// Debug Macros

#define DEBUG_INFO(...) do{            \
    printf("[Info    ]:  %s(%d): ", __FUNCTION__, __LINE__);                \
    printf(__VA_ARGS__);                                                    \
    } while(0)

#define DEBUG_WARN(...) do{            \
    printf("[Critical]:  %s(%d): ", __FUNCTION__, __LINE__);                \
    printf(__VA_ARGS__);                                                    \
    printf("\n\t\tFrom File: %s\n", __FILE__);                              \
    } while(0)

#define DEBUG_ERROR(...) do{          \
    printf("[Error   ]:  %s(%d): ", __FUNCTION__, __LINE__);                \
    printf(__VA_ARGS__);                                                    \
    printf("\n\t\tFrom File: %s\n", __FILE__);                              \
    } while(0)

#define DEBUG_CRIT(...) do{            \
    printf("[Critical]:  %s(%d): ", __FUNCTION__, __LINE__);                \
    printf(__VA_ARGS__);                                                    \
    printf("\n\t\tFrom File: %s\n", __FILE__);                              \
    } while(0)
