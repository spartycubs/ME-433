function [arr] = plotimufilters()
colors = 'krbgcmyg';

% Best noise reduction: MAF filter 
% Least computational complexity: IIR filter


%OPEN SERIAL COMMUNICATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

port = 'COM3';   % Set up serial port address
if ~isempty(instrfind)                  % If PIC is not connected
    fclose(instrfind);                  % Close serial
    delete(instrfind);                  % ...
end

mySerial = serial(port, 'Baudrate', 230400); %Connect to PIC
set(mySerial,'TimeOut', 30)

fprintf('Opening port %s...\n', port);    % Print to screen
fopen(mySerial);

fprintf('Program starting.\n');
fprintf(mySerial, 'r\n');

data=zeros(5,1);
arr = zeros(5,100); %Preallocate plot vector here
arr(:,:) = NaN; %Initialize plot vector as empty data
for i=1:100
    %[arr(:,i)]= fscanf(mySerial, '%c');
    str = fgetl(mySerial);
    data = textscan(str, '%f', 'Delimiter',','); 
    data = data{1};
    arr(:,i)=data;
    %[arr(1,i)]= fscanf(mySerial, '%d');
end   
figure;
for xx = 2:5
    plot (6:100, arr(xx,6:100),colors(xx))
    hold on;
end
legend('raw', 'maf', 'iir', 'fir');
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%CLOSE FUNCTION

fclose(mySerial);
fprintf('Port %s closed.\n', port);

end