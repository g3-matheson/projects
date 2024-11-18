import matplotlib.pyplot as mplot
import numpy as np

x1 = []
x2 = []

filename = "pyq1newton.txt"
with open(filename, "r") as file:
    current_list = x1
    for line in file:
        line = line.strip()
        if line.startswith('x'):
            _, value = line.split("=")
            current_list.append(float(value))
        elif line.startswith('!'):
            current_list = x2



def fnewton(x):
    return x - (x**3 - 5)/(3*(x**2))
def fchord(x):
    return x - (x**3 - 5)/3

fig, ax = mplot.subplots()

def drawIterations(x, f, plot_title, function_label):    
    ax.plot([x[0],x[0]], [min,x[1]], color = 'red', label = r"$x_k$")
    for i in range(1, len(x)-1):
        ax.plot([x[i-1],x[i]], [x[i], x[i]], color = 'red')
        ax.plot([x[i],x[i]], [x[i], x[i+1]], color = 'red')
    
    
    ## draw curve
    curve_x = np.linspace(min(x)-1, max(x)+1, 100)
    curve_x = curve_x[curve_x != 0] # R syntax works here apparently
    curve_y = f(curve_x)
    
    ax.plot(curve_x, curve_y, color = 'blue', label = function_label)
    ax.plot(np.linspace(min(x)-1, max(x)+1),np.linspace(min(x)-1, max(x)+1), color = 'black', label = r"$y=x$")
    
    ax.set_xlim(min(x)-1, max(x)+1)
    ax.set_ylim(min(x)-1, max(x)+1)
    ax.set_title(plot_title, fontsize = 15)
    ax.legend(loc = "lower right")
    
    mplot.show()
    
print("x1: ", x1)
print("x2: ", x2)

drawIterations(x1, fnewton, r"Newton's Method Iteration: $x_{k+1} = x_k - \frac{g(x_k)}{g^{\prime}(x_k)}$"
               , r"$f(x) = x - \frac{g(x)}{g^{\prime}(x)}$" )