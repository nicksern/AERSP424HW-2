% MATLAB code to simulate a 3D N-body problem with elastic collisions
% with a custom RK4 algorithm
% Melik Demirel

% Clear variables and figures
clear; close all; clc;

% % EXAMPLE INPUTS # 1 (no collisions but cool movement)
% n = 3; % Number of spheres
% m = [10000000000; 10000000000; 1000000000]; % Masses of the spheres
% L = [0.5; 0.2; 0.1]; % Radii of the spheres
% r0 = [-1, 0, 0; 1, 0, 3; 0, -3, 0]; % Initial positions
% v0 = [0, 0, 0; -0.5, 0, 0; 0, 1, 0]; % Initial velocities
% consts.G = 6.67e-11; % Gravitational constant (set to zero for this test case)
% consts.k = 2000; % Coefficient of restitution
% consts.c = 100; % Coefficient of damping
% t0 = 0; % Initial time
% tf = 100; % Final time
% dt = 0.001;
% tol = 0.01;

%EXAMPLE INPUTS # 2 (shows collisions)
n = 3;
m = [3;1;0.5];
L = [0.5;0.5;0.2];
r0 = [0,0,0; 10,1,0; 0,0,10];
v0 = [1,0,0; -1,0,0; 0,0,0];
consts.G = 6;
consts.k = 3000; % Coefficient of stiffness
consts.c = 2; % Coefficient of damping
t0 = 0; % Initial time
tf = 20; % Final time
dt = 0.001; % ∆time
tol = 0.01; % Selection tol

%% Main

% Initial state vector s0
% For each sphere, positions and velocities: x, y, z, vx, vy, vz
s0 = zeros(6*n, 1);
for i = 1:n
    idx = 6*(i-1);
    s0(idx+1) = r0(i,1);
    s0(idx+2) = r0(i,2);
    s0(idx+3) = r0(i,3);
    s0(idx+4) = v0(i,1);
    s0(idx+5) = v0(i,2);
    s0(idx+6) = v0(i,3);
end

% Time span
tspan = t0:dt:tf;

% Call ode45
[t, s] = RK4(@(t,s) NBodyCollision(t, s, n, m, L, consts), tspan, s0);

% Select Frames for visualization
select_t = t0:0.1:tf;
[t, s] = select_frames(t, s, select_t, tol);

% Visualization
visualize_simulation(t, s, n, L);

%% ODE Function
function dsdt = NBodyCollision(t, s, n, m, L, consts)
    % Initialize derivatives
    dsdt = zeros(6*n, 1);

    % Unpack constants
    G = consts.G; % Gravitational constant
    k = consts.k; % Stiffness coefficient (N/m)
    c = consts.c; % Damping coefficient (N·s/m)

    % Unpack state variables
    x = s(1:6:end);   % x positions
    y = s(2:6:end);   % y positions
    z = s(3:6:end);   % z positions
    vx = s(4:6:end);  % x velocities
    vy = s(5:6:end);  % y velocities
    vz = s(6:6:end);  % z velocities

    % Initialize accelerations
    ax = zeros(n, 1);
    ay = zeros(n, 1);
    az = zeros(n, 1);

    % Compute pairwise forces
    for i = 1:n
        for j = 1:n
            if i ~= j
                % Displacement components and distance
                dx = x(j) - x(i);
                dy = y(j) - y(i);
                dz = z(j) - z(i);
                dist = sqrt(dx^2 + dy^2 + dz^2);

                % Overlap and relative velocity
                overlap = L(i) + L(j) - dist;
                dvx = vx(j) - vx(i);
                dvy = vy(j) - vy(i);
                dvz = vz(j) - vz(i);

                % Force functions
                Fg = @(dr, m1, m2) G * m1 * m2 * dr / max(dist^3, 1e-10);
                Fc = @(dr, vr) (overlap > 0) * (-k * overlap * dr/dist + c * vr);

                % Force components
                Fx = Fg(dx, m(i), m(j)) + Fc(dx, dvx);
                Fy = Fg(dy, m(i), m(j)) + Fc(dy, dvy);
                Fz = Fg(dz, m(i), m(j)) + Fc(dz, dvz);

                % Accumulate accelerations
                ax(i) = ax(i) + Fx / m(i);
                ay(i) = ay(i) + Fy / m(i);
                az(i) = az(i) + Fz / m(i);
            end
        end
    end

    % Pack derivatives into dsdt
    for i = 1:n
        idx = 6 * (i - 1);
        dsdt(idx + 1) = vx(i); % dx/dt
        dsdt(idx + 2) = vy(i); % dy/dt
        dsdt(idx + 3) = vz(i); % dz/dt
        dsdt(idx + 4) = ax(i); % dvx/dt
        dsdt(idx + 5) = ay(i); % dvy/dt
        dsdt(idx + 6) = az(i); % dvz/dt
    end
end

%% Custom RK4 Solver

% RK4 Varient of ODE45 Solver
function [T, S] = RK4(odefun, t_vec, s0)
    % Extract the number of steps and states
    n = length(t_vec);  % n time values (steps)
    m = length(s0);     % m states

    % Preallocate storage
    T = t_vec(:);       % Ensure T is a column vector
    S = zeros(n, m);    % Solution matrix (n steps rows x m states cols)

    % Set initial conditions
    S(1, :) = s0;

    % Perform RK4 integration over each time step
    s = s0; % Initialize state
    for i = 1:n-1 % for n-1 next time values
        % Step size for this interval
        h = T(i + 1) - T(i);

        % Compute the next step using rk4_step
        s_next = rk4_step(odefun, T(i), s, h);

        % Store results
        S(i + 1, :) = s_next; % assign cols

        % Update the state
        s = s_next;
    end

    % Nested function!
    % This is a standard 4th-order Runge-Kutta method
    function y_next = rk4_step(f, t, y, h)
        k1 = f(t, y);                                       % k1
        k2 = f(t + (h/2),     y + (h/2)*k1);                % k2
        k3 = f(t + (h/2),     y + (h/2)*k2);                % k3
        k4 = f(t + h,       y + h*k3);                      % k4
        y_next = y + h * ((k1 + (2*k2) + (2*k3) + k4)/6);   % yn+1
    end
end


%% Visualization Function
function visualize_simulation(t, s, n, L)
    % Compute global axis limits based on the entire simulation
    num_steps = length(t);
    positions = zeros(n, 3, num_steps);
    for idx = 1:num_steps
        for i = 1:n
            positions(i, 1, idx) = s(idx, 6*(i-1)+1);
            positions(i, 2, idx) = s(idx, 6*(i-1)+2);
            positions(i, 3, idx) = s(idx, 6*(i-1)+3);
        end
    end
    x_all = positions(:,1,:);
    y_all = positions(:,2,:);
    z_all = positions(:,3,:);
    xmin = min(x_all(:)) - max(L);
    xmax = max(x_all(:)) + max(L);
    ymin = min(y_all(:)) - max(L);
    ymax = max(y_all(:)) + max(L);
    zmin = min(z_all(:)) - max(L);
    zmax = max(z_all(:)) + max(L);
    
    % Visualization
    figure;
    hold on;
    colormap parula;
    colors = parula(n);
    [Xs, Ys, Zs] = sphere(20); % Create a unit sphere
    axis equal;
    xlim([xmin xmax]);
    ylim([ymin ymax]);
    zlim([zmin zmax]);
    xlabel('X');
    ylabel('Y');
    zlabel('Z');
    grid on;
    view(3);
    hold on;
    
    % Preallocate plot handles
    sphere_handles = gobjects(n,1);
    line_handles = gobjects(n,1);
    for i = 1:n
        % Initialize spheres
        sphere_handles(i) = surf([], [], [], 'FaceColor', colors(i,:), 'EdgeColor', 'k', 'FaceAlpha', 0.5);
        % Initialize pathlines
        line_handles(i) = plot3(NaN, NaN, NaN, '-.', 'Color', colors(i,:), 'LineWidth',5);
    end
    
    % Animation loop
    while(true)
        for idx = 1:num_steps
            for i = 1:n
                % Update sphere positions
                X = L(i)*Xs + positions(i,1,idx);
                Y = L(i)*Ys + positions(i,2,idx);
                Z = L(i)*Zs + positions(i,3,idx);
                set(sphere_handles(i), 'XData', X, 'YData', Y, 'ZData', Z);
                % Update pathlines
                set(line_handles(i), 'XData', squeeze(positions(i,1,1:idx)), 'YData', squeeze(positions(i,2,1:idx)), 'ZData', squeeze(positions(i,3,1:idx)));
            end
            % Set axis limits globally
            drawnow;
        end
    end
end





%% Select frames function
function [new_t, new_y] = select_frames(t, y, select_t, tol)
    % Inputs:
    % t - Original time vector (Nx1 array)
    % y - Original data matrix (NxM array, where M is the number of variables)
    % select_t - Desired time points to select (Kx1 array)
    % tol - Tolerance within which to select the time points
    
    % Outputs:
    % new_t - Filtered time vector
    % new_y - Filtered data matrix corresponding to new_t
    
    % Initialize empty arrays for the filtered results
    new_t = [];
    new_y = [];
    
    % Loop over each target time in select_t
    for i = 1:length(select_t)
        % Find indices of times in t that are within the tolerance
        idx = find(abs(t - select_t(i)) <= tol, 1);
        
        % If matches are found, append them to the new_t and new_y arrays
        if ~isempty(idx)
            new_t = [new_t; t(idx)];
            new_y = [new_y; y(idx, :)];
        end
    end
    
    % Remove duplicates and sort the results (optional, for neatness)
    [new_t, unique_idx] = unique(new_t, 'stable');
    new_y = new_y(unique_idx, :);
end
