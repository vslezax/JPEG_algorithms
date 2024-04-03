clc;
clear;
close all;

folder_path = 'C:\Users\user\CLionProjects\JPEG\images\freq';
file_list = dir(fullfile(folder_path, 'DC_R*.txt'));

for i = 1:numel(file_list)
    file_name = file_list(i).name;
    file_path = fullfile(folder_path, file_name);
    
    data = dlmread(file_path);
    x = data(:, 1);
    y = data(:, 2);
    
    figure;
    bar(x, y);
    title(sprintf('%s', file_name), 'Interpreter', 'none');
    xlabel('DC values');
    ylabel('Count');
end

file_list = dir(fullfile(folder_path, 'DC_diff_R*.txt'));

for i = 1:numel(file_list)
    file_name = file_list(i).name;
    file_path = fullfile(folder_path, file_name);
    
    data = dlmread(file_path);
    x = data(:, 1);
    y = data(:, 2);
    
    figure;
    bar(x, y);
    title(sprintf('%s', file_name), 'Interpreter', 'none');
    xlabel('DC value difference');
    ylabel('Count');
end
