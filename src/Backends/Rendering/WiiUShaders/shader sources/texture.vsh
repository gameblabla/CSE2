; $MODE = "UniformRegister"

; $UNIFORM_VARS[0].name = "vertex_transform"
; $UNIFORM_VARS[0].type = "vec2"
; $UNIFORM_VARS[0].count = 1
; $UNIFORM_VARS[0].offset = 4
; $UNIFORM_VARS[0].block = -1
; $UNIFORM_VARS[1].name = "texture_coordinate_transform"
; $UNIFORM_VARS[1].type = "vec2"
; $UNIFORM_VARS[1].count = 1
; $UNIFORM_VARS[1].offset = 0
; $UNIFORM_VARS[1].block = -1

; $ATTRIB_VARS[0].name = "input_texture_coordinates"
; $ATTRIB_VARS[0].type = "vec2"
; $ATTRIB_VARS[0].location = 1
; $ATTRIB_VARS[1].name = "input_vertex_coordinates"
; $ATTRIB_VARS[1].type = "vec2"
; $ATTRIB_VARS[1].location = 0

; $NUM_SPI_VS_OUT_ID = 1
; $SPI_VS_OUT_ID[0].SEMANTIC_0 = 0

00 CALL_FS NO_BARRIER 
01 ALU: ADDR(32) CNT(9) 
      0  x: MUL         R0.x,  R1.x,  C0.x      
         y: MUL         R0.y,  R1.y,  C0.y      
         z: MUL         ____,  R2.y,  C1.y      VEC_120 
         w: MUL         ____,  R2.x,  C1.x      VEC_120 
         t: MOV         R1.z,  R2.z      
      1  x: ADD         R1.x,  PV0.w, -1.0f      
         y: ADD         R1.y,  PV0.z,  1.0f      
         w: MOV         R1.w,  R2.w      
02 EXP_DONE: POS0, R1
03 EXP_DONE: PARAM0, R0.xyzz  NO_BARRIER 
04 ALU: ADDR(41) CNT(1) 
      2  x: NOP         ____      
05 NOP NO_BARRIER 
END_OF_PROGRAM








