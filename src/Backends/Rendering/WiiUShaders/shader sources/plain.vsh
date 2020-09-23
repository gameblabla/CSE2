; $MODE = "UniformRegister"

; $UNIFORM_VARS[0].name = "vertex_transform"
; $UNIFORM_VARS[0].type = "vec2"
; $UNIFORM_VARS[0].count = 1
; $UNIFORM_VARS[0].offset = 0
; $UNIFORM_VARS[0].block = -1

; $ATTRIB_VARS[0].name = "input_vertex_coordinates"
; $ATTRIB_VARS[0].type = "vec2"
; $ATTRIB_VARS[0].location = 0

; $NUM_SPI_VS_OUT_ID = 1
; $SPI_VS_OUT_ID[0].SEMANTIC_0 = 0

00 CALL_FS NO_BARRIER 
01 ALU: ADDR(32) CNT(7) 
      0  x: MUL         ____,  R1.y,  C0.y      
         y: MUL         ____,  R1.x,  C0.x      
         z: MOV         R1.z,  R1.z      
         w: MOV         R1.w,  R1.w      
      1  x: ADD         R1.x,  PV0.y, -1.0f      
         y: ADD         R1.y,  PV0.x,  1.0f      
02 EXP_DONE: POS0, R1
03 EXP_DONE: PARAM0, R0.____
04 ALU: ADDR(39) CNT(1) 
      2  x: NOP         ____      
05 NOP NO_BARRIER 
END_OF_PROGRAM

