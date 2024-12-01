// Included to run
#include "HW2Problems.hpp"
// Everything else
#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <algorithm>
#include <tuple>
#include <matplot/matplot.h>
#include <chrono>
#include <thread>
using namespace std;
using namespace matplot;
// Please see the explanations in the markdown, or the reference matlab file. I've tried to resemble the structure of the matlab file as much as possible

// Structure to hold constants
struct Constants 
{
    double G;
    double k;
    double c;
};

// Function prototypes allow us to call them out of order
vector<double> NBodyCollision(double t, const vector<double>& s, int n, const vector<double>& m, const vector<double>& L, const Constants& consts);
void RK4(function<vector<double>(double, const vector<double>&)> odefun, 
    const vector<double>& t_vec, const vector<double>& s0, vector<double>& T, vector<vector<double>>& S);
vector<double> rk4_step(function<vector<double>(double, const vector<double>&)> f, double t, const vector<double>& y, double h);
void select_frames(const vector<double>& t, const vector<vector<double>>& y, 
    const vector<double>& select_t, double tol, vector<double>& new_t, vector<vector<double>>& new_y);
void visualize_simulation(const vector<double>& t, const vector<vector<double>>& s, int n, const vector<double>& L);
tuple<vector<vector<double>>, vector<vector<double>>, vector<vector<double>>> sphere(int n = 20);

// ODE Function
vector<double> NBodyCollision(double t, const vector<double>& s, int n, const vector<double>& m, const vector<double>& L, const Constants& consts) 
{
    // Initialize derivatives
    vector<double> dsdt(6 * n, 0.0);

    // Unpack constants
    double G = consts.G;
    double k = consts.k;
    double c = consts.c;

    // Unpack state variables
    vector<double> x(n), y(n), z(n), vx(n), vy(n), vz(n);
    for (int i = 0; i < n; i++) 
    {
        int idx = 6 * i;
        x[i] = s[idx + 0];
        y[i] = s[idx + 1];
        z[i] = s[idx + 2];
        vx[i] = s[idx + 3];
        vy[i] = s[idx + 4];
        vz[i] = s[idx + 5];
    }

    // Initialize accelerations
    vector<double> ax(n, 0.0), ay(n, 0.0), az(n, 0.0);

    // Compute pairwise forces
    for (int i = 0; i < n; i++) 
    {
        for (int j = 0; j < n; j++) 
        {
            if (i != j) 
            {
                // Displacement components and distance
                double dx = x[j] - x[i];
                double dy = y[j] - y[i];
                double dz = z[j] - z[i];
                double dist = sqrt(dx * dx + dy * dy + dz * dz);
                // Overlap and relative velocity
                double overlap = L[i] + L[j] - dist;
                double dvx = vx[j] - vx[i];
                double dvy = vy[j] - vy[i];
                double dvz = vz[j] - vz[i];
                // Force functions (lambda functions)
                auto Fg = [&](double dr, double m1, double m2) 
                {
                    return G * m1 * m2 * dr / std::max(dist * dist * dist, 1e-10);
                };
                auto Fc = [&](double dr, double vr) 
                {
                    return (overlap > 0) ? (-k * overlap * dr / dist + c * vr) : 0.0;
                };
                // Force components
                double Fx = Fg(dx, m[i], m[j]) + Fc(dx, dvx);
                double Fy = Fg(dy, m[i], m[j]) + Fc(dy, dvy);
                double Fz = Fg(dz, m[i], m[j]) + Fc(dz, dvz);
                // Accumulate accelerations
                ax[i] += Fx / m[i];
                ay[i] += Fy / m[i];
                az[i] += Fz / m[i];
            }
        }
    }
    // Pack derivatives into dsdt
    for (int i = 0; i < n; i++) 
    {
        int idx = 6 * i;
        dsdt[idx + 0] = vx[i]; // dx/dt
        dsdt[idx + 1] = vy[i]; // dy/dt
        dsdt[idx + 2] = vz[i]; // dz/dt
        dsdt[idx + 3] = ax[i]; // dvx/dt
        dsdt[idx + 4] = ay[i]; // dvy/dt
        dsdt[idx + 5] = az[i]; // dvz/dt
    }
    return dsdt;
}

// RK4 Solver function
void RK4(function<vector<double>(double, const vector<double>&)> odefun, const vector<double>& t_vec, 
    const vector<double>& s0, vector<double>& T, vector<vector<double>>& S) 
{
    int n_steps = t_vec.size();
    int n_states = s0.size();

    T = t_vec;
    S.resize(n_steps, vector<double>(n_states, 0.0));

    vector<double> s = s0;
    S[0] = s0;

    for (int i = 0; i < n_steps - 1; i++) {
        double h = T[i + 1] - T[i];
        vector<double> s_next = rk4_step(odefun, T[i], s, h);
        S[i + 1] = s_next;
        s = s_next;
    }
}

// RK4 Method Function
vector<double> rk4_step(function<vector<double>(double, const vector<double>&)> f, double t, const vector<double>& y, double h) 
{
    vector<double> k1 = f(t, y);
    vector<double> y_temp(y.size());
    for (size_t i = 0; i < y.size(); i++) 
    {
        y_temp[i] = y[i] + h / 2.0 * k1[i];
    }
    vector<double> k2 = f(t + h / 2.0, y_temp);
    for (size_t i = 0; i < y.size(); i++) 
    {
        y_temp[i] = y[i] + h / 2.0 * k2[i];
    }
    vector<double> k3 = f(t + h / 2.0, y_temp);
    for (size_t i = 0; i < y.size(); i++) 
    {
        y_temp[i] = y[i] + h * k3[i];
    }
    vector<double> k4 = f(t + h, y_temp);

    vector<double> y_next(y.size());
    for (size_t i = 0; i < y.size(); i++) 
    {
        y_next[i] = y[i] + h * (k1[i] + 2.0 * k2[i] + 2.0 * k3[i] + k4[i]) / 6.0;
    }
    return y_next;
}

// Frame selection for final plot
void select_frames(const vector<double>& t, const vector<vector<double>>& y, const vector<double>& select_t, 
                double tol, vector<double>& new_t, vector<vector<double>>& new_y) 
{
    new_t.clear();
    new_y.clear();
    for (size_t i = 0; i < select_t.size(); i++) 
    {
        double target_t = select_t[i];
        auto it = find_if(t.begin(), t.end(), [&](double ti) { return fabs(ti - target_t) <= tol; }); // fabs is absolute value
        if (it != t.end()) 
        {
            size_t idx = distance(t.begin(), it);
            new_t.push_back(t[idx]);
            new_y.push_back(y[idx]);
        }
    }
}

void visualize_simulation(const vector<double>& t, const vector<vector<double>>& s, int n, const vector<double>& L) 
{
    size_t num_steps = t.size();
    vector<vector<vector<double>>> positions(n, vector<vector<double>>(3, vector<double>(num_steps)));

    // Compute positions
    for (size_t idx = 0; idx < num_steps; ++idx) {
        for (int i = 0; i < n; ++i) {
            positions[i][0][idx] = s[idx][6 * i + 0];
            positions[i][1][idx] = s[idx][6 * i + 1];
            positions[i][2][idx] = s[idx][6 * i + 2];
        }
    }

    // Compute global axis limits
    vector<double> x_all, y_all, z_all;
    for (int i = 0; i < n; ++i) {
        x_all.insert(x_all.end(), positions[i][0].begin(), positions[i][0].end());
        y_all.insert(y_all.end(), positions[i][1].begin(), positions[i][1].end());
        z_all.insert(z_all.end(), positions[i][2].begin(), positions[i][2].end());
    }
    double L_max = *max_element(L.begin(), L.end());
    double xmin = *min_element(x_all.begin(), x_all.end()) - L_max;
    double xmax = *max_element(x_all.begin(), x_all.end()) + L_max;
    double ymin = *min_element(y_all.begin(), y_all.end()) - L_max;
    double ymax = *max_element(y_all.begin(), y_all.end()) + L_max;
    double zmin = *min_element(z_all.begin(), z_all.end()) - L_max;
    double zmax = *max_element(z_all.begin(), z_all.end()) + L_max;

    // Visualization
    auto f = figure(true);
    f->size(800, 600);
    f->name("N Body Simulation");
    f->number_title(false);
    f->color("black");
    f->position({0, 0, 600, 600});
    f->size(1000, 1000);
    f->draw();
    f->font("Arial");
    f->font_size(12);
    f->title("N Body Simulation");

    // Get current axes
    auto ax = f->current_axes();
    ax->hold(true);
    
    // Set axis limits and labels
    ax->xlim({xmin, xmax});
    ax->ylim({ymin, ymax});
    ax->zlim({zmin, zmax});
    ax->xlabel("X");
    ax->ylabel("Y");
    ax->zlabel("Z");
    ax->grid(true);
    ax->view(45, 45); // Set a consistent view angle
    
    // Generate sphere data
    int sphereResolution = 20;
    vector<vector<double>> sphereX, sphereY, sphereZ;
    tie(sphereX, sphereY, sphereZ) = sphere(sphereResolution);

    // Animation loop
    for (int cycles = 0; cycles != 2; cycles++) // Number of times to play
    {
        for (size_t idx = 0; idx < num_steps; idx++) // Time steps
        {
            ax->clear(); // Clears the current axes for the new frame

            // Reset axis limits and labels after clearing
            ax->xlim({xmin, xmax});
            ax->ylim({ymin, ymax});
            ax->zlim({zmin, zmax});
            ax->xlabel("X");
            ax->ylabel("Y");
            ax->zlabel("Z");
            ax->grid(true);
            ax->view(45, 45); // Set a consistent view angle

            for (int i = 0; i < n; i++) // For each object
            {
                // Scale and translate sphere data
                vector<vector<double>> X(sphereResolution + 1, vector<double>(sphereResolution + 1));
                vector<vector<double>> Y(sphereResolution + 1, vector<double>(sphereResolution + 1));
                vector<vector<double>> Z(sphereResolution + 1, vector<double>(sphereResolution + 1));
                for (int m = 0; m <= sphereResolution; m++) 
                {
                    for (int j = 0; j <= sphereResolution; j++) 
                    {
                        X[m][j] = L[i] * sphereX[m][j] + positions[i][0][idx];
                        Y[m][j] = L[i] * sphereY[m][j] + positions[i][1][idx];
                        Z[m][j] = L[i] * sphereZ[m][j] + positions[i][2][idx];
                    }
                }
                
                // Plot the surface and set properties
                auto surface = ax->surf(X, Y, Z);
                surface->edge_color("none");
                surface->face_alpha(1.0); // Fully opaque

                // Plot pathlines
                if (idx > 0) {
                    vector<double> x_path(positions[i][0].begin(), positions[i][0].begin() + idx + 1);
                    vector<double> y_path(positions[i][1].begin(), positions[i][1].begin() + idx + 1);
                    vector<double> z_path(positions[i][2].begin(), positions[i][2].begin() + idx + 1);
                    auto line = ax->plot3(x_path, y_path, z_path);
                    line->line_width(2);
                }
            }
            f->draw();
            this_thread::sleep_for(chrono::milliseconds(10));
        }
    }   
}

// Sphere generation
tuple<vector<vector<double>>, vector<vector<double>>, vector<vector<double>>> sphere(int n) 
{
    vector<double> theta(n + 1), phi(n + 1);
    for (int i = 0; i <= n; i++) 
    {
        theta[i] = i * M_PI / n;
        phi[i] = i * 2 * M_PI / n;
    }
    vector<vector<double>> X(n + 1, vector<double>(n + 1));
    vector<vector<double>> Y(n + 1, vector<double>(n + 1));
    vector<vector<double>> Z(n + 1, vector<double>(n + 1));
    for (int i = 0; i <= n; i++) 
    {
        for (int j = 0; j <= n; j++) 
        {
            X[i][j] = sin(theta[i]) * cos(phi[j]);
            Y[i][j] = sin(theta[i]) * sin(phi[j]);
            Z[i][j] = cos(theta[i]);
        }
    }
    return make_tuple(X, Y, Z);
};

// Run problem 4
void runProblem4() 
{
    cout << "################ Question 4.1 ###################" << endl;
    // Initialize variables
    int n = 3;
    vector<double> m = {3, 1, 0.5};
    vector<double> L = {0.5, 0.5, 0.2};
    vector<vector<double>> r0 = {{0, 0, 0}, {10, 1, 0}, {0, 0, 10}};
    vector<vector<double>> v0 = {{1, 0, 0}, {-1, 0, 0}, {0, 0, 0}};
    Constants consts;
    consts.G = 6;
    consts.k = 3000;
    consts.c = 2;
    double t0 = 0.0;
    double tf = 20.0;
    double dt = 0.001;
    double tol = 0.01;

    // Initialize s0
    cout << "Generating initial states..." << endl;
    vector<double> s0(6 * n, 0.0);
    for (int i = 0; i < n; i++) 
    {
        int idx = 6 * i;
        s0[idx + 0] = r0[i][0];
        s0[idx + 1] = r0[i][1];
        s0[idx + 2] = r0[i][2];
        s0[idx + 3] = v0[i][0];
        s0[idx + 4] = v0[i][1];
        s0[idx + 5] = v0[i][2];
    }

    // Generate tspan
    vector<double> tspan;
    for (double t = t0; t <= tf; t += dt) 
    {
        tspan.push_back(t);
    }

    // Call RK4 solver
    cout << "Running RK4..." << endl;
    vector<double> T;
    vector<vector<double>> S;
    RK4([&](double t, const vector<double>& s) { return NBodyCollision(t, s, n, m, L, consts); }, tspan, s0, T, S);

    // Select Frames for visualization
    cout << "Selecting frames..." << endl;
    vector<double> select_t;
    for (double t = t0; t <= tf; t += 0.1) 
    {
        select_t.push_back(t);
    }
    vector<double> new_t;
    vector<vector<double>> new_s;
    select_frames(T, S, select_t, tol, new_t, new_s);

    // Visualization
    cout << "Animating Results" << endl;
    visualize_simulation(new_t, new_s, n, L);
    cout << "################ End of Question 4.1 ###################" << endl;
}