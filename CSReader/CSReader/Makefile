##############################################################################################
#
#       !!!! Do NOT edit this makefile with an editor which replace tabs by spaces !!!!    
#
##############################################################################################
# 
# On command line:
#
# make  = Create project
# make clean = Clean project files.
# To rebuild project do "make clean" and "make ".
# VERSION:1.0.0
#
#list all the cross compile
#CROSS			=arm-poky-linux-gnueabi-gcc--sysroot=/opt/fsl-imx-x11/4.1.15-2.0.0/sysroots/cortexa7hf-neon-poky-linux-gnueabi/usr/include-
#CROSS			= /opt/fsl-imx-x11/4.1.15-2.0.0/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-

# CC				= $(CROSS)gcc
# CXX				= $(CROSS)g++
# AR				= $(CROSS)ar

#CC  		   := $(CC)
#LD  		   := $(LD)
#AR  		   := $(AR)
#CXX		   := $(CXX)
#CPP		   := $(CPP)
#AS		   := $(AS)

#RANLIB			= $(CROSS)ranlib
# CXXFLAGS		= -O1 -s --sysroot=/opt/fsl-imx-x11/4.1.15-2.0.0/sysroots/cortexa7hf-neon-poky-linux-gnueabi/ -march=armv7ve -mfpu=neon  -mfloat-abi=hard -mcpu=cortex-a7 
CXXFLAGS		= -O1 -s 
#LDFLAGS			= api/libgsserial.a	 #
#target out put bin name
APP_NAME		= CSReader
SDK_DIR			= /opt/fsl-imx-x11/4.1.15-2.0.0/sysroots/x86_64-pokysdk-linux/usr/lib/arm-poky-linux-gnueabi/gcc/arm-poky-linux-gnueabi/5.3.0/include/
#list incluce head files directory
SDK_INC_DIR 	= -I so_h  -I software
#list include libs directory
sdkExtendLibDir	= so

# List cpp source files here 
SRC_PATH		= software


#expand cpp source files 
SRC-y 			:= $(wildcard api/*.cpp) $(wildcard link/*.cpp) $(wildcard main.cpp) $(wildcard json/*.cpp) $(wildcard ble/*.cpp)

#define object files ,no need modify					
OBJDIR			= Release
OBJS			= $(patsubst %.cpp, $(OBJDIR)/%.o, $(SRC-y))
#define depend files

DEPS			=$(OBJS:%.o=%.d)
 
# List all user libraries here
ULIBS			= -lpthread
#

MYDLLS 			+= -L $(sdkExtendLibDir)

SDK_LIB 		= $(MYDLLS) $(ULIBS)




	
##############################################################################################
# makefile rules
##############################################################################################	
$(APP_NAME):	$(OBJS)
	@echo ""
	@echo "+---------------------------------------------------"
	@echo "|"
	@echo "|   building target: $(APP_NAME)"
	@echo "|"
	@echo "+---------------------------------------------------"
	$(CXX)  $(OBJS) $(LDFLAGS) -o $(APP_NAME) $(SDK_INC_DIR) $(SDK_LIB)
	@echo ""
	@echo "+---------------------------------------------------"
	@echo "|"
	@echo "|   Finished building target: $(APP_NAME)"
	@echo "|"
	@echo "+---------------------------------------------------"

$(OBJS):	$(OBJDIR)/%.o : %.cpp
	$(CXX) -c $(SDK_INC_DIR) $(CXXFLAGS)  $< -o $@
	
$(DEPS):	$(OBJDIR)/%.d :	%.cpp #
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)
	@set -e; rm -f $@; \
	$(CXX) -MM  $<  $(SDK_INC_DIR) > $@.$$$$; \
	sed 's\[a-zA-Z0-9_-]*.o:\$(OBJDIR)/$*.o:\g' < $@.$$$$ > $@;\
	rm -f $@.$$$$

-include $(DEPS)#

.PHONY: clean
clean:
	@rm -vf $(APP_NAME)  *~ *.gdb $(OBJS) $(DEPS)

