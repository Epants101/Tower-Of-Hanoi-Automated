# Tower-Of-Hanoi-Automated
An automated version of the Tower of Hanoi where the computer solves it.

BUILD INSTRUCTIONS - 
If you are on Windows, a prebuilt binary is present in the root directory of the repository.
Otherwise, premake must be used to build the project. On Windows, run premake.bat, then build the visual studio project.
On another platform, the build must be done manually through the command line.

USAGE - 
When the program is launched, it will solve the Tower of Hanoi for each number of rings up to the maximum, which is hardwired into the code. 
A .csv file containing the results is saved to the screenshot directory.
Then the solution for the maximum number of rings will be played by the program.
The r key can be used to reset the puzzle. 
The s key can be used to save screenshots, however, the directory it saves to is hardwired into the code.
To change any setting, the code needs to be recompiled.

WARNING: This program uses a lot of ram initially, it could crash if your computer lacks ram.