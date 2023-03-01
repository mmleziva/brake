# brake
bldc drive of 12kN brake
main.c:
control.c: control program for this application
variousfcs.c: auxiliary functions. buttonfilt() button or switch filter, pwmvolt() set pwm motor output, Filt() adc filter, inmbmem() input values into modbus memory
ecmotor.c: motor commutation according to Hall sensors
modbus.c: some modbus protocol functions 
sericom.c : uart for modbus
