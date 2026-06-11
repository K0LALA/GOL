# What?

This program is a simulation for Conway's Game of Life in C using SDL3 for the graphical interface.<br>
It has a theoretically infinite grid by using a sparse encoding method as [javidx9](https://www.youtube.com/@javidx9) described in his video:

[![Video Title](https://img.youtube.com/vi/OqfHIujOvnE/0.jpg)](https://www.youtube.com/watch?v=OqfHIujOvnE)

# How to use

## Launch

Call `./gol` or `./gol-static` depending on the version you have.<br>
Please note you may need to allow the execution of either file for this to work.

### Random cells

By default, the window will have a 500x500 square randomly filled by around 7%.<br>
You can change the size of the starting square by specifying two arguments, like so: `./gol WIDTH HEIGHT`.

### Patterns

Another way to launch is to load patterns using the RLE format as described [here](https://conwaylife.com/wiki/Run_Length_Encoded).

>[!NOTE]
> There are example patterns in the `patterns` folder of this current repository.

>[!WARNING]
> You can find more patterns at [Game of Life Wiki](https://conwaylife.com/wiki/Category:Patterns)
> Please note however that you need to comment all lines starting with something like *x = ...* or *title=...*.
> Otherwise you will probably get strange results.

When you have your pattern, call `./gol path/to/pattern.rle` to simulate.<br>
If you want to add an offset for the pattern, either positive or negative, add it after the pattern's path: `./gol path/to/pattern.rle OFFSET_X OFFSET_Y`

## Run

By default, the simulation doesn't start by itself.<br>
So you need to start it manually using either of these 2 methods:

1. Press `Space` and the simulation will run every frame, or at least as fast as it can. If you press `Space` again, the program will pause the simulation until you press `Space` again.
2. Press `S` and the simulation will iterate once.

If you want to stop, press `Escape` or the close the window.<br>
You can resize the window even when paused and it will render the current state.
