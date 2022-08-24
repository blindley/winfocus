# winfocus
A Windows program that sets the foreground window based on various parameters.

## Visual Prominence mode
This is the default (and currently only) mode. First the program enumerates all visible windwos on the system.
The visible area of each window is then calculated. Windows which don't pass certain thresholds for area, as well as width and height individually,
are discarded from the working set. The remaining windows are sorted by their left-most point, then labeled numerically starting at 0.
The first argument in the command line parameters should be an integer, indicating which window to set the focus to, based on the algorithm above.

### Example
In my current setup, I have 2 monitors. The monitor on the left has 1 maximized chrome window. The monitor on the right has another chrome window
filling the left half of the screen, and my code editor filling the right half of the screen. The only other visually prominent windows on my screen
are the taskbars on each monitor, however, they do not meet the height threshold, and so are not part of the final set. So when I run the following
comand

    > winfocus 1
    
The chrome window on my left monitor gets the label 0. The chrome monitor on the left side of my right monitor gets the label 1. And the code editor
on the right side of my right monitor gets the label 2. And since I passed a 1 to the program, the second chrome window gets the focus.
