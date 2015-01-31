#include <stdio.h>
#include <iostream>

#define DEBUG_LEVEL_INFO		0x0F	// 0x1000b
#define DEBUG_LEVEL_WARN		0x07	// 0x0100b
#define DEBUG_LEVEL_ERROR		0x03	// 0x0010b
#define DEBUG_LEVEL_CRIT		0x01	// 0x0001b

// TODO: [Critical] Fix Preprocessor macros errors
// Debug Functions
#define DEBUG_INFO(fmt, args...)  if ((DebugLevel & DEBUG_LEVEL_INFO) == DEBUG_LEVEL_INFO) \
    printf( "[Info    ]:  %s(%d): " fmt "\n", __FUNCTION__, __LINE__, ## args)
#define DEBUG_WARN(fmt, args...)  if ((DebugLevel & DEBUG_LEVEL_WARN) == DEBUG_LEVEL_WARN) \
    printf( "[Warning ]:  %s(%d): " fmt "\t\tFrom File: %s\n", __FUNCTION__, __LINE__, ## args, __FILE__)
#define DEBUG_ERROR(fmt, args...) if ((DebugLevel & DEBUG_LEVEL_ERROR) == DEBUG_LEVEL_ERROR) \
    printf( "[Error   ]:  %s(%d): " fmt "\t\tFrom File: %s\n", __FUNCTION__, __LINE__, ## args, __FILE__)
#define DEBUG_CRIT(fmt, args...)  if ((DebugLevel & DEBUG_LEVEL_CRIT) == DEBUG_LEVEL_CRIT) \
    printf( "[Critical]:  %s(%d): " fmt "\t\tFrom File: %s\n", __FUNCTION__, __LINE__, ## args, __FILE__)

