import matplotlib.pyplot as mplot
import numpy as np

def logistic(c,x):
    return c*x*(1-x)

def drawIterations(x, c, plot_title, function_label, plot_filename):
    ax.plot([x[0],x[0]], [0,x[1]], color = 'red', label = r"$x_k$")
    for i in range(1, len(x)-1):
        ax.plot([x[i-1],x[i]], [x[i], x[i]], color = 'red', linewidth = 0.1)
        ax.plot([x[i],x[i]], [x[i], x[i+1]], color = 'red', linewidth = 0.1)
    
    ## draw curve
    curve_x = np.linspace(min(x)-1, max(x)+1, 100)
    curve_x = curve_x[curve_x != 0] # R syntax works here apparently
    curve_y = logistic(c, curve_x)
    
    ax.plot(curve_x, curve_y, color = 'blue', label = function_label)
    ax.plot(np.linspace(min(x)-1, max(x)+1),np.linspace(min(x)-1, max(x)+1), color = 'black', label = r"$y=x$")
    
    ax.set_xlim(0, 1.1)
    ax.set_ylim(0, 1.1)
    ax.set_title(plot_title, fontsize = 15)
    ax.legend(loc = "lower right")
    
    mplot.savefig(plot_filename, dpi = 300, bbox_inches='tight')

filename = "pyq3.txt"
x = [[]]
cvalues = []
with open(filename, "r") as file:
    counter = 0
    x.append([])
    current_list = x[counter]
    for line in file:
        line = line.strip()
        if line.startswith('x'):
            _, value = line.split("=")
            current_list.append(float(value))
        elif line.startswith('c'):
            _, cvalue = line.split("=")
            cvalues.append(float(cvalue))
        elif line.startswith('!'):
            counter += 1
            x.append([])
            current_list = x[counter]

for i in range(0, len(cvalues)):
    print("cvalues[i] = ", cvalues[i])
    c = cvalues[i]
    fig, ax = mplot.subplots()
    
    drawIterations(x[i], c, r"$c=$"+f"{c}", r"$f(x) = cx(1-x)$", f"logistic_{i}.png")
    
    ax.clear()
    mplot.clf()