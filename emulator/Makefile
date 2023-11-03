# .DEFUALT_GOAL = vcpu

WORK_DIR 			= .
VERILOG_DIR 		= $(WORK_DIR)/verilog
CC_DIR 				= $(WORK_DIR)/src
BUILD_DIR 			= $(WORK_DIR)/build
TAR_DIR 			= $(BUILD_DIR)/obj

VERILOG_TOP 		= $(VERILOG_DIR)/CPU.v
VFLAGS 				= --trace --cc --exe -O3 -I$(VERILOG_DIR) -Mdir $(BUILD_DIR) --no-MMD
VFLAGS 				+= -Wno-UNOPTFLAT 
CINC_PATH 			= -CFLAGS -I$(abspath $(WORK_DIR))/include

REWRITE 			= $(abspath $(WORK_DIR))/script/rewrite.mk


CSRCS =  $(shell find $(CC_DIR) -name "*.cc")
VSRCS = $(shell find $(VERILOG_DIR) -name "*.v")
BINARY = $(BUILD_DIR)/VCPU

IMG = 


COLOR_RED   		= \033[31m
COLOR_GREEN 		= \033[32m
COLOR_YELLOW 		= \033[33m
COLOR_BLUE  		= \033[34m
COLOR_PURPLE 		= \033[35m
COLOR_DBLUE 		= \033[36m
COLOR_NONE  		= \033[0m

include ./script/build.mk