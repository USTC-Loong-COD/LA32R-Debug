
#include <iostream>

using namespace std;


#define BITS(a, m, n) (((a) >> (n)) & ((1 << ((m) - (n) + 1)) - 1))

#define SBITS(a, m, n) \
    ((((a) >> (n)) & ((1 << ((m) - (n) + 1)) - 1)) | \
    (((a) >> (m)) & 1 ? (~0 << (m - n + 1)) : 0))


void disasm(char* buf, uint32_t inst) {
    auto opcode = BITS(inst, 6, 0);
    auto funct3 = BITS(inst, 14, 12);
    auto funct7 = BITS(inst, 31, 25);
    const char* rd = ("r" + to_string(BITS(inst, 11, 7))).c_str();
    const char* rs1 = ("r" + to_string(BITS(inst, 19, 15))).c_str();
    const char* sr2 = ("r" + to_string(BITS(inst, 24, 20))).c_str();

    switch(opcode){
        // lui
        case 0x37:
            sprintf(buf, "%7s %3s, %d\t", "lui", rd, BITS(inst, 31, 12) << 12);
            break;
        // auipc
        case 0x17:
            sprintf(buf, "%7s %3s, %d\t", "auipc", rd, BITS(inst, 31, 12) << 12);
            break;
        // jal
        case 0x6f:
            sprintf(buf, "%7s %3s, %d\t", "jal", rd, SBITS(inst, 31, 31) << 20 | BITS(inst, 30, 21) << 1 | BITS(inst, 20, 20) << 11 | BITS(inst, 19, 12) << 12);
            break;
        // jalr
        case 0x67:
            sprintf(buf, "%7s %3s, %3s, %d\t", "jalr", rd, rs1, SBITS(inst, 31, 20));
            break;
        // branch
        case 0x63:
            switch(funct3){
                case 0x0:
                    sprintf(buf, "%7s %3s, %3s, %d\t", "beq", rs1, sr2, SBITS(inst, 31, 31) << 12 | BITS(inst, 30, 25) << 5 | BITS(inst, 11, 8) << 1 | BITS(inst, 7, 7) << 11);
                    break;
                case 0x1:
                    sprintf(buf, "%7s %3s, %3s, %d\t", "bne", rs1, sr2, SBITS(inst, 31, 31) << 12 | BITS(inst, 30, 25) << 5 | BITS(inst, 11, 8) << 1 | BITS(inst, 7, 7) << 11);
                    break;
                case 0x4:
                    sprintf(buf, "%7s %3s, %3s, %d\t", "blt", rs1, sr2, SBITS(inst, 31, 31) << 12 | BITS(inst, 30, 25) << 5 | BITS(inst, 11, 8) << 1 | BITS(inst, 7, 7) << 11);
                    break;
                case 0x5:
                    sprintf(buf, "%7s %3s, %3s, %d\t", "bge", rs1, sr2, SBITS(inst, 31, 31) << 12 | BITS(inst, 30, 25) << 5 | BITS(inst, 11, 8) << 1 | BITS(inst, 7, 7) << 11);
                    break;
                case 0x6:
                    sprintf(buf, "%7s %3s, %3s, %d\t", "bltu", rs1, sr2, SBITS(inst, 31, 31) << 12 | BITS(inst, 30, 25) << 5 | BITS(inst, 11, 8) << 1 | BITS(inst, 7, 7) << 11);
                    break;
                case 0x7:
                    sprintf(buf, "%7s %3s, %3s, %d\t", "bgeu", rs1, sr2, SBITS(inst, 31, 31) << 12 | BITS(inst, 30, 25) << 5 | BITS(inst, 11, 8) << 1 | BITS(inst, 7, 7) << 11);
                    break;
                default: 
                    sprintf(buf, "%32s\t", "unimp");
                    break;
            }
        // load
        case 0x03:
            switch(funct3){
                case 0x0:
                    sprintf(buf, "%7s %3s, %d(%3s)\t", "lb", rd, SBITS(inst, 31, 20), rs1);
                    break;
                case 0x1:
                    sprintf(buf, "%7s %3s, %d(%3s)\t", "lh", rd, SBITS(inst, 31, 20), rs1);
                    break;
                case 0x2:
                    sprintf(buf, "%7s %3s, %d(%3s)\t", "lw", rd, SBITS(inst, 31, 20), rs1);
                    break;
                case 0x4:
                    sprintf(buf, "%7s %3s, %d(%3s)\t", "lbu", rd, SBITS(inst, 31, 20), rs1);
                    break;
                case 0x5:
                    sprintf(buf, "%7s %3s, %d(%3s)\t", "lhu", rd, SBITS(inst, 31, 20), rs1);
                    break;
                default:
                    sprintf(buf, "%32s\t", "unimp");
                    break;
            }
        // store
        case 0x23:
            switch(funct3){
                case 0x0:
                    sprintf(buf, "%7s %3s, %d(%3s)\t", "sb", sr2, SBITS(inst, 31, 25) << 5 | BITS(inst, 11, 7), rs1);
                    break;
                case 0x1:
                    sprintf(buf, "%7s %3s, %d(%3s)\t", "sh", sr2, SBITS(inst, 31, 25) << 5 | BITS(inst, 11, 7), rs1);
                    break;
                case 0x2:
                    sprintf(buf, "%7s %3s, %d(%3s)\t", "sw", sr2, SBITS(inst, 31, 25) << 5 | BITS(inst, 11, 7), rs1);
                    break;
                default:
                    sprintf(buf, "%32s\t", "unimp");
                    break;
            }
        // calculate immidiate
        case 0x13:
            switch (funct3){
                case 0x0:
                    sprintf(buf, "%7s %3s, %3s, %d\t", "addi", rd, rs1, SBITS(inst, 31, 20));
                    break;
                case 0x2:
                    sprintf(buf, "%7s %3s, %3s, %d\t", "slti", rd, rs1, SBITS(inst, 31, 20));
                    break;
                case 0x3:
                    sprintf(buf, "%7s %3s, %3s, %d\t", "sltiu", rd, rs1, SBITS(inst, 31, 20));
                    break;
                case 0x4:
                    sprintf(buf, "%7s %3s, %3s, %d\t", "xori", rd, rs1, SBITS(inst, 31, 20));
                    break;
                case 0x6:
                    sprintf(buf, "%7s %3s, %3s, %d\t", "ori", rd, rs1, SBITS(inst, 31, 20));
                    break;
                case 0x7:
                    sprintf(buf, "%7s %3s, %3s, %d\t", "andi", rd, rs1, SBITS(inst, 31, 20));
                    break;
                case 0x1:
                    sprintf(buf, "%7s %3s, %3s, %d\t", "slli", rd, rs1, BITS(inst, 24, 20));
                    break;
                case 0x5:
                    if(funct7 == 0x0){
                        sprintf(buf, "%7s %3s, %3s, %d\t", "srli", rd, rs1, BITS(inst, 24, 20));
                    }
                    else if(funct7 == 0x20){
                        sprintf(buf, "%7s %3s, %3s, %d\t", "srai", rd, rs1, BITS(inst, 24, 20));
                    }
                    else sprintf(buf, "%32s\t", "unimp");
                    break;
                default:
                    sprintf(buf, "%32s\t", "unimp");
                    break;
            }
        // calculate
        case 0x33:
            switch(funct3){
                case 0x0:
                    if(funct7 == 0x0){
                        sprintf(buf, "%7s %3s, %3s, %3s\t", "add", rd, rs1, sr2);
                    }
                    else if(funct7 == 0x20){
                        sprintf(buf, "%7s %3s, %3s, %3s\t", "sub", rd, rs1, sr2);
                    }
                    else sprintf(buf, "%32s\t", "unimp");
                    break;
                case 0x1:
                    sprintf(buf, "%7s %3s, %3s, %3s\t", "sll", rd, rs1, sr2);
                    break;
                case 0x2:
                    sprintf(buf, "%7s %3s, %3s, %3s\t", "slt", rd, rs1, sr2);
                    break;
                case 0x3:
                    sprintf(buf, "%7s %3s, %3s, %3s\t", "sltu", rd, rs1, sr2);
                    break;
                case 0x4:
                    sprintf(buf, "%7s %3s, %3s, %3s\t", "xor", rd, rs1, sr2);
                    break;
                case 0x5:
                    if(funct7 == 0x0){
                        sprintf(buf, "%7s %3s, %3s, %3s\t", "srl", rd, rs1, sr2);
                    }
                    else if(funct7 == 0x20){
                        sprintf(buf, "%7s %3s, %3s, %3s\t", "sra", rd, rs1, sr2);
                    }
                    else sprintf(buf, "%32s\t", "unimp");
                    break;
                case 0x6:
                    sprintf(buf, "%7s %3s, %3s, %3s\t", "or", rd, rs1, sr2);
                    break;
                case 0x7:
                    sprintf(buf, "%7s %3s, %3s, %3s\t", "and", rd, rs1, sr2);
                    break;
                default:
                    sprintf(buf, "%32s\t", "unimp");
            }
        default:
            sprintf(buf, "%32s\t", "unimp");
            break;
    }

}
// int main(){
//     uint32_t inst;
//     char buf[100];
//     ifstream fin("../../test/test", ios::binary);
//     ofstream four("./result");
//     while(!fin.eof()){
//         fin.read((char*)&inst, sizeof(inst));
//         disasm(buf, inst);
//         four << buf << endl;
//     }
//     return 0;
// }
    