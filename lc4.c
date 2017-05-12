/*
 * CIS 240 HW 10: LC4 Simulator
 * lc4.c
 */

#include "lc4.h"

FILE* global_file = 0;

//set_control_signals is a helper funtion used to set
//all control signals in decode_instructions based on
//a given instruction
void set_control_signals(ctrl* control, unsigned char pc_mux_ctl,
  unsigned char rs_mux_ctl,
  unsigned char rt_mux_ctl,
  unsigned char rd_mux_ctl,
  unsigned char reg_file_we,
  unsigned char arith_mux_ctl,
  unsigned char arith_ctl,
  unsigned char logic_mux_ctl,
  unsigned char logic_ctl,
  unsigned char shift_ctl,
  unsigned char const_ctl,
  unsigned char cmp_ctl,
  unsigned char alu_mux_ctl,
  unsigned char reg_input_mux_ctl,
  unsigned char data_we,
  unsigned char nzp_we) {

  control->mux_ctrls.pc_mux_ctl = pc_mux_ctl;
  control->mux_ctrls.rs_mux_ctl = rs_mux_ctl;
  control->mux_ctrls.rt_mux_ctl = rt_mux_ctl;
  control->mux_ctrls.rd_mux_ctl = rd_mux_ctl;
  control->mux_ctrls.reg_input_mux_ctl = reg_input_mux_ctl;
  control->mux_ctrls.arith_mux_ctl = arith_mux_ctl;
  control->mux_ctrls.logic_mux_ctl = logic_mux_ctl;
  control->mux_ctrls.alu_mux_ctl = alu_mux_ctl;
  control->alu_ctrls.arith_ctl = arith_ctl;
  control->alu_ctrls.logic_ctl = logic_ctl;
  control->alu_ctrls.const_ctl = const_ctl;
  control->alu_ctrls.shift_ctl = shift_ctl ;
  control->alu_ctrls.cmp_ctl = cmp_ctl;
  control->reg_ctrls.reg_file_we = reg_file_we ;
  control->reg_ctrls.nzp_we = nzp_we;
  control->reg_ctrls.data_we = data_we;
}
/*
 * Resets all control signals to 0.
 * Param: pointer to control signals struct
 */
void clear_control_signals(ctrl* control) {

  control->mux_ctrls.pc_mux_ctl = '\0';
  control->mux_ctrls.rs_mux_ctl = '\0';
  control->mux_ctrls.rt_mux_ctl = '\0';
  control->mux_ctrls.rd_mux_ctl = '\0';
  control->mux_ctrls.reg_input_mux_ctl = '\0';
  control->mux_ctrls.arith_mux_ctl = '\0';
  control->mux_ctrls.logic_mux_ctl = '\0';
  control->mux_ctrls.alu_mux_ctl = '\0';
  control->alu_ctrls.arith_ctl = '\0';
  control->alu_ctrls.logic_ctl = '\0';
  control->alu_ctrls.shift_ctl = '\0';
  control->alu_ctrls.const_ctl = '\0';
  control->alu_ctrls.cmp_ctl = '\0';
  control->reg_ctrls.reg_file_we = '\0';
  control->reg_ctrls.nzp_we = '\0';
  control->reg_ctrls.data_we = '\0';
}



/*
 * Resets the machine state as PennSim would do.
 * Param: pointer to current machine state
 * Make sure to reset the entire lc4 machine, including the control signals!
 */
void reset_lc4(lc4_state* state, ctrl* control) {

  clear_control_signals(control);

  //Start at PC x8200 with Privilege bit set to 1
  state->PC = 0x8200;
  state->PSR = 0x8000;
  state->uimm = 0;
  state->imm = 0;
  state->rs_addr = 0;
  state->rt_addr = 0;
  state->rd_addr = 0;
  
  for (int i = 0; i < 8; i++)
  {
    state->memory.R[i] = 0;
  }

  for (int i = 0; i < 65536; i++) {
    state->memory.memory_array[i] = 0;
  }
}

//set_immediate_values is a helper function used in update_
//lc4_state to set the immediate values with the state struct
//For signed immediate values, it calls on sext, to sign
//extend
void set_immediate_values(lc4_state* state, unsigned short int I){
  unsigned short int opr = INSN_OPR(I);
  if (opr == 0)
  {
    //BR
    unsigned short int BR = INSN_BR(I);
    if (BR == 0) 
    {
      return;
    }
    else 
    {
      state->imm = sext(9, I & 0x01FF);
    }
  }

  //ARITHMETIC
  else if (opr == 1)
  {
    unsigned short int arith = INSN_ARITH(I);
    if ((arith >> 2) == 1) 
    {
      state->imm = sext(5, I & 0x001F);
    }
  }
  //CMP
  else if (opr == 2)
  {
    unsigned short int cmp = INSN_CMP(I);
    if (cmp == 2)
    {
      state->imm = sext(7, I & 0x007F);
    }
    else if (cmp == 3)
    {
      state->uimm = I & 0x007F;
    }
  }
  //JSR
  else if (opr == 4)
  {
    unsigned short int jsr = INSN_JSR(I);
    if (jsr == 1)
    {
      state->imm = sext(11, I & 0x07FF);
    }
  }
  //Logic
  else if (opr == 5)
  {
    unsigned short int logic = INSN_LGC(I);
    if ((logic >> 2) == 1)
    {
      state->imm = sext(5, I & 0x001F);
    }
  }
  //LDR
  else if (opr == 6)
  {
   state->imm = sext(6, I & 0x003F);
 }

 //STR
 else if (opr == 7)
 {
   state->imm = sext(6, I & 0x003F);
 }

 //CONST
 else if (opr == 9)
 {
  state->imm = sext(9, I & 0x01FF);
} 

//SHIFT
else if (opr == 10)
{
  unsigned short int shift = INSN_SHFT(I);
  if (shift == 0 || shift == 1  || shift == 2)
  {
   state->uimm =  I & 0x000F;
 }
} 

//JMP
else if (opr == 12)
{
  unsigned int jmp = INSN_JMP(I);
  if (jmp == 1)
  {
    state->imm = sext(11, I & 0x07FF);
  }
}
//HICONST
else if (opr == 13)
{
  state->uimm = I & 0x00FF;
}
//TRAP
else if (opr == 15)
{
 state->uimm = I & 0x00FF;
}
}


/*
 * Decodes instruction and sets control signals accordingly.
 * Params: pointer to control signals struct, instruction to decode
 * HINT: Split up the individual decoding into multiple functions
 * Here is a great place to use switch statements to call a specific decoding function
 */
void decode_instruction(ctrl* control, unsigned short int I) {
  unsigned short int opr = INSN_OPR(I);
  if (opr == 0)
  {
    unsigned short int BR = INSN_BR(I);
    if (BR == 0) 
    {
     set_control_signals(control, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
   }
   else 
   {
    set_control_signals(control, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  }
}

  //ARITHMETIC
else if (opr == 1)
{
  unsigned short int arith = INSN_ARITH(I);
  if (arith == 0)
  {
    set_control_signals(control, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
  }
  else if (arith == 1)
  {
    set_control_signals(control, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1);
  }
  else if (arith == 2)
  {
    set_control_signals(control, 1, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1);
  }
  else if (arith == 3)
  {
    set_control_signals(control, 1, 0, 0, 0, 1, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1);
  }
  else if ((arith >> 2) == 1) 
  {
    set_control_signals(control, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
  }
}

  //CMP
else if (opr == 2)
{
  unsigned short int cmp = INSN_CMP(I);
  if (cmp == 0)
  {
    set_control_signals(control, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 1);
  }
  else if (cmp == 1)
  {
    set_control_signals(control, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4, 0, 0, 1);
  }
  else if (cmp == 2)
  {
    set_control_signals(control, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 0, 0, 1);
  }
  else if (cmp == 3)
  {
    set_control_signals(control, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 0, 0, 1);
  }
}
//JSR
else if (opr == 4)
{
  unsigned short int jsr = INSN_JSR(I);
  if (jsr == 1)
  {
    set_control_signals(control, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
  }
  else if (jsr == 0)
  {
    set_control_signals(control, 3, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1);
  }
}
//LOGIC
else if (opr == 5)
{
  unsigned short int logic = INSN_LGC(I);
  if (logic == 0)
  {
    set_control_signals(control, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1);
  }
  else if (logic == 1)
  {
    set_control_signals(control, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1);
  }
  else if (logic == 2)
  {
    set_control_signals(control, 1, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 1);
  }
  else if (logic == 3)
  {
    set_control_signals(control, 1, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 1, 0, 0, 1);
  }
  else if ((logic >> 2) == 1)
  {
    set_control_signals(control, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1);
  }
}
//LDR
else if (opr == 6)
{
  set_control_signals(control, 1, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1);
}
//STR
else if (opr == 7)
{
  set_control_signals(control, 1, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
}
//RTI
else if (opr == 8)
{
  set_control_signals(control, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}
//CONST
else if (opr == 9)
{
  set_control_signals(control, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 1);
} 
//SHIFT
else if (opr == 10)
{
  unsigned short int shift = INSN_SHFT(I);
  if (shift == 0)
  {
    set_control_signals(control, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1);
  }

  else if (shift == 1)
  {
    set_control_signals(control, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 1);
  }

  else if (shift == 2)
  {
    set_control_signals(control, 1, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 1);
  }

  else if (shift == 3)
  {
   set_control_signals(control, 1, 0, 0, 0, 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 1);
 }
} 
//JMP
else if (opr == 12)
{
  unsigned int jmp = INSN_JMP(I);
  if (jmp == 0) {
    set_control_signals(control, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  }

  else if (jmp == 1)
  {
    set_control_signals(control, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  }
}
//HICONST
else if (opr == 13)
{
  set_control_signals(control, 1, 2, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 3, 0, 0, 1);
}
//TRAP
else if (opr == 15)
{
  set_control_signals(control, 4, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1);
}
}



/*
 * Updates the machine state, simulating a single clock cycle.
 * Check for exceptions here.
 * Set privileged bit.
 * Set registers.
 * Returns 0 if update successful, a nonzero error code if unsuccessful.
 * Params: pointer to current machine state, pointer to control signals struct
 */
int update_lc4_state(lc4_state* state, ctrl* control) {
  unsigned short int instructions = state->memory.memory_array[state->PC];
  decode_instruction(control, instructions);
  set_registers(state, control, instructions);
  unsigned short int rs_out = rs_mux(state, control);
  unsigned short int rt_out = rt_mux(state, control);
  set_immediate_values(state, instructions);
  unsigned short int alu_out = alu_mux(state, control, rs_out, rt_out);
  unsigned short int reg_out = reg_input_mux(state, control, alu_out);
  if (control->reg_ctrls.data_we == 1) 
  {
    state->memory.memory_array[alu_out] = rt_out;
  }
  if (control->reg_ctrls.reg_file_we == 1)
  {
    state->memory.R[state->rd_addr] = reg_out;
  }
  if (control->reg_ctrls.reg_file_we == 1 || ((instructions & 0xF000) == 0x2000))
  {
    state->PSR = state->PSR & 0x8000;
    if ((short) reg_out < 0) {
      state->PSR = state->PSR | 4;
    }
    else if ((short) reg_out == 0) {
      state->PSR = state->PSR | 2;
    }
    else if ((short) reg_out > 0) {
      state->PSR = state->PSR | 1;
    }
  }
  if (check_exceptions(state))
  {
    return 1;
  }
  if ((instructions & 0xF000) == 0xF000) 
  {
    state->PSR = state->PSR | 0x8000;
  }
  else if ((instructions & 0xF000) == 0x8000) 
  {
    state->PSR = state->PSR & 0x7FFF;
  }
  print_operation(state, global_file, control);
  state->PC = pc_mux(state, control, rs_out);
  return 0;
}



/*
* Check if any exceptions have happened in the current state.
* Returns int corresponding to error code that occured, or 0 if no errors
* Param: pointer to current machine state
*/
int check_exceptions(lc4_state* state) {
  unsigned short int I = state->memory.memory_array[state->PC];
  if (state->PC >= 0x2000 & state->PC < 0x8000) 
  {
    printf("PC out of bounds\n");
    return 1;
  }

  if (state->PC >= 0xA000) 
  {
    printf("Error: PC out of bounds\n");
    return 1;
  }
  if (INSN_OPR(I) == 6 || (INSN_OPR(I) == 7))
  {
    if ((state->memory.R[state->rs_addr] + state->imm) < 0x2000 || ((state->memory.R[state->rs_addr] + state->imm) >= 0x8000 && (state->memory.R[state->rs_addr] + state->imm) < 0xA000))
    {
      printf("%0.4X\n", state->memory.R[state->rs_addr] + state->imm);
      printf("Error: LDR/STR out of bounds\n");
      return 2;
    }
    else if ((state->memory.R[state->rs_addr] + state->imm) > 0x9FFF && !(state->PSR >= 0x8000)) 
    {
      printf("Error: Unpriveleged LDR/STR\n");
      return 3;
    }
  }
  if (state->PC >= 0x8000 && !(state->PSR >= 0x8000))
  {
    printf("Error: Unpriveleged access of OS\n");
    return 3;
  }
  return 0;
}




/*
 * Sets file pointer.
 * Param: pointer to the file
 */
void set_fp(FILE* fp) {

  global_file = fp;
}



/*
 * Sets the values for rs_addr, rt_addr, and rd_addr fields in machine_state based
 * on current instruction.
 * Param: pointer to machine_state, current instruction
 */
void set_registers(lc4_state* state, ctrl* control, unsigned short int I) {
//printf("%d\n", INSN_RSO(I));
  if (control->mux_ctrls.rs_mux_ctl == 0) {
    state->rs_addr = INSN_RSO(I);
  }
  
  else if (control->mux_ctrls.rs_mux_ctl == 1) {
    state->rs_addr = 7;
  }

  else if (control->mux_ctrls.rs_mux_ctl == 2) {
    state->rs_addr = INSN_RST(I);

  }

  if (control->mux_ctrls.rt_mux_ctl == 0) {
    state->rt_addr = INSN_RTO(I);
  }
  
  else if (control->mux_ctrls.rt_mux_ctl == 1) 
  {
    state->rt_addr = INSN_RST(I);
  }

  if (control->mux_ctrls.rd_mux_ctl == 0) {
    state->rd_addr = INSN_RST(I);
  }
  
  else if (control->mux_ctrls.rd_mux_ctl == 1) 
  {
    state->rd_addr = 7;
  }
 // printf("Set %d\n", state->rd_addr);
}



/*
 * Returns the current output of the RS mux.
 * Param: pointer to current machine state, pointer to control signals struct
 */
unsigned short int rs_mux(lc4_state* state, ctrl* control) {

  return state->memory.R[state->rs_addr];
}



/*
 * Returns the current output of the RT mux.
 * Param: pointer to current machine state, pointer to control signals struct
 */
unsigned short int rt_mux(lc4_state* state, ctrl* control) {

  return state->memory.R[state->rt_addr];
}



/*
 * Returns the current output of the ALU mux.
 * Params: pointer to current machine state, RS mux output, RT mux output, pointer to control signals struct
 */
unsigned short int alu_mux(lc4_state* state, ctrl* control, unsigned short int rs_out, unsigned short int rt_out) {

  if (control->mux_ctrls.alu_mux_ctl == 0) 
  {
    if (control->alu_ctrls.arith_ctl == 0) 
    {
      if (control->mux_ctrls.arith_mux_ctl == 0) {
        return rs_out + rt_out;
      }
      else if (control->mux_ctrls.arith_mux_ctl == 1 || control->mux_ctrls.arith_mux_ctl == 2) {
        return rs_out + state->imm;
      }
    }
    else if (control->alu_ctrls.arith_ctl == 1) 
    {
      if (control->mux_ctrls.arith_mux_ctl == 0) {
        return rs_out * rt_out;
      }
    }
    else if (control->alu_ctrls.arith_ctl == 2) 
    {
      if (control->mux_ctrls.arith_mux_ctl == 0) {
        return rs_out - rt_out;
      }
    }
    else if (control->alu_ctrls.arith_ctl == 3) 
    {
      if (control->mux_ctrls.arith_mux_ctl == 0) {
        return rs_out / rt_out;
      }
    }
    else if (control->alu_ctrls.arith_ctl == 4) 
    {
      if (control->mux_ctrls.arith_mux_ctl == 0) {
        return (rs_out % rt_out);
      }
    }
  }
  else if (control->mux_ctrls.alu_mux_ctl == 1) 
  {  
    if (control->alu_ctrls.logic_ctl == 0)
    {
      if (control->mux_ctrls.logic_mux_ctl == 0)
      {
        return (rs_out & rt_out);
      }
      else if (control->mux_ctrls.logic_mux_ctl == 1)
      {
        return (rs_out & state->imm);
      }
    }
    else if (control->alu_ctrls.logic_ctl == 1)
    {
      return ~rs_out;
    }
    else if (control->alu_ctrls.logic_ctl == 2)
    {
      if (control->mux_ctrls.logic_mux_ctl == 0)
      {
        return (rs_out | rt_out);
      }
    }
    else if (control->alu_ctrls.logic_ctl == 3)
    {
      if (control->mux_ctrls.logic_mux_ctl == 0)
      {
        return (rs_out ^ rt_out);
      }
    }
  }
  else if (control->mux_ctrls.alu_mux_ctl == 2) 
  {
    if (control->alu_ctrls.shift_ctl == 0)
    {
      return (rs_out << state->uimm);
    }
    else if (control->alu_ctrls.shift_ctl == 1)
    {
      return ((short) rs_out >> state->uimm);
    }
    else if (control->alu_ctrls.shift_ctl == 2)
    {
      return (rs_out >> state->uimm);
    }
  }
  else if (control->mux_ctrls.alu_mux_ctl == 3) 
  {
    if (control->alu_ctrls.const_ctl == 0) 
    {
      return state->imm;
    }
    else if (control->alu_ctrls.const_ctl == 1) 
    {
      return ((rs_out & 0xFF) | (state->uimm << 8));
    }
  }
  else if (control->mux_ctrls.alu_mux_ctl == 4) 
  {
    if (control->alu_ctrls.cmp_ctl == 0) 
    {
      if ((short) rs_out < (short) rt_out)
      {
        return (short) -1;
      }
      else if ((short) rs_out > (short) rt_out)
      {
        return 1;
      }
      else if ((short) rs_out == (short) rt_out)
      {
        return 0;
      }
    }
    else if (control->alu_ctrls.cmp_ctl == 1) 
    {
      if (rs_out < rt_out)
      {
        return (short) -1;
      }
      else if (rs_out > rt_out)
      {
        return 1;
      }
      else if (rs_out == rt_out)
      {
        return 0;
      }
    }
    else if (control->alu_ctrls.cmp_ctl == 2) 
    {
      if ((short) rs_out < state->imm)
      {
        return (short) -1;
      }
      else if ((short) rs_out > state->imm)
      {
        return 1;
      }
      else if ((short) rs_out == state->imm)
      {
        return 0;
      }
    }
    else if (control->alu_ctrls.cmp_ctl == 3) 
    {
      if (rs_out < state->uimm)
      {
        return (short) -1;
      }
      else if (rs_out > state->uimm)
      {
        return 1;
      }
      else if (rs_out == state->uimm)
      {
        return 0;
      }
    }
  }
}



/*
 * Returns the current output of the register input mux.
 * Params: pointer to current machine state, current ALU mux output, pointer to control signals struct
 */
unsigned short int reg_input_mux(lc4_state* state, ctrl* control, unsigned short int alu_out) {

  if (control->mux_ctrls.reg_input_mux_ctl == 0) 
  {
    return alu_out;
  }
  else if (control->mux_ctrls.reg_input_mux_ctl == 1) 
  {
    return state->memory.memory_array[alu_out];
  }
  else if (control->mux_ctrls.reg_input_mux_ctl == 2)
  {
    return state->PC + 1;
  }
}



/*
 * Returns the current output of the PC mux.
 * Params: current RS mux output, pointer to current machine state, pointer to control signals struct
 */
unsigned short int pc_mux(lc4_state* state, ctrl* control, unsigned short int rs_out) {

  if (control->mux_ctrls.pc_mux_ctl == 0)
  {
    unsigned short int check_nzp = (state->memory.memory_array[state->PC] >> 9) & 7;
    if (((state->PSR & 7) & check_nzp) > 0)
    {
      return state->PC + 1 + state->imm;
    }
    else 
    {
      return state->PC + 1;
    }
  }
  else if (control->mux_ctrls.pc_mux_ctl == 1)
  {
    return state->PC + 1;
  }

  else if (control->mux_ctrls.pc_mux_ctl == 2)
  {
    return state->PC + 1 + state->imm;
  }

  else if (control->mux_ctrls.pc_mux_ctl == 3)
  {
    return (short) rs_out;
  }

  else if (control->mux_ctrls.pc_mux_ctl == 4)
  {
    return (0x8000 | state->uimm);
  }

  else if (control->mux_ctrls.pc_mux_ctl == 5)
  {
    return ((state->PC & 0x8000) | (state->imm << 4));
  }
}



/*
* Sign extends the given input unsigned short int to the inputted length
* Returns the sign extended value.
* Params: length to sign extend to, value to sign extend
*/
short int sext(unsigned int length, unsigned short int input) {

  short int left = input << (16 - length);
  short int right = left >> (16 - length);
  return right;
}

/*
* Prints information about the current state
* Very helpful in debugging!
* Implementing this will also help with receiving partial credit.
*/
void print_lc4_state(lc4_state* state, ctrl* control) {
  printf("Control Signals: \n");
  printf("%d ", control->mux_ctrls.pc_mux_ctl);
  printf("%d ", control->mux_ctrls.rs_mux_ctl);
  printf("%d ", control->mux_ctrls.rt_mux_ctl);
  printf("%d ", control->mux_ctrls.rd_mux_ctl);
  printf("%d ", control->mux_ctrls.reg_input_mux_ctl);
  printf("%d ", control->mux_ctrls.arith_mux_ctl);
  printf("%d ", control->mux_ctrls.logic_mux_ctl);
  printf("%d ", control->mux_ctrls.alu_mux_ctl);
  printf("%d ", control->alu_ctrls.arith_ctl);
  printf("%d ", control->alu_ctrls.logic_ctl);
  printf("%d ", control->alu_ctrls.const_ctl);
  printf("%d ", control->alu_ctrls.shift_ctl);
  printf("%d ", control->alu_ctrls.cmp_ctl);
  printf("%d ", control->reg_ctrls.reg_file_we);
  printf("%d ", control->reg_ctrls.nzp_we);
  printf("%d\n", control->reg_ctrls.data_we);

  printf("State: \n");
  printf("%0.4x ", state->PC);
  printf("%0.4x ", state->PSR);
  printf("%d ", state->uimm);
  printf("%d ", state->imm);
  printf("%d ", state->rs_addr);
  printf("%d ", state->rt_addr);
  printf("%d\n", state->rd_addr);

  printf("Registers: \n");
  printf("%d ", (short) state->memory.R[0]);
  printf("%d ", (short) state->memory.R[1]);
  printf("%d ", (short) state->memory.R[2]);
  printf("%d ", (short) state->memory.R[3]);
  printf("%d ", (short) state->memory.R[4]);
  printf("%d ", (short) state->memory.R[5]);
  printf("%d ", (short) state->memory.R[6]);
  printf("%d\n", (short) state->memory.R[7]);
  printf("\n");

}

/*
* Prints current operation to stdout and the output file
*/
void print_operation(lc4_state* state, FILE* fp, ctrl* control) {
  unsigned short int PC = state->PC;
  printf("%0.4X ", PC);
  unsigned short int I = state->memory.memory_array[state->PC];
  unsigned short int opr = INSN_OPR(I);
  if (opr == 0)
  {
    unsigned short int BR = INSN_BR(I);
    if (BR == 0) 
    {
     printf("NOP\n");
   }
   BR = BR & 7;
   if (BR == 4)
   {
    printf("BRn x%.4X\n", state->PC + 1 + state->imm);
  }
  else if (BR == 6)
  {
    printf("BRnz x%.4X\n", state->PC + 1 + state->imm);
  }
  else if (BR == 5)
  {
    printf("BRnp x%.4X\n", state->PC + 1 + state->imm);
  }
  else if (BR == 2)
  {
    printf("BRz x%.4X\n", state->PC + 1 + state->imm);
  }
  else if (BR == 3)
  {
    printf("BRzp x%.4X\n", state->PC + 1 + state->imm);
  }
  else if (BR == 1)
  {
    printf("BRp x%.4X\n", state->PC + 1 + state->imm);
  }
  else if (BR == 7)
  {
    printf("BRnzp x%.4X\n", state->PC + 1 + state->imm);
  }
}
  //ARITHMETIC
else if (opr == 1)
{
  unsigned short int arith = INSN_ARITH(I);
  if (arith == 0)
  {
    printf("ADD R%d, R%d, R%d\n", state->rd_addr, state->rs_addr, state->rt_addr);
  }
  else if (arith == 1)
  {
    printf("MUL R%d, R%d, R%d\n", state->rd_addr, state->rs_addr, state->rt_addr);
  }
  else if (arith == 2)
  {
    printf("SUB R%d, R%d, R%d\n", state->rd_addr, state->rs_addr, state->rt_addr);
  }
  else if (arith == 3)
  {
    printf("DIV R%d, R%d, R%d\n", state->rd_addr, state->rs_addr, state->rt_addr);
  }
  else if ((arith >> 2) == 1) 
  {
    printf("ADD R%d, R%d, #%d\n", state->rd_addr, state->rs_addr, state->imm);
  }
}

  //CMP
else if (opr == 2)
{
  unsigned short int cmp = INSN_CMP(I);
  if (cmp == 0)
  {
    printf("CMP R%d, R%d\n", state->rs_addr, state->rt_addr);
  }
  else if (cmp == 1)
  {
    printf("CMPU R%d, R%d\n", state->rs_addr, state->rt_addr);
  }
  else if (cmp == 2)
  {
    printf("CMPI R%d, #%d\n", state->rs_addr, state->imm);
  }
  else if (cmp == 3)
  {
    printf("CMPIU R%d, #%d\n", state->rs_addr, state->uimm);
  }
}
//JSR
else if (opr == 4)
{
  unsigned short int jsr = INSN_JSR(I);
  if (jsr == 1)
  {
    printf("JSR 0x%.4X\n", ((state->PC & 0x8000) | (state->imm << 4)));
  }
  else if (jsr == 0)
  {
    printf("JSRR R%d\n", state->rs_addr);
  }
}
//LOGIC
else if (opr == 5)
{
  unsigned short int logic = INSN_LGC(I);
  if (logic == 0)
  {
    printf("AND R%d, R%d, R%d\n", state->rd_addr, state->rs_addr, state->rt_addr);
  }
  else if (logic == 1)
  {
    printf("NOT R%d, R%d\n", state->rd_addr, state->rs_addr);
  }
  else if (logic == 2)
  {
    printf("OR R%d, R%d, R%d\n", state->rd_addr, state->rs_addr, state->rt_addr);
  }
  else if (logic == 3)
  {
    printf("XOR R%d, R%d, R%d\n", state->rd_addr, state->rs_addr, state->rt_addr);
  }
  else if ((logic >> 2) == 1)
  {
    printf("AND R%d, R%d, #%d\n", state->rd_addr, state->rs_addr, state->imm);
  }
}
//LDR
else if (opr == 6)
{
  printf("LDR R%d, R%d, #%d\n", state->rd_addr, state->rs_addr, state->imm);
}
//STR
else if (opr == 7)
{
  printf("STR R%d, R%d, #%d\n", state->rd_addr, state->rs_addr, state->imm);
}
//RTI
else if (opr == 8)
{
  printf("RTI\n");
}
//CONST
else if (opr == 9)
{
  printf("CONST R%d, #%d\n", state->rd_addr, state->imm);
} 
//SHIFT
else if (opr == 10)
{
  unsigned short int shift = INSN_SHFT(I);
  if (shift == 0)
  {
    printf("SLL R%d, R%d, #%d\n", state->rd_addr, state->rs_addr, state->uimm);
  }

  else if (shift == 1)
  {
    printf("SRA R%d, R%d, #%d\n", state->rd_addr, state->rs_addr, state->uimm);
  }

  else if (shift == 2)
  {
    printf("SRL R%d, R%d, #%d\n", state->rd_addr, state->rs_addr, state->uimm);
  }

  else if (shift == 3)
  {
   printf("MOD R%d, R%d, R%d\n", state->rd_addr, state->rs_addr, state->rt_addr);
 }
} 
//JMP
else if (opr == 12)
{
  unsigned int jmp = INSN_JMP(I);
  if (jmp == 0) {
    printf("JMPR R%d\n", state->rs_addr);
  }

  else if (jmp == 1)
  {
    printf("JMP x%0.4X\n", state->PC + 1 + state->imm);
  }
}
//HICONST
else if (opr == 13)
{
  printf("HICONST R%d, x%0.2X\n", state->rd_addr, state->uimm);
}
//TRAP
else if (opr == 15)
{
  printf("TRAP x%0.4X\n", state->uimm);
}

//Print Registers
for (int i = 0; i < 8; i++) {
  printf("R%d = %0.4X ", i, state->memory.R[i]);
}

printf("\nPSR = %d\n", (state->PSR >> 15 & 1));
printf("NZP = %d\n", (state->PSR & 7));

//FILE PRINTING
fprintf(fp, "%0.4X ", PC);
unsigned short int instructions = I;
int i = 0;
while (i < 16) {
  if (instructions & 0x8000)
  {
    fprintf(fp, "1");
  }  
  else
  {
    fprintf(fp, "0");
  }

  instructions <<= 1;
  i++;
}

fprintf(fp, " %0.1X ", control->reg_ctrls.reg_file_we);
if (control->reg_ctrls.reg_file_we == 1)
{
 fprintf(fp, "%0.1X ", state->rd_addr);
 fprintf(fp, "%0.4X ", state->memory.R[state->rd_addr]);
}
else fprintf(fp, "0 0000 ");

fprintf(fp, "%0.1X ", control->reg_ctrls.nzp_we);

if (control->reg_ctrls.nzp_we == 1)
{
  fprintf(fp, "%0.1X ", state->PSR & 7);
}
else fprintf(fp, "0 ");

fprintf(fp, "%0.1X ", control->reg_ctrls.data_we);

if (I >> 12 == 6 || I >> 12 == 7)
{
 fprintf(fp, "%0.4X ", state->memory.R[state->rs_addr] + state->imm);
 if (I >> 12 == 7)
 {
   fprintf(fp, "%0.4X\n", state->memory.R[state->rt_addr]);
 }
 else if (I >> 12 == 6)
 {
  fprintf(fp, "%0.4X\n", state->memory.R[state->rd_addr]);
}
}
else fprintf(fp, "0000 0000\n");

}



