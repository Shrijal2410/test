
hfig = figure;
set(hfig, 'Color', [0 0 0]); % Set the figure background to black

hAxes = axes;
set(hAxes, 'Color', [0 0 0]); % Set the axes background to black

% Set the axes and labels colors to white for better visibility
hAxes.XColor = [1 1 1];
hAxes.YColor = [1 1 1];
xlabel('time (s)', 'Color', [1 1 1])
ylabel('Absolute Position of Slider (mm)', 'Color', [1 1 1])
grid on
hold on
plot(out.RingFingerPID.time, out.RingFingerPID.signals(1).values, 'Color', [1 0.38 0.53], 'LineWidth', 1.5, 'DisplayName', 'Set Point')
plot(out.RingFingerPID.time, out.RingFingerPID.signals(2).values, 'Color', [0 1 0], 'LineWidth', 1.5, 'DisplayName', 'Measured Position of Slider') % Adjust the color as needed

% Create the legend and position it in the northeast corner
hLegend = legend('show', 'TextColor', [1 1 1], 'Location', 'northeast'); % Set the legend text color to white and position to northeast
legendPosition = get(hLegend, 'Position');
legendPosition(2) = legendPosition(2) + 0.02; % Move the legend upward by increasing the bottom position
set(hLegend, 'Position', legendPosition);

pictureWidth = 20;
hw_ratio = 0.65;
set(findall(hfig, '-property', 'FontSize'), 'FontSize', 17);
set(findall(hfig, '-property', 'Box'), 'Box', 'off');
set(findall(hfig, '-property', 'Interpreter'), 'Interpreter', 'latex');
set(findall(hfig, '-property', 'TickLabelInterpreter'), 'TickLabelInterpreter', 'latex');

% Set the figure's position and size
set(hfig, 'Units', 'centimeters', 'Position', [3 3 pictureWidth hw_ratio * pictureWidth]);
pos = get(hfig, 'Position');
set(hfig, 'PaperPositionMode', 'Auto', 'PaperUnits', 'centimeters', 'PaperSize', [pos(3) pos(4)]);

% Save the figure in different formats
%print(hfig, 'Wrist_roll_pdf_figure', '-dpdf', '-vector', '-fillpage');
%print(hfig, 'Wrist_roll_png_figure', '-dpng', '-vector');
savefig(hfig, 'Ring_finger_figure.fig'); % Save the figure in .fig format

% Alternatively, you can use saveas for more formats
% saveas(hfig, 'Wrist_roll_figure.fig'); % Save as .fig file

