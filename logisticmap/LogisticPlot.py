import sys
import matplotlib.pyplot as mplot
import numpy as np
import os

x = [[]]
cvalues = []
fig, ax = mplot.subplots()

def logistic(c,x):
    return c*x*(1-x)

def createPlot(plotData, c, plot_title, function_label, plot_filename):
    global fig, ax
    
    ax.plot([plotData[0],plotData[0]], [0,plotData[1]], color = 'red', label = r"$x_k$")
    for i in range(1, len(x)-3):
        ax.plot([plotData[i-1],plotData[i]], [plotData[i], plotData[i]], color = 'red', linewidth = 0.35)
        ax.plot([plotData[i],plotData[i]], [plotData[i], plotData[i+1]], color = 'red', linewidth = 0.35)
    
    ## draw curve
    curve_x = np.linspace(min(plotData)-1, max(plotData)+1, 100)
    curve_x = curve_x[curve_x != 0] # R syntax works here apparently
    curve_y = logistic(c, curve_x)
    
    ax.plot(curve_x, curve_y, color = 'blue', label = function_label)
    ax.plot(np.linspace(min(plotData)-1, max(plotData)+1),np.linspace(min(plotData)-1, max(plotData)+1), color = 'black', label = r"$y=x$")
    
    ax.set_xlim(0, 1.1)
    ax.set_ylim(0, 1.1)
    ax.set_title(plot_title, fontsize = 15)
    ax.legend(loc = "lower right")
    
    mplot.savefig(plot_filename, dpi = 100, bbox_inches='tight')

def loadFile(filename):
    global x, cvalues

    with open(filename, "r") as file:
        counter = 0
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

def main(bucketID):
    global fig, ax, x, cvalues
    
    loadFile(f"Logistic_{bucketID}.txt")
    
    for i in range(1, len(cvalues)-1):
        c = cvalues[i]
        
        createPlot(x[i], c, r"$c=$"+f"{c}", r"$f(x) = cx(1-x)$", f"./plots/logistic_{bucketID}_{i}.png")
        
        ax.clear()
        mplot.clf()
        fig, ax = mplot.subplots()
    
# calls main(n) from command-line: "python LogisticPlot.py n"
if(__name__ == "__main__"):
    bucketID = int(sys.argv[1])
    print(f"Processing Bucket # {bucketID+1}")
    main(bucketID)