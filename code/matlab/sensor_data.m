% Load data
data = readtable("data\raw\curl.csv");
X = data{:,"Var2"};
Y = data{:,"Var3"};

fs = 10; % Sampling frequency
interval_length = fs * 5; % 5 seconds (4 seconds skip + 1 second data)
data_length = fs * 1; % 1 second of data to keep

% Initialize arrays to store mean and peak values
mean_values_X = []; mean_values_Y = [];
peak_values_X = []; peak_values_Y = [];

% Extract mean and peak values for each relevant 1-second interval
% curl.csv
% for i = 0.8*fs+1 : interval_length : length(X(1:300,:))
% indexMiddle.csv
% for i = 4*fs+1 : interval_length : length(X)
% ringLittle.csv
% for i = 3.7*fs+1 : interval_length : length(X)
% roll.csv
% for i = 4*fs+1 : interval_length : length(X)
% snap.csv
% for i = 4*fs+1 : interval_length : length(X)
% thumb.csv
% for i = 13.8*fs+1 : interval_length : length(X)
% wrist.csv
% for i = 8.8*fs+1 : interval_length : length(X)
    if i + data_length - 1 <= length(X)
        X_segment = X(i:i+data_length-1);
        Y_segment = Y(i:i+data_length-1);
        
        % Mean and peak for X
        mean_values_X = [mean_values_X, mean(X_segment)];
        peak_values_X = [peak_values_X, max(abs(X_segment))];
        
        % Mean and peak for Y
        mean_values_Y = [mean_values_Y, mean(Y_segment)];
        peak_values_Y = [peak_values_Y, max(abs(Y_segment))];
    end
end

% Calculate overall mean of mean values and peak values
overall_mean_X = mean(mean_values_X);
overall_mean_Y = mean(mean_values_Y);
overall_peak_X = mean(peak_values_X);
overall_peak_Y = mean(peak_values_Y);

% Display results
disp('Mean values at instances for X:');
disp(mean_values_X);
disp('Mean values at instances for Y:');
disp(mean_values_Y);

disp('Peak values at instances for X:');
disp(peak_values_X);
disp('Peak values at instances for Y:');
disp(peak_values_Y);



% Display minimum (mean) and maximum (peak) for X and Y
disp('For X:');
disp(['Minimum (mean): ', num2str(overall_mean_X), ', Maximum (peak): ', num2str(overall_peak_X)]);
disp('For Y:');
disp(['Minimum (mean): ', num2str(overall_mean_Y), ', Maximum (peak): ', num2str(overall_peak_Y)]);