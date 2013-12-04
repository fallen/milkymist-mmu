`ifdef LM32_CONFIG_V
`else
`define LM32_CONFIG_V

`ifdef RESCUE
`define CFG_EBA_RESET 32'h00220000
`define CFG_DEBA_RESET 32'h10000000
`else
`define CFG_EBA_RESET 32'h00860000
`define CFG_DEBA_RESET 32'h10000000
`endif

`define CFG_PL_MULTIPLY_ENABLED
`define CFG_PL_BARREL_SHIFT_ENABLED
`define CFG_SIGN_EXTEND_ENABLED
`define CFG_MC_DIVIDE_ENABLED
`define CFG_EBR_POSEDGE_REGISTER_FILE

`define CFG_ICACHE_ENABLED
`define CFG_ICACHE_ASSOCIATIVITY   1
`define CFG_ICACHE_SETS            256
`define CFG_ICACHE_BYTES_PER_LINE  16
`define CFG_ICACHE_BASE_ADDRESS    32'h0
`define CFG_ICACHE_LIMIT           32'h7fffffff

`define CFG_DCACHE_ENABLED
`define CFG_DCACHE_ASSOCIATIVITY   1
`define CFG_DCACHE_SETS            256
`define CFG_DCACHE_BYTES_PER_LINE  16
`define CFG_DCACHE_BASE_ADDRESS    32'h0
`define CFG_DCACHE_LIMIT           32'h7fffffff

// Enable Debugging
//`define CFG_JTAG_ENABLED
//`define CFG_JTAG_UART_ENABLED
`define CFG_DEBUG_ENABLED
//`define CFG_HW_DEBUG_ENABLED
`define CFG_ROM_DEBUG_ENABLED
`define CFG_BREAKPOINTS 32'h4
`define CFG_WATCHPOINTS 32'h4
`define CFG_EXTERNAL_BREAK_ENABLED
`define CFG_GDBSTUB_ENABLED

function integer clog2;
  input integer value;
  begin
    value = value - 1;
    for (clog2 = 0; value > 0; clog2 = clog2 + 1)
      value = value >> 1;
  end
endfunction

`define CLOG2 clog2

//
// MEMORY MANAGEMENT UNIT
//

// Enable instruction and data translation lookaside buffers and
// restricted user mode.
`define CFG_MMU_ENABLED
`define CFG_MMU_WITH_ASID

`ifdef CFG_MMU_ENABLED
`ifdef CFG_MMU_WITH_ASID
`define CFG_MMU_ASID_WIDTH               5
`endif
`endif

`endif
