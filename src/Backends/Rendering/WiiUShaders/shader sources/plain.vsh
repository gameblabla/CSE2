; $MODE = "UniformRegister"
; $ATTRIB_VARS[0].name = "input_vertex_coordinates"
; $ATTRIB_VARS[0].type = "vec2"
; $ATTRIB_VARS[0].location = 0
; $NUM_SPI_VS_OUT_ID = 1
; $SPI_VS_OUT_ID[0].SEMANTIC_0 = 0

00 CALL_FS NO_BARRIER 
01 EXP_DONE: POS0, R1
02 EXP_DONE: PARAM0, R0.____
03 ALU: ADDR(32) CNT(1) 
      0  x: NOP         ____      
04 NOP NO_BARRIER 
END_OF_PROGRAM






