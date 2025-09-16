#include <machine.hpp>
#include <math.h>
#include <iostream>

void execute(doytVM machine){
    while (machine.i_inx < machine.ins.size()){
        auto instr = machine.current();

        switch (instr.code){
            case Opcode::ADD: { machine.reg[instr.c] = machine.reg[instr.a] + machine.reg[instr.b]; break; } 
            case Opcode::SUB: { machine.reg[instr.c] = machine.reg[instr.a] - machine.reg[instr.b]; break; }
            case Opcode::MUL: { machine.reg[instr.c] = machine.reg[instr.a] * machine.reg[instr.b]; break; }
            case Opcode::DIV: { machine.reg[instr.c] = machine.reg[instr.a] / machine.reg[instr.b]; break; }
            case Opcode::MOD: { machine.reg[instr.c] = (int)machine.reg[instr.a] % (int)machine.reg[instr.b]; break; }

            case Opcode::SQRT:{ machine.reg[instr.c] = std::sqrt(machine.reg[instr.a]); break; }
            case Opcode::EXP: { machine.reg[instr.c] = std::exp (machine.reg[instr.a]); break; }
            case Opcode::NEG: { machine.reg[instr.c] = -machine.reg[instr.a]; break; }
            case Opcode::LN:  { machine.reg[instr.c] = std::log(machine.reg[instr.a]); break; }

            case Opcode::sADD:{ machine.reg[instr.c] += machine.reg[instr.a]; break; }
            case Opcode::sSUB:{ machine.reg[instr.c] -= machine.reg[instr.a]; break; }
            case Opcode::sMUL:{ machine.reg[instr.c] *= machine.reg[instr.a]; break; }
            case Opcode::sDIV:{ machine.reg[instr.c] /= machine.reg[instr.a]; break; }

            case Opcode::PRINT:{ std::cout << machine.reg[instr.a]; }
        };

        machine.advance();
    };
};

