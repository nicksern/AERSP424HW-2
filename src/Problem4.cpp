#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <matplot/matplot.h>

int main() {
    using namespace std;
    using namespace matplot;

    // Air properties (Standard Temperature and Pressure Conditions)
    double Uinf = 1.0;     // Freestream wind speed [m/s]
    double mu = 1.8e-5;    // Dynamic viscosity [kg/m-s]
    double rho = 1.225;    // Density [kg/m^3]
    double nu = mu / rho;  // Kinematic viscosity [m^2/s]

    // Set up the domain
    double L = 0.1;         // Plate length (10 cm) [m]
    double H = L / 10;      // Arbitrarily define domain height [m]
    int ix = 1000;          // Number of grid points in the x-direction
    int jx = 500;           // Number of grid points in the y-direction

    // Sub-divide the domain into blocks
    double dx = L / (ix - 1);
    double dy = H / (jx - 1);

    // x- and y-points in the domain
    vector<double> x(ix);
    vector<double> y(jx);
    for (int i = 0; i < ix; ++i) {
        x[i] = i * dx;
    }
    for (int j = 0; j < jx; ++j) {
        y[j] = j * dy;
    }

    // Initialize velocity matrices and apply initial conditions
    vector<vector<double>> u_n(ix, vector<double>(jx, Uinf));     // u(y) at current time step, n
    vector<vector<double>> u_np1(ix, vector<double>(jx, Uinf));   // u(y) at next time step, n+1
    vector<vector<double>> v_n(ix, vector<double>(jx, 0.0));      // v(y) at current time step, n
    vector<vector<double>> v_np1(ix, vector<double>(jx, 0.0));    // v(y) at next time step, n+1

    // Apply the boundary conditions
    for (int i = 0; i < ix; ++i) {
        u_n[i][0] = 0.0;
        u_np1[i][0] = 0.0;
    }

    // Main computation loop
    double maxresidual = 1e-06;
    double converged = 1.0;
    int n = 0;  // Iteration counter
    vector<double> residual;

    auto tic = chrono::steady_clock::now();
    while (converged >= maxresidual) {
        n += 1;
        // Compute u at the next time step
        for (int i = 0; i < ix - 1; ++i) {
            for (int j = 1; j < jx - 1; ++j) {
                // Compute alpha and beta
                double alpha = nu * dx / (u_n[i][j] * dy * dy);
                double beta = (v_n[i][j] * dx) / (2.0 * u_n[i][j] * dy);

                // Update u_np1
                u_np1[i + 1][j] = (1.0 / (1.0 + 2.0 * alpha)) * (u_n[i][j]
                                     + alpha * (u_n[i + 1][j + 1] + u_n[i + 1][j - 1])
                                     - beta * (u_n[i][j + 1] - u_n[i][j - 1]));
            }
        }

        // Compute v at the next time step
        for (int i = 0; i < ix - 1; ++i) {
            for (int j = 1; j < jx - 1; ++j) {
                v_np1[i + 1][j] = v_n[i + 1][j - 1] - (dy / (2.0 * dx)) *
                                  (u_np1[i + 1][j] - u_np1[i][j] + u_np1[i + 1][j - 1] - u_np1[i][j - 1]);
            }
        }

        // Compute residual
        double max_diff = 0.0;
        for (int i = 0; i < ix; ++i) {
            for (int j = 0; j < jx; ++j) {
                double diff = fabs(u_n[i][j] - u_np1[i][j]);
                if (diff > max_diff) {
                    max_diff = diff;
                }
            }
        }
        converged = max_diff;
        residual.push_back(converged);

        // Update u and v for the next iteration
        u_n = u_np1;
        v_n = v_np1;
    }
    auto toc = chrono::steady_clock::now();
    chrono::duration<double> elapsed_seconds = toc - tic;
    cout << "Computation Time: " << elapsed_seconds.count() << " seconds" << endl;

    // Prepare data for plotting
    vector<vector<double>> u_transposed(jx, vector<double>(ix));
    for (int i = 0; i < ix; ++i) {
        for (int j = 0; j < jx; ++j) {
            u_transposed[j][i] = u_n[i][j];
        }
    }

    // Plot the solution
    auto fig = figure(true);
    imagesc(x, y, u_transposed);
    gca()->y_dir(axis::normal);
    colormap(palette::jet());
    colorbar();
    xlabel("x [m]");
    ylabel("y [m]");
    auto cb = gcf()->current_colorbar();
    cb->label("u(y) [m/s]");
    show();

    // Compute the boundary layer height, delta, along the length of the plate
    tic = chrono::steady_clock::now();
    vector<double> delta(ix, 0.0);
    for (int i = 0; i < ix; ++i) {
        int jDelta = 0;
        for (int j = 0; j < jx; ++j) {
            double utemp = u_n[i][j];
            if (utemp <= 0.99 * Uinf) {
                jDelta = j;
            }
        }
        delta[i] = y[jDelta];
    }
    toc = chrono::steady_clock::now();
    elapsed_seconds = toc - tic;
    cout << "Boundary Layer Height Computation Time: " << elapsed_seconds.count() << " seconds" << endl;

    return 0;
}
