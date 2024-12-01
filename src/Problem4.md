# Problem 4
For problem 04, we've developed an N-Body simulator with soft collisions. 

## Background

The original code, which was made in MATLAB, was developed by Melik Demirel. The `NBodySim.m` file, found in `/references/` is the original code for this program. 

## The Mathematical Theory
In essence, problem 4 simulates the N-Body Problem, as seen in Aerospace 450 or on [Wikipedia](https://en.wikipedia.org/wiki/N-body_problem), in addition to Hooke's Law with damping (to handle acceleration during collisions, as will be discussed). Furthermore, the physics is formulated as follows...

### Basic Definitions

We define two masses or objects with the subscripts $i$ and $j$, where $i \ne j$, with masses $m_i$ and $m_j$.

We can define a displacement vector between the position vectors of these vectors as:

$$\vec{\Delta r} = \vec{r}_j - \vec{r}_i$$

It is important to note here that each position vector is composed of 3 input state variables ($r_x$, $r_y$, $r_z$) for each object.
From this, we can define the "object against another object" direction (displacement normal vector) as:

$$\hat{\Delta r} = \frac{\vec{\Delta r}}{\left\|\vec{\Delta r}\right\|}$$

Similarly, for relative velocity:

$$\vec{\Delta v} = \vec{v}_j - \vec{v}_i$$

It is also important to note here that each velocity vector is composed of 3 input state variables ($v_x$, $v_y$, $v_z$) for each object.
Finally, we can define the overlap between any two masses, given their corresponding radii $L$:

$$q = L_j + L_i - \left\|\vec{\Delta r}\right\|$$

### The Gravity Force:
Based on Newton's law of Gravitation, we have the following gravity force, defined for two masses: 

$$\vec{F_{g,ij}} = \frac{G m_i m_j \hat{\Delta r}}{\left\|\vec{\Delta r}\right\|^2}$$

In the code, we use the maximum function in the denominator to prevent divide by zero.

### Hooke's Law with Damping:
Based on Hooke's law for a spring with stiffness constant $k$, alongside some linear damping with damping constant $c$, we have the following contact force defined for two masses:

$$\vec{F_{c,ij}} = f(q) \cdot \left( - k q \hat{\Delta r} + c \vec{\Delta v} \right) $$

Where $f(q)$ is an activation function that dictates overlap (collision) between two objects based on the value of $q$, given by:

$$
f(q) = \begin{cases} 
1, & q > 0 \\ 
0, & q \leq 0
\end{cases}
$$

### Momentum Equation: Newton's Second Law
Putting it all together, for each object $i$, we have 3 acceleration output state variables ($a_x$, $a_y$, $a_z$) that make up the acceleration vector:

$$\vec{a_i} =  \frac{1}{m_i} \cdot \sum_{j=1 \atop j \ne i}^n \vec{F_{g,ij}} + \vec{F_{c,ij}}$$

### System of First Order ODEs
To describe the system as a set of first order odes (to satisfy the general ODE of states seen in the RK4 numerical scheme described below), we must write the momentum equations as follows (decomposing the second order ODEs to twice the number of first order ODES, and decomposing each vector into its components), for each mass $i$:

$$
\begin{cases} 
\frac{d}{dt} v_{x,i} = \frac{1}{m_i} \cdot \sum_{j=1 \atop j \ne i}^n F_{gx,ij} + F_{cx,ij} \\ 
\frac{d}{dt} v_{y,i} = \frac{1}{m_i} \cdot \sum_{j=1 \atop j \ne i}^n F_{gy,ij} + F_{cy,ij} \\ 
\frac{d}{dt} v_{z,i} = \frac{1}{m_i} \cdot \sum_{j=1 \atop j \ne i}^n F_{gz,ij} + F_{cz,ij} \\ 
\frac{d}{dt} r_{x,i} = v_{x,i} \\
\frac{d}{dt} r_{y,i} = v_{y,i} \\
\frac{d}{dt} r_{z,i} = v_{z,i} 
\end{cases}
$$

## Numerical RK4 Solver Scheme
To solve the coupled set of ODEs for every mass at every point in time, a custom RK4 solver was also developed by Melik Demirel. In terms of in-line usage within MATLAB, the RK4 solver was designed to resemble MATLAB's own ode45 function. 

The solver solves 6 states ($r_x$, $r_y$, $r_z$ position and $v_x$, $v_y$, $v_z$ velocity) for each mass. The numerical Euler scheme solves the general ODE of the form:

$$\frac{ds}{dt} = f(t,s)$$

Where $s$ is the set of states, $t$ is time, and $f(t,s)$ is a ODE function that governs the system.

Given $N$ masses, the set of states becomes an array that is of $6N$ length, where there are 6 states for each mass.

The ODE function $f(t,s)$ essentially intakes the $6N$ states $s$ ($r_x$, $r_y$, $r_z$ position and $v_x$, $v_y$, $v_z$ velocity for each mass), and uses the mathematical theory described earlier to find the $6N$ derivative states $ds/dt$ ($v_x$, $v_y$, $v_z$ velocity and $a_x$, $a_y$, $a_z$ acceleration for each mass).

To solve the scheme, the Runge-Kutta method of order 4 is employed. Given a time array $t$ of length $M$ that contains every time value $t_m$ for every point in time, starting from the given initial state array $s_0$ at time $t = 0$ seconds such that subscript $m = 0$ (the initial conditions), and given the ODE function $f(t,s)$ we have:

$$s_{m+1} = f_{\mathrm{RK4}}(f(t,s),s_m,t_m,t_{m+1})$$

The RK4 function, $f_{\mathrm{RK4}}(f(t,s),s_m,t_m,t_{m+1})$, for every state at a certain point in time $s=s_m$, is defined as:

$$h = t_{m+1} - t_m$$
$$k_1 = f(t, s_m)$$
$$k_2 = f(t+\frac{h}{2}, s_m + \frac{h}{2} \cdot k_1)$$
$$k_3 = f(t + \frac{h}{2}, s_m + \frac{h}{2} \cdot k_2)$$
$$k_4 = f(t + h, s_m + h \cdot k_3)$$
$$f_{\mathrm{RK4}}(f(t,s),s_m,t_m,t_{m+1}) = s_m + h \cdot \frac{k_1 + 2 k_2 + 2 k_3 + k_4}{6}$$

The resulting solution matrix containing $s$ at every point in time is thus of size $M \times N$ ($\text{rows of time} \times \text{columns of states}$).

