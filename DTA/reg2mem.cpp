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
#define TAINT true
#define UNTAINT false

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

UINT64 ins_count = 0;
UINT64 saved_rip_reference = 0;
unordered_map<UINT64, bool> memory_taint_map; // byte-level taint

// unordered_set failed .....
/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */


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
VOID taint_check(CONTEXT* ctx)
{
    UINT64 rsp ;
    PIN_GetContextRegval(ctx, REG_RSP, reinterpret_cast<UINT8*>(&rsp));

    for ( size_t i = 0 ; i < sizeof(rsp) ; ++i ) {
        if ( memory_taint_map[rsp + i] ) {
            cout << "Overflow detected !!! " << endl;
            cout << "Raising the flag !!!! " << endl;
            return ;
        }
    }


}
/* ===================================================================== */
/* ===================================================================== */
VOID memory_will_be_written(ADDRINT pc, VOID *ea, USIZE size, UINT64 reg) 
{
    static const xed_state_t dstate = {
        XED_MACHINE_MODE_LONG_64, XED_ADDRESS_WIDTH_64b};
    REG pulled_reg = (REG) reg ;
    char buf[2048];
    xed_decoded_inst_t xedd;
    xed_decoded_inst_zero_set_mode(&xedd, &dstate);
    xed_uint64_t runtime_address = static_cast<xed_uint64_t>(pc);
    xed_error_enum_t xed_code = xed_decode(&xedd, reinterpret_cast<UINT8*>(pc), 15);
    BOOL xed_ok = ( xed_code == XED_ERROR_NONE );
    if ( xed_ok ) {
        xed_format_context(XED_SYNTAX_ATT, &xedd, buf, 2048, runtime_address, 0, 0);
        cout << buf << " read from register : " << REG_StringShort(pulled_reg) << endl;
        cout << "effective address is " << hex << ea << " with size == " << size << endl << endl;
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
VOID get_saved_rip_reference(CONTEXT* ctx)
{
     
    PIN_GetContextRegval(ctx, REG_RSP, reinterpret_cast<UINT8*>(&saved_rip_reference));


}

/* ===================================================================== */

VOID Instruction(INS ins, VOID *v)
{

    //  II : Propagate
    if ( INS_MemoryOperandIsWritten(ins, 0) ) {
        if ( INS_IsMemoryWrite(ins) ) {
            
            
            REG pulled_reg ;
            if ( INS_Size(ins) <= 2 ) {
                pulled_reg = INS_RegR(ins, 0);
            } else {
                pulled_reg = INS_RegR(ins, INS_MaxNumRRegs(ins)-1);
            }
            // decode_instruction(ins, " >> Is Memory Write " );
            // cout << "       read from register " << REG_StringShort(pulled_reg) << endl;
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) memory_will_be_written, IARG_INST_PTR, IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE, IARG_UINT64, (UINT64) pulled_reg, IARG_END);
        } else {
            decode_instruction(ins, " true negative !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        }
    } else {
        if ( INS_IsMemoryWrite(ins) ) {
            decode_instruction(ins, "false negative ????????????????????????");
        }
    }

}

/* ===================================================================== */

VOID Fini(INT32 code, VOID *v)
{
    cerr << "Syscall Count " << ins_count  << endl;
    cerr << "saved_rip_reference 0x" << hex << saved_rip_reference << endl; 
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
    

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
