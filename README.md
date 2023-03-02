# brake
BLDC motor drive of 12kN brake.  
Main.c: Reset and main cycle.  
Control.c: Control program for this application.  
Variousfcs.c: Auxiliary functions. buttonfilt() button or switch filter, pwmvolt() set pwm motor output, Filt() adc filter, inmbmem() input values into modbus memory.  
Ecmotor.c: Motor commutation according to Hall sensors.  
Modbus.c: Some basic modbus protocol functions.   
Sericom.c : Uart for modbus.  
