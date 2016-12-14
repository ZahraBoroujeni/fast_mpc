from cvxpy import *
import numpy
import matplotlib.pyplot as plt

# Problem data.
n = 15
m = 10
numpy.random.seed(1)
A = numpy.random.randn(n, m)
b = numpy.random.randn(n, 1)
# gamma must be positive due to DCP rules.
gamma = Parameter(sign="positive")

# Construct the problem.
x = Variable(m)
error = sum_squares(A*x - b)
obj = Minimize(error + gamma*norm(x, 1))
prob = Problem(obj)

# Construct a trade-off curve of ||Ax-b||^2 vs. ||x||_1
sq_penalty = []
l1_penalty = []
x_values = []
gamma_vals = numpy.logspace(-4, 6)
for val in gamma_vals:
    gamma.value = val
    prob.solve()
    # Use expr.value to get the numerical value of
    # an expression in the problem.
    sq_penalty.append(error.value)
    l1_penalty.append(norm(x, 1).value)
    x_values.append(x.value)

plt.rc('text', usetex=True)
plt.rc('font', family='serif')
plt.figure(figsize=(6,10))

# Plot trade-off curve.
plt.subplot(211)
plt.plot(l1_penalty, sq_penalty)
plt.xlabel(r'\|x\|_1', fontsize=16)
plt.ylabel(r'\|Ax-b\|^2', fontsize=16)
plt.title('Trade-Off Curve for LASSO', fontsize=16)

# Plot entries of x vs. gamma.
plt.subplot(212)
for i in range(m):
    plt.plot(gamma_vals, [xi[i,0] for xi in x_values])
plt.xlabel(r'\gamma', fontsize=16)
plt.ylabel(r'x_{i}', fontsize=16)
plt.xscale('log')
plt.title(r'\text{Entries of x vs. }\gamma', fontsize=16)

# plt.tight_layout()
plt.show()