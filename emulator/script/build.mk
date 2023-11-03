

SIMU_DIR = $(abspath $(WORK_DIR)/../simulator)
SIMUISO = $(SIMU_DIR)/build/simulator-so
CSRCS := $(shell find src/ -name "*.cc")

VERILOG := $(shell find $(VERILOG_DIR) -name "*.v")

all: $(BINARY) 

$(SIMUISO):
	@echo "$(COLOR_YELLOW)[Make DIFF]$(COLOR_NONE) $(notdir $(SIMU_DIR))/build/simulator-so"
	@$(MAKE) -s -C $(SIMU_DIR)

$(BINARY): $(CSRCS) $(VERILOG)
	@echo "$(COLOR_DBLUE)[VERILATE]$(COLOR_NONE) $(notdir $(BUILD_DIR))/VCPU"
	@mkdir -p $(BUILD_DIR)
	@verilator $(VFLAGS) $(CSRCS) $(CINC_PATH) $(VERILOG_TOP)
	@echo "$(COLOR_DBLUE)[MAKE]$(COLOR_NONE) $(notdir $(BUILD_DIR))/VCPU"
	@$(MAKE) -s -C $(BUILD_DIR) -f $(REWRITE) -j8


ARGS = 
run: $(BINARY) $(SIMUISO)
	@mkdir -p ./reports
	@echo "$(COLOR_YELLOW)[RUN]$(COLOR_NONE) build/$(notdir $<)"
	@$(BINARY) $(IMG) $(SIMUISO) $(ARGS)

gdb: $(BINARY)
	@echo "$(COLOR_YELLOW)[GDB]$(COLOR_NONE) build/$(notdir $<)"
	gdb --args  $(BINARY) $(IMG) $(SIMUISO) $(ARGS)

clean:
	rm -rf $(BUILD_DIR)

clean-report:
	rm -rf reports

