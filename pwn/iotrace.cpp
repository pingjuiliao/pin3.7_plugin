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


VOID decode_syscall(CONTEXT* ctx)
{
    UINT64 syscall_no, a1, a2, a3, rip ; 
    PIN_GetContextRegval(ctx, REG_RIP, reinterpret_cast<UINT8*>(&rip));
    PIN_GetContextRegval(ctx, REG_RAX, reinterpret_cast<UINT8*>(&syscall_no));
    PIN_GetContextRegval(ctx, REG_RDI, reinterpret_cast<UINT8*>(&a1));
    PIN_GetContextRegval(ctx, REG_RSI, reinterpret_cast<UINT8*>(&a2));
    PIN_GetContextRegval(ctx, REG_RDX, reinterpret_cast<UINT8*>(&a3));
    
    switch (syscall_no) {
        case 0:
            cout << "At Rip : 0x" << hex << rip << endl ; 
            cout << "read( fd: " << hex << a1 ;
            cout << ", buf: " << hex << a2 ;
            cout << ", size: "<< hex << a3 << " ) ;" << endl;
            break ;
        case 1:
            cout << "write( fd: " << hex << a1 ;
            cout << ", buf: " << hex << a2 ;
            cout << ", size: "<< hex << a3 << " ) ;" << endl;
            break ;
       default:
            break ;
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
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) decode_syscall, IARG_CONTEXT, IARG_END);
    } 

}

/* ===================================================================== */

VOID Fini(INT32 code, VOID *v)
{
#ifdef DEBUG
    for ( auto it = memory_taint_map.begin(); it != memory_taint_map.end(); ++it ) {
        if ( it->second ) {
            cout << hex << it->first << " is Tainted !!\n" ;
        }
    }
#endif
    
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
