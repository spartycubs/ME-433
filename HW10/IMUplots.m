function [array] = IMUplots()
colors = 'kmbgrcyg';

port = 'COM3';                          % Set up serial port address
if ~isempty(instrfind)                  % If PIC is not connected
    fclose(instrfind);                  % Close serial
    delete(instrfind);                  % ...
end

mySerial = serial(port, 'Baudrate', 230400); %Connect to PIC
set(mySerial,'TimeOut', 30)
fprintf('Opening port %s...\n', port);    % Print to screen
fopen(mySerial);
fprintf(mySerial, 'r\n');

temp=zeros(5,1);
array = zeros(5,100); %Preallocate array for plotting
array(:,:) = NaN; 
for i=1:100
    str = fgetl(mySerial);
    temp = textscan(str, '%f', 'Delimiter',','); 
    temp = temp{1};  %read data into temporary variable
    array(:,i)=temp; %write data into plotting array
end   
figure;
for type = 2:5   %plot raw, MAF, IIR, and FIR filters
    plot (6:100, array(type,6:100),colors(type))
    hold on;
end
legend('raw', 'maf', 'iir', 'fir');

fclose(mySerial);
end