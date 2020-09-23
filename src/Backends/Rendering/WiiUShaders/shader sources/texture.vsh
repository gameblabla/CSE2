; $MODE = "UniformRegister"

; $UNIFORM_VARS[0].name = "texture_coordinate_transform"
; $UNIFORM_VARS[0].type = "vec2"
; $UNIFORM_VARS[0].count = 1
; $UNIFORM_VARS[0].offset = 0
; $UNIFORM_VARS[0].block = -1

; $ATTRIB_VARS[0].name = "input_texture_coordinates"
; $ATTRIB_VARS[0].type = "vec2"
; $ATTRIB_VARS[0].location = 1
; $ATTRIB_VARS[1].name = "input_vertex_coordinates"
; $ATTRIB_VARS[1].type = "vec4"
; $ATTRIB_VARS[1].location = 0

; $NUM_SPI_VS_OUT_ID = 1
; $SPI_VS_OUT_ID[0].SEMANTIC_0 = 0

00 CALL_FS NO_BARRIER 
01 ALU: ADDR(32) CNT(2) 
      0  x: MUL         R1.x,  R1.x,  C0.x      
         y: MUL         R1.y,  R1.y,  C0.y      
02 EXP_DONE: POS0, R2
03 EXP_DONE: PARAM0, R1.xyzz  NO_BARRIER 
04 ALU: ADDR(34) CNT(1) 
      1  x: NOP         ____      
05 NOP NO_BARRIER 
END_OF_PROGRAM








