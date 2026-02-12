# Graph
C single file library for generating SVG chart from input data file.

## Usage
To use Graph library in your project just download .h file and include it in your project like shown bellow.

```
#define GRAPHS_IMPLEMENTATION
#include "graphs.h"
```

The **GRAPHS_IMPLEMENTATION** macro is very important! If macro is not included Graph won't work at all or won't work correctly!

After include you just have to call graphs() function. It accept one parameter, which is path to input data file.

```
graphs("data.txt"); //will try to find data.txt file
```

## Input file format
Graph accepts text file in folowing format:

```
-1
0
1
-5
5
```

Basicly it accepts INTs, which are separated by new line.

## Roadmap
- v1.1 | [11. Feb 2026]: simple version of graph (accepts only INTs, include bugs, simple graph grid)
    - v1.1.1 | [12. Feb 2026]: small bugs fix (fclose added, LineCords UB)

- version 2: simple graph generator with support of decimal values
- version 3: richer labels, axis, ...

## Well known bugs
- int_parse() errors
- replace exit(1) with return



###### Created by warezoid with the love of freedom and numbers. 🧡