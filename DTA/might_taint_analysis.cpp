/*BEGIN_LEGAL 
Intel Open Source License 

Copyright (c) 2002-2018 Intel Corporation. All rights reserved.
 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */
/*! @file
 *  This file contains an ISA-portable PIN tool for counting dynamic instructions
 */

#include "pin.H"
extern "C" {
#include "xed-interface.h"
}
#include <iostream>
#include <string>
#include <unordered_map>

#define REG_STATE uint64_t
#define TAINT64 0xffffffffffffffff
#define TAINT32 0x00000000ffffffff
#define TAINT16 0x000000000000ffff
#define TAINT8_HIGH 0x00000000ffff0000
#define TAINT8_LOW  0x000000000000ffff

#define DOUBLE 16
#define QWORD 8
#define TAINT_STATE bool
#define TAINT true
#define UNTAINT false

// #define DEBUG

enum FullRegister {
    FULL_REG_NULL = 0,
    FULL_REG_RAX,
    FULL_REG_RBX,
    FULL_REG_RCX,
    FULL_REG_RDX,
    FULL_REG_RSI,
    FULL_REG_RDI,
    FULL_REG_RBP,
    FULL_REG_R8,
    FULL_REG_R9,
    FULL_REG_R10,
    FULL_REG_R11,
    FULL_REG_R12,
    FULL_REG_R13,
    FULL_REG_R14,
    FULL_REG_R15,
    FULL_REG_RSP,
    FULL_REG_RIP,
    FULL_REG_XMM0,
    FULL_REG_XMM1,
    FULL_REG_XMM2,
    FULL_REG_XMM3,
    FULL_REG_XMM4,
    FULL_REG_XMM5,
    FULL_REG_XMM6,
    FULL_REG_XMM7,

    FULL_REG_TOTAL_NUM,
};
/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

// is memory taint :
unordered_map<UINT64, TAINT_STATE> memory_taint_map; // byte-level taint
bool last_instruction_is_syscall_read = false ;
// is reg taint :
TAINT_STATE reg_state[FULL_REG_TOTAL_NUM][DOUBLE] ;
// unordered_set failed .....

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */


/* ===================================================================== */
/* Make the programe Exit Gracefully */
/* ===================================================================== */

VOID graceful_exit() {
    cout << "exiting gracefully " << endl;
    PIN_ExitApplication(0);
}



FullRegister reg_to_full_reg( REG reg )
{
    switch(reg) {
        case REG_RAX:
        case REG_EAX:
        case REG_AX:
        case REG_AH:
        case REG_AL:
            return FULL_REG_RAX;
        case REG_RBX:
        case REG_EBX:
        case REG_BX:
        case REG_BH:
        case REG_BL:
            return FULL_REG_RBX;
        case REG_RCX:
        case REG_ECX:
        case REG_CX:
        case REG_CH:
        case REG_CL:
            return FULL_REG_RCX;
        case REG_RDX:
        case REG_EDX:
        case REG_DX:
        case REG_DH:
        case REG_DL:
            return FULL_REG_RDX;
        case REG_RSI:
        case REG_ESI:
            return FULL_REG_RSI;
        case REG_RDI:
        case REG_EDI:
            return FULL_REG_RDI;
        
        case REG_RBP:
        case REG_EBP:
        case REG_BP:
            return FULL_REG_RBP;
        
        case REG_R8:
        case REG_R8D:
        case REG_R8W:
        case REG_R8B:
            return FULL_REG_R8;
        case REG_R9:
        case REG_R9D:
        case REG_R9W:
        case REG_R9B:
            return FULL_REG_R9;
        case REG_R10:
        case REG_R10D:
        case REG_R10W:
        case REG_R10B:
            return FULL_REG_R10;
        case REG_R11:
        case REG_R11D:
        case REG_R11W:
        case REG_R11B:
            return FULL_REG_R11;
        case REG_R12:
        case REG_R12D:
        case REG_R12W:
        case REG_R12B:
            return FULL_REG_R12;
        case REG_R13:
        case REG_R13D:
        case REG_R13W:
        case REG_R13B:
            return FULL_REG_R13;
        case REG_R14:
        case REG_R14D:
        case REG_R14W:
        case REG_R14B:
            return FULL_REG_R14;
        case REG_R15:
        case REG_R15D:
        case REG_R15W:
        case REG_R15B:
            return FULL_REG_R15;
        case REG_RSP:
        case REG_ESP:
            return FULL_REG_RSP;
        case REG_RIP:
        case REG_EIP:
            return FULL_REG_RIP;

        case REG_XMM0:
            return FULL_REG_XMM0;
        case REG_XMM1:
            return FULL_REG_XMM1;
        case REG_XMM2:
            return FULL_REG_XMM2;
        case REG_XMM3:
            return FULL_REG_XMM3;
        case REG_XMM4:
            return FULL_REG_XMM4;
        case REG_XMM5:
            return FULL_REG_XMM5;
        case REG_XMM6:
            return FULL_REG_XMM6;
        case REG_XMM7:
            return FULL_REG_XMM7;
        
        default:
            return FULL_REG_NULL;
    }
}



/* ===================================================================== */
VOID check_rip(void)
{
    for ( USIZE i = 0 ; i < QWORD ; ++i ) {
        if ( reg_state[FULL_REG_RIP][i] ) {
            cout << endl;
            cout << "Dreadful Error : " << endl;
            cout << "   Rip is hijacked !" << endl;
            graceful_exit();
        }
    }
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    cerr <<
        "This tool prints out the number of dynamic instructions executed to stderr.\n"
        "\n";

    cerr << KNOB_BASE::StringKnobSummary();

    cerr << endl;

    return -1;
}

/* ===================================================================== */
VOID mem2mem(ADDRINT pc, VOID *ea, USIZE size, UINT64 reg, CONTEXT *ctx)
{
    REG src_deref_reg = REG_RCX;
    REG dst_deref_reg = (REG) reg ;
    UINT64 src_addr, dst_addr;
    
    PIN_GetContextRegval(ctx, src_deref_reg, reinterpret_cast<UINT8*>(&src_addr));
    PIN_GetContextRegval(ctx, dst_deref_reg, reinterpret_cast<UINT8*>(&dst_addr));
    for ( USIZE i = 0 ; i < size ; ++i ) {
        if ( memory_taint_map[src_addr + i] ) {
            memory_taint_map[dst_addr + i] = TAINT ;
        }
    }
}
/* ===================================================================== */

VOID register_will_be_written(ADDRINT pc, VOID *ea, USIZE size, UINT64 reg) 
{
    if ( size > DOUBLE ) {
        return ;
    }
    FullRegister full_reg = reg_to_full_reg((REG) reg) ;
    for ( USIZE i = 0 ; i < size ; ++ i ) {  
        reg_state[full_reg][i] = memory_taint_map[(UINT64) ea + i] ;
    }
    if ( full_reg == FULL_REG_RIP ) {
        check_rip(); // may not return
    }
}

/* ===================================================================== */
VOID memory_will_be_written(ADDRINT pc, VOID *ea, USIZE size, UINT64 reg) 
{
    if ( size > DOUBLE ) {
        return ;
    }
    FullRegister full_reg = reg_to_full_reg( (REG) reg) ;
    for ( USIZE i = 0 ; i < size ; ++i ) {
        memory_taint_map[(UINT64) ea + i] = reg_state[full_reg][i] ;
    }
    
}
/* ===================================================================== */


VOID detect_read(CONTEXT* ctx)
{
    // string dummy = "in detect_read" ;
    UINT64 syscall_no, fd, buf, size ;
    // read(fd, buf, size)
    
    // cout << "in detect_read" << endl;
    PIN_GetContextRegval(ctx, REG_RAX, reinterpret_cast<UINT8*>(&syscall_no));
    PIN_GetContextRegval(ctx, REG_RDI, reinterpret_cast<UINT8*>(&fd));
    PIN_GetContextRegval(ctx, REG_RSI, reinterpret_cast<UINT8*>(&buf));
    PIN_GetContextRegval(ctx, REG_RDX, reinterpret_cast<UINT8*>(&size));
    
    if (syscall_no == 0 && fd == 0 ) {
        
        // this is a read(...) from stdin
        cout << " read( " << fd << ", " << hex << buf << ", " << size << " ) " << endl ; ;
        
        // last_instruction_is_syscall_read = true ;
        
        // Taint!
        for ( UINT64 i = 0 ; i < size ; ++i ) {
            memory_taint_map[buf + i] = TAINT ; // true for tainted !!
        }
    }
}

/* ===================================================================== */

VOID decode_instruction(INS ins, string s) 
{
    char buf[2048];
    xed_decoded_inst_t* xedd = INS_XedDec(ins);
    ADDRINT addr = INS_Address(ins);
    xed_format_context(XED_SYNTAX_ATT, xedd, buf, 2048, static_cast<UINT64>(addr), 0, 0);
    cout << buf << " " << s <<  endl;
}
/* ===================================================================== */


VOID Instruction(INS ins, VOID *v)
{
    //   I : Pool 
    if ( INS_IsSyscall(ins) ) {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) detect_read, IARG_CONTEXT, IARG_END);
    } 

    //  II : Propogate
    if ( INS_MemoryOperandIsRead(ins, 0) && INS_IsMemoryRead(ins) ) {
        // MemRead, RegWrite
        REG assigned_reg = INS_RegW(ins, 0);
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) register_will_be_written, IARG_INST_PTR, IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE, IARG_UINT64, (UINT64) assigned_reg, IARG_END);
    } 
    if ( INS_MemoryOperandIsWritten(ins, 0) && INS_IsMemoryWrite(ins) ) {
        // MemWrite, RegRead
        REG pulled_reg ;
        if ( INS_Size(ins) <= 2 ) {
            pulled_reg = INS_RegR(ins, 0) ;
        } else {
            pulled_reg = INS_RegR(ins, INS_MaxNumRRegs(ins) - 1);
        }
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) memory_will_be_written, IARG_INST_PTR, IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE, IARG_UINT64, (UINT64) pulled_reg, IARG_END);
    }
    
    if ( INS_HasRealRep(ins) && INS_IsMemoryWrite(ins) ) {
        // rep stos 
        REG dst_deref_reg = INS_RegR(ins, 0);
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) mem2mem, IARG_INST_PTR, IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE, IARG_UINT64, (UINT64) dst_deref_reg, IARG_CONTEXT, IARG_END);


    }
    // III : Check
}

/* ===================================================================== */

VOID Fini(INT32 code, VOID *v)
{
    for ( auto it = memory_taint_map.begin(); it != memory_taint_map.end(); ++it ) {
        if ( it->second ) {
            cout << hex << it->first << " is Tainted !!\n" ;
        }
    }
    
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char *argv[])
{
    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }
    for ( int i = 0; i < FULL_REG_TOTAL_NUM; ++i ) {
        for ( USIZE j = 0 ; j < QWORD ; ++j ) {
            reg_state[i][j] = false ;
        }
    }

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
