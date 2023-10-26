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
    uint32_t prd;
    uint32_t prj;
    uint32_t prk;
}inst_log_t;

uint32_t reg_rename_table[32];
static statistic stat;


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
        disasm(buf, ilog[(ilog_idx+i)%ILOG_SIZE].inst, ilog[(ilog_idx+i)%ILOG_SIZE].prd, ilog[(ilog_idx+i)%ILOG_SIZE].prj, ilog[(ilog_idx+i)%ILOG_SIZE].prk);
        std::cout << std::left << std::setw(46) << std::setfill(' ') << buf << '\t';
        std::cout << std::right << std::hex << std::setw(8) << std::setfill('0') << ilog[(ilog_idx+i)%ILOG_SIZE].rf_wdata << std::endl;
    }
}
void add_ilog(uint32_t pc, uint32_t inst, uint32_t rf_wdata, uint32_t prd, uint32_t rd, bool rd_valid, uint32_t rj, uint32_t rk){
    ilog[ilog_idx].pc = pc;
    ilog[ilog_idx].inst = inst;
    ilog[ilog_idx].rf_wdata = rf_wdata;
    ilog[ilog_idx].prj = reg_rename_table[rj];
    ilog[ilog_idx].prk = reg_rename_table[rk];
    if(rd_valid){
        reg_rename_table[rd] = prd;
    }
    ilog[ilog_idx].prd = prd;

    ilog_idx = (ilog_idx + 1) % ILOG_SIZE;
}
#endif

inline bool test_break(uint32_t inst){
    return inst == 0x002a0000;
}

void set_cpu_state(uint32_t pc, uint32_t rd, bool rd_valid, uint32_t rf_wdata){
    if(rd_valid){
        cpu.reg[rd] = rf_wdata;
    }
    cpu.pc = pc;
}
bool commit_update(bool commit_en, uint32_t pc, uint32_t rd, bool rd_valid, uint32_t rf_wdata, uint32_t prd){
    if(commit_en){
        auto inst = paddr_read(cpu.pc, 4);
        #ifdef ITRACE
            if(cpu.state == SIM_RUNNING) add_ilog(cpu.pc, inst, rf_wdata, prd, BITS(inst, 4, 0), rd_valid, BITS(inst, 9, 5), BITS(inst, 14, 10));
        #endif
        set_cpu_state(pc, rd, rd_valid, rf_wdata);

        if(test_break(inst)){
            cpu.state = SIM_END;
            cpu.halt_pc = pc;
        }
        return true;
    }
    return false;
}

void single_cycle(){
    dut->clock = 0;
    dut->io_inst1_IF = paddr_read(dut->io_pc_IF, 4);
    dut->io_inst2_IF = paddr_read(dut->io_pc_IF + 4, 4);
    dut->io_inst3_IF = paddr_read(dut->io_pc_IF + 8, 4);
    dut->io_inst4_IF = paddr_read(dut->io_pc_IF + 12, 4);
    dut->eval();
    if(dut->io_mem_is_store_cmt){
        paddr_write(uint32_t(dut->io_mem_waddr_cmt), dut->io_mem_wdata_cmt, 1 << ((dut->io_mem_wlen_cmt) % 4));
    }
    if(dut->io_mem_is_load_ex){
        dut->io_mem_rdata_ex = paddr_read(uint32_t(dut->io_mem_raddr_ex), 1 << ((dut->io_mem_rlen_ex) % 4));
    }

    dut->clock = 1;
    dut->eval();
#ifdef DUMP_WAVE
    sim_time++;
        m_trace->dump(sim_time);
#endif
}

void reset(){
    cpu.state = SIM_STOP;
    cpu.pc = RESET_VECTOR;
    dut->reset = 1;
    dut->clock = 0;
    dut->eval();
    dut->clock = 1;
    dut->eval();
    dut->reset = 0;
    std::cout << "Reset at pc = " << std::hex << dut->io_pc_IF << std::endl;
}
void cpu_exec(uint64_t n){
#ifndef CONFIG_REF
    switch(cpu.state){
        case SIM_ABORT: case SIM_END:
        std::cout << "Program execution has ended." << std::endl;
        return;
        default: cpu.state = SIM_RUNNING;
    }
#endif
    while(n--){
        uint32_t commit_num = 0;
        commit_num += commit_update(dut->io_commit_en1, dut->io_commit_pc_1, dut->io_commit_rd1, dut->io_commit_rd_valid1, dut->io_commit_rf_wdata1, dut->io_commit_prd1);
        commit_num += commit_update(dut->io_commit_en2, dut->io_commit_pc_2, dut->io_commit_rd2, dut->io_commit_rd_valid2, dut->io_commit_rf_wdata2, dut->io_commit_prd2);
        commit_num += commit_update(dut->io_commit_en3, dut->io_commit_pc_3, dut->io_commit_rd3, dut->io_commit_rd_valid3, dut->io_commit_rf_wdata3, dut->io_commit_prd3);
        commit_num += commit_update(dut->io_commit_en4, dut->io_commit_pc_4, dut->io_commit_rd4, dut->io_commit_rd_valid4, dut->io_commit_rf_wdata4, dut->io_commit_prd4);
        if(cpu.state != SIM_RUNNING) break;
#ifdef DIFFTEST
        if(dut->io_commit_is_ucread1 || dut->io_commit_is_ucread2 || dut->io_commit_is_ucread3 || dut->io_commit_is_ucread4){
            difftest_sync();
        }
        else if(commit_num != 0) difftest_step(commit_num);
#endif
        stat.ipc_update(commit_num);
        stat.mul_commit_update(commit_num);
        stat.predict_update(dut);
        stat.stall_update(dut);
        single_cycle();
    }
#ifndef CONFIG_REF
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
    stat.print_stat();
    stat.generate_markdown_report();
#endif
}

void init_statistic(const char* name){
    std::string sname(name);
    auto start = sname.find_last_of('/');
    auto end = sname.find_first_of('.');
    stat.set_name(sname.substr(start + 1, end - start - 1).c_str());
}

