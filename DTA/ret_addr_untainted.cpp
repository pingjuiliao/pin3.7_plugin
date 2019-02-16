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
#include <iostream>
#include <string>
#include <unordered_map>
#define MAP_SIZE

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

UINT64 ins_count = 0;
UINT64 saved_rip_reference = 0;
unordered_map<UINT64, bool> taint_map; // byte-level taint
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
        if ( taint_map[rsp + i] ) {
            cout << "Overflow detected !!! " << endl;
            cout << "Raising the flag !!!! " << endl;
            return ;
        }
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
        cout << " read( " << fd << ", " << buf << ", " << size << " ) ;" << endl ;
        for ( UINT64 i = 0 ; i < size ; ++i ) {
            // buf + i == address 
            taint_map[buf + i] = true ; // true for tainted !!
        }
    }
    ins_count++;
}

/* ===================================================================== */
VOID get_saved_rip_reference(CONTEXT* ctx)
{
     
    PIN_GetContextRegval(ctx, REG_RSP, reinterpret_cast<UINT8*>(&saved_rip_reference));


}

/* ===================================================================== */

VOID Instruction(INS ins, VOID *v)
{
    
    if ( INS_IsSyscall(ins) ) {
        // size_t val ;
        // PIN_GetContextRegval((CONTEXT *) IARG_CONTEXT, REG_RAX, (uint8_t *) &val);
        // cout << "raxval is " << val << endl ; 
        // cout << "is syscall" << endl;
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)detect_read, IARG_CONTEXT, IARG_END);
    }
    if ( INS_IsRet(ins) ) {
    
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) taint_check, IARG_CONTEXT, IARG_END);
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
