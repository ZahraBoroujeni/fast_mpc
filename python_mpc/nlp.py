import cvxpy as cvx

# Create two scalar optimization variables.
F = cvx.Variable()
s = cvx.Variable()
x = cvx.Variable()
y = cvx.Variable()
v = cvx.Variable()
theta = cvx.Variable()
# Create two constraints.
constraints = [ cvx.square(x) + cvx.square(y)>= 9,
        		cvx.square(x +2) + cvx.square(y -2.5)>=1 ,
              	cvx.square(x +2) + cvx.square(y -2.5) <= 0.9025]

# Form objective.
obj = cvx.Minimize((-100*y + 0.1*cvx.square(F) + 0.01* cvx.square(s)))

# Form and solve problem.
prob = cvx.Problem(obj, constraints)
prob.solve()  # Returns the optimal value.
# x = v * cos ( theta )*h+x; 
#             y=v*sin(theta)*h+y; 
#             v=(F / m)*h+v; 
#             theta=(s/I)*h+theta;
print ("status:", prob.status)
print ("optimal value", prob.value)
print ("optimal var", x.value, y.value)