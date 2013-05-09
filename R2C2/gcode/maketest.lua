--
-- make test gcode file for R2C2 inkshield
-- Peter Brier
--
print("; R2C2 Inkshield test file");
print("G92 X0 Y0 E0 Z0; zero");
print("G90; absolote positions");
print("M200 E1000 ; 1000 pulses/mm");
print("G1 F60    ; 1 mm/sec");
print("M701 S10  ; pulse duration 5usec");


function send_wait(s)
  print(s);
  print("M606;");
end;

s = 1;
x = 0;
e = 0;

send_wait("G1 X0 Y0");

-- single nozzles
for i=1,12 do
  x = x + 1;
  e = e + 1;
  send_wait("M700 S" .. s); -- select nozzle  
  send_wait("G1 X" .. x .. "E" .. e ); -- move
  x = x + 0.5;
  send_wait("G1 X" .. x ); -- move
  s = s * 2;
end


-- increasing nozzles
s = 0;
for i=1,12 do
  x = x + 1;
  e = e + 1;
  s = ((s+1) * 2)-1;
  send_wait("M700 S" .. s);
  send_wait("G1 X" .. x .. "E" .. e );
  x = x + 0.5;
  send_wait("G1 X" .. x ); -- move 
end
