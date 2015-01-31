# Compiler
CC = arm-linux-gnueabihf-g++

# Sources Directory
SOURCEDIR = src/

# Common Flags:
INCLUDES = -I/usr/local/include/opencv2 -I/usr/local/include/CL -I/usr/local/include/
CFLAGS = -Wextra
LFLAGS = 
LIBS = -lopencv_core -lOpenCL -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann -lmali
USER_LIBS = -L/usr/local/lib -L/usr/lib/arm-linux-gnueabihf -L/usr/lib/arm-linux-gnueabihf/mali-egl

# Debug Flags
D_CFLAGS = -g3 -rdynamic
#D_CFLAGS =
D_LFLAGS = 
D_LIBS =

# Release Flags
R_CFLAGS = -O3
R_LFLAGS = 
R_LIBS =

# Sources
D_SRCS = $(shell find $(SOURCEDIR) -name '*.cpp')
R_SRCS = $(shell find $(SOURCEDIR) -name '*.cpp')
OBJS = $(shell sh -c 'cd $(SOURCEDIR);ls | grep .cpp | sed s/cpp/o/g')
D_OBJS = Debug/$(OBJS)
R_OBJS = Release/$(OBJS)

.PHONY: all Debug Release

# All Target
all: Debug Release
	@echo '=== Finished Building target: $@ ==='

# Tool invocations
Debug: $(D_OBJS)
	@echo 'Building target: Debug'
	@echo 'Invoking: GCC C++ Linker'
	$(CC) $(LFLAGS) $(D_LFLAGS) $(USER_LIBS) -o "Debug/OpenCV-Playground-Debug" $(D_OBJS) $(LIBS)
	@echo 'Finished building target: Debug'
	@echo ' '
	
Release: $(R_OBJS)
	@echo 'Building target: Release'
	@echo 'Invoking: GCC C++ Linker'
	$(CC) $(LFLAGS) $(R_LFLAGS) $(USER_LIBS) -o "Release/OpenCV-Playground-Release" $(R_OBJS) $(LIBS)
	@echo 'Finished building target: Release'
	@echo ' '

$(D_OBJS): $(D_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CC) $(INCLUDES) $(CFLAGS) $(D_CFLAGS) -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

$(R_OBJS): $(R_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CC) $(INCLUDES) $(CFLAGS) $(R_CFLAGS) -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
	

# Other Targets
clean:
	-rm -fr $(OBJS) Debug/ Release/
	-mkdir Debug 
	-mkdir Release
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
