# Modify from here --------------------------------------------------------------------------------------

# Compiler
CC = arm-linux-gnueabihf-g++

# Sources Directory
SOURCEDIR = src/
# Binaries Directory
BINDIR = bin/

# Common Flags:
INCLUDES = -I/usr/local/include/opencv2 -I/usr/local/include/CL -I/usr/local/include/
CFLAGS = -Wextra
LFLAGS = 
LIBS = -lopencv_core -lOpenCL -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann -lmali
USER_LIBS = -L/usr/local/lib -L/usr/lib/arm-linux-gnueabihf -L/usr/lib/arm-linux-gnueabihf/mali-egl

# Debug Options
D_BINNAME = CV-Playground-Debug
D_OBJDIR = Debug
D_CFLAGS = -g3 -rdynamic
D_LFLAGS = 
D_LIBS =

# Release Options
R_BINNAME = CV-Playground-Release
R_OBJDIR = Release
R_CFLAGS = -O3
R_LFLAGS = 
R_LIBS =

# Modify to here --------------------------------------------------------------------------------------

# Sources
D_SRCS = $(shell find $(SOURCEDIR) -name '*.cpp')
R_SRCS = $(shell find $(SOURCEDIR) -name '*.cpp')
OBJS = $(shell sh -c 'cd $(SOURCEDIR);ls | grep .cpp | sed s/cpp/o/g')
D_OBJS = $(D_OBJDIR)/$(OBJS)
R_OBJS = $(R_OBJDIR)/$(OBJS)

.PHONY: all Debug Release

# All Target
all: Debug Release
	@echo '=== Finished Building target: $@ ==='

# Pre-Building
BuildDirs:
	mkdir -p $(BINDIR)
	mkdir -p $(D_OBJDIR)
	mkdir -p $(R_OBJDIR)

# Tool invocations
Debug: BuildDirs $(D_OBJS)
	@echo 'Building target: Debug'
	@echo 'Invoking: GCC C++ Linker'
	$(CC) $(LFLAGS) $(D_LFLAGS) $(USER_LIBS) -o "$(BINDIR)/$(D_BINNAME)" $(D_OBJS) $(LIBS)
	@echo 'Finished building target: Debug'
	@echo ' '
	
Release: BuildDirs $(R_OBJS)
	@echo 'Building target: Release'
	@echo 'Invoking: GCC C++ Linker'
	$(CC) $(LFLAGS) $(R_LFLAGS) $(USER_LIBS) -o "$(BINDIR)/$(R_BINNAME)" $(R_OBJS) $(LIBS)
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
	-rm -fr $(BINDIR) $(D_OBJDIR) $(R_OBJDIR)
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
