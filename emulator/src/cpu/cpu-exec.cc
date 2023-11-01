#include <common.h>
#include <paddr.h>
#include <cpu.h>
#include <disasm.h>
#include <device.h>
#include <difftest.h>
#include <statistic.h>
// #define DUMP_WAVE
#ifdef ITRACE
typedef struct{
    uint32_t pc;
    uint32_t inst;
    uint32_t rf_wdata;
}inst_log_t;

#define ILOG_SIZE 16
static inst_log_t ilog[ILOG_SIZE];
static int ilog_idx = 0;

void print_ilog(){
    std::cout << std::left << std::setw(16) << std::setfill(' ') << "PC" ;
    std::cout << std::left << std::setw(46) << std::setfill(' ') << "INSTRUCTION" << '\t' << "RF_WDATA" << std::endl;
    std::cout << std::left << std::setw(16+46+10) << std::setfill('-') << "" << std::endl;
    for(int i = 0; i < ILOG_SIZE; i++){
        std::cout << std::hex << std::setw(8) << std::setfill('0') << ilog[(ilog_idx+i)%ILOG_SIZE].pc << ": \t";
        char buf[100];
        disasm(buf, ilog[(ilog_idx+i)%ILOG_SIZE].inst);
        std::cout << std::left << std::setw(46) << std::setfill(' ') << buf << '\t';
        std::cout << std::right << std::hex << std::setw(8) << std::setfill('0') << ilog[(ilog_idx+i)%ILOG_SIZE].rf_wdata << std::endl;
    }
}
void add_ilog(VCPU* dut){
    ilog[ilog_idx].pc = dut->pc_WB;
    ilog[ilog_idx].inst = dut->inst_WB;
    ilog[ilog_idx].rf_wdata = dut->rf_wdata_WB;
    ilog_idx = (ilog_idx + 1) % ILOG_SIZE;
}
#endif

static statistic stat;

inline bool test_break(uint32_t inst){
    return inst == 0x002a0000;
}

void set_cpu_state(VCPU* dut){
    if(dut->rf_we_WB){
        cpu.reg[dut->rd_WB] = dut->rf_wdata_WB;
    }
    cpu.pc = dut->pc_WB;
}
bool commit_update(VCPU *dut){
    if(dut->commit_WB){
        #ifdef ITRACE
            if(cpu.state == SIM_RUNNING) add_ilog(dut);
        #endif
        set_cpu_state(dut);

        if(test_break(dut->inst_WB)){
            cpu.state = SIM_END;
            cpu.halt_pc = dut->pc_WB;
        }
        return true;
    }
    return false;
}

void single_cycle(){
    dut->clk = 0;
    dut->inst_IF = paddr_read(dut->pc_IF, 4);
    dut->eval();
    if(dut->is_store_MEM){
        paddr_write(dut->mem_addr_MEM, dut->mem_wdata_MEM, 1 << (dut->mem_size_MEM));
    }
    if(dut->is_load_MEM){
        dut->mem_rdata_MEM = paddr_read(dut->mem_addr_MEM, 1 << (dut->mem_size_MEM));
    }
    dut->clk = 1;
    dut->eval();
#ifdef DUMP_WAVE
    sim_time++;
        m_trace->dump(sim_time);
#endif
}
/* reset PC to RESET_VECTOR */
void reset(){
    cpu.state = SIM_STOP;
    cpu.pc = RESET_VECTOR;
    dut->rstn = 0;
    dut->clk = 0;
    dut->eval();
    dut->clk = 1;
    dut->eval();
    dut->rstn = 0;
}
void cpu_exec(uint64_t n){
    switch(cpu.state){
        case SIM_ABORT: case SIM_END:
        std::cout << "Program execution has ended." << std::endl;
        return;
        default: cpu.state = SIM_RUNNING;
    }
    while(n--){
        commit_update(dut);
        if(cpu.state != SIM_RUNNING) break;
        single_cycle();
    }
#ifdef ITRACE
    print_ilog();
#endif
    switch(cpu.state){
        case SIM_END: 
        Log("simulation %s at pc = " FMT_WORD, (cpu.reg[4] == 0 ? INLINE_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) : 
            INLINE_FMT("HIT BAD TRAP", ANSI_FG_RED)), cpu.halt_pc); break;
        case SIM_ABORT:
        Log("simulation %s at pc = " FMT_WORD, INLINE_FMT("ABORT", ANSI_FG_RED), cpu.halt_pc); break;
        case SIM_STOP:
        Log("simulation %s at pc = " FMT_WORD, INLINE_FMT("STOP", ANSI_FG_YELLOW), cpu.halt_pc); break;
        break;
    }
}

// void init_statistic(const char* name){
//     std::string sname(name);
//     auto start = sname.find_last_of('/');
//     auto end = sname.find_first_of('.');
//     stat.set_name(sname.substr(start + 1, end - start - 1).c_str());
// }

