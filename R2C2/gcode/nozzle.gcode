; R2C2 Inkshield test file
G92 X0 Y0 E0 Z0; zero
G90; absolote positions
M200 E1000 ; 1000 pulses/mm
G1 F600    ; 1 mm/sec
M701 S10  ; pulse duration 5usec
M700 S0
G4;
G1 X0 Y0 E2
G4;
M700 S1
G4;
G1 X1E3
G4;
M700 S2
G4;
G1 X2E4
G4;
M700 S4
G4;
G1 X3E5
G4;
M700 S8
G4;
G1 X4E6
G4;
M700 S16
G4;
G1 X5E7
G4;
M700 S32
G4;
G1 X6E8
G4;
M700 S64
G4;
G1 X7E9
G4;
M700 S128
G4;
G1 X8E10
G4;
M700 S256
G4;
G1 X9E11
G4;
M700 S512
G4;
G1 X10E12
G4;
M700 S1024
G4;
G1 X11E13
G4;
M700 S2048
G4;
G1 X12E14
G4;
M700 S1
G4;
G1 X13E15
G4;
M700 S3
G4;
G1 X14E16
G4;
M700 S7
G4;
G1 X15E17
G4;
M700 S15
G4;
G1 X16E18
G4;
M700 S31
G4;
G1 X17E19
G4;
M700 S63
G4;
G1 X18E20
G4;
M700 S127
G4;
G1 X19E21
G4;
M700 S255
G4;
G1 X20E22
G4;
M700 S511
G4;
G1 X21E23
G4;
M700 S1023
G4;
G1 X22E24
G4;
M700 S2047
G4;
G1 X23E25
G4;
M700 S4095
G4;
G1 X24E26
G4;
