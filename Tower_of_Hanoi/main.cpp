#include "raylib.h"
#include <string>
#include <array>
#include <vector>
#include <iostream>
#include <algorithm>
#include <utility>

const Color rodColour = BEIGE;
const Color baseColour = DARKBROWN;
const Color diskColour = RED;
const Color selectedDiskColour = GREEN;

const std::string screenshotSaveDirectory = "C:\\Users\\epant\\Pictures\\Tower Of Hanoi Pics";

const unsigned int numberOfRings = 30;
unsigned int moveNumber = 0;

enum ROD
{
    LEFT,
    MIDDLE,
    RIGHT
};

bool operator==(const Color& a, const Color& b) {
    if ((a.r==b.r)&&(a.g==b.g)&&(a.b==b.b)&&(a.a==b.a))
    {
        return true;
    }
    else {
        return false;
    }
}

//Game functions
void drawRod(int x, int y, int width, int height, int baseDepth, int rodDiameter);
bool drawDisks(int width, int height, int rodDiameter, int rodY, std::array<int, 3> rodXs, std::array<unsigned int, numberOfRings> disks, int diskHeight, int selectedDisk);
std::array<unsigned int, numberOfRings> moveDisk(int from, int to, std::array<unsigned int, numberOfRings> disks);
std::array<Rectangle, 3> generateBoundingBoxes(std::array<int, 3> rodXs, int rodY, int rodWidth, int rodHeight);
int findSelectedDisk(int rod, std::array<unsigned int, numberOfRings> disks);

//Ai functions
void moveStack(char from, char to, char auxiliary, unsigned int size, std::vector<std::pair<char, char>>* moves);

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;
    int textOffset = 10;
    int moveStringLength;
    bool errorHasOccured = false;

    int rodWidth = screenWidth/5;
    int rodHeight = screenHeight/2;
    int baseDepth = screenHeight/24;
    int rodDiameter = screenWidth/70;
    int rodY = screenHeight/4;
    std::array<int, 3> rodXs = {screenWidth/4-rodWidth/2, screenWidth/2-rodWidth/2, 3*screenWidth/4-rodWidth/2};
    std::array<Rectangle, 3> rodBoundingBoxes = generateBoundingBoxes(rodXs, rodY, rodWidth, rodHeight);

    std::array<int, 2> moves = {-1, -1};
    int move = -1;
    int selectedDisk = -1;
    bool hasMoveBeenMade = false;

    //ring number-1, rod
    std::array<unsigned int, numberOfRings> disks;
    disks.fill(ROD::LEFT);
    
    //Screenshot stuff
    std::string screenshotFileName;
    if (!ChangeDirectory(screenshotSaveDirectory.c_str()))
    {
        std::cout << "Screenshot directory is not available.\n";
        //errorHasOccured = true;
    }

    //Solution Stuff
    std::vector<std::pair<char, char>>* solution = new std::vector<std::pair<char, char>>();
    moveStack(ROD::LEFT, ROD::RIGHT, ROD::MIDDLE, 1, solution);
    std::string solutionString = "n(rings),n(moves)\n";
    for (int i = 1; i <= numberOfRings; i++)
    {
        delete solution;
        solution = new std::vector<std::pair<char, char>>();
        moveStack(ROD::LEFT, ROD::RIGHT, ROD::MIDDLE, i, solution);
        solutionString += std::to_string(i) + "," + std::to_string(solution->size()) + "\n";
        std::cout << solutionString;
    }
    SaveFileText("data.csv", strdup(solutionString.c_str()));

    InitWindow(screenWidth, screenHeight, "Tower Of Hanoi");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose() || errorHasOccured)    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------
        hasMoveBeenMade = false;

        //Test for mouse clicks
        if (IsMouseButtonPressed(0))
        {
            if (CheckCollisionPointRec(GetMousePosition(), rodBoundingBoxes[ROD::LEFT]))
            {
                move = ROD::LEFT;
                hasMoveBeenMade = true;
            }
            else if (CheckCollisionPointRec(GetMousePosition(), rodBoundingBoxes[ROD::MIDDLE])) {
                move = ROD::MIDDLE;
                hasMoveBeenMade = true;
            }
            else if (CheckCollisionPointRec(GetMousePosition(), rodBoundingBoxes[ROD::RIGHT])) {
                move = ROD::RIGHT;
                hasMoveBeenMade = true;
            }
            
            //Log the move
            if (hasMoveBeenMade)
            {
                if (moves[0] == -1)
                {
                    moves[0] = move;
                    selectedDisk = findSelectedDisk(move, disks);
                }
                else if (moves[1] == -1) {
                    moves[1] = move;
                    disks = moveDisk(moves[0], moves[1], disks);
                    moves = {-1, -1};
                    selectedDisk = -1;
                }
                else {
                    std::cout << "A move error has occurred.\n";
                    errorHasOccured = true;
                }
            }
        }

        //Check for keyboard
        //Check for "r" for reset
        if (IsKeyPressed(82))
        {
            moves = { -1, -1 };
            move = -1;
            selectedDisk = -1;
            moveNumber = 0;
            hasMoveBeenMade = false;
            disks.fill(ROD::LEFT);
        }

        

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(WHITE);

        //Draw the counter for the number of rings
        DrawText((std::string("Number of Rings: ") + std::to_string(numberOfRings)).c_str() , textOffset, textOffset, 40, BLACK);

        //Draw the counter for the number of moves in the top right
        moveStringLength = MeasureText((std::string("Number of Moves: ") + std::to_string(moveNumber)).c_str(), 40);
        DrawText((std::string("Number of Moves: ") + std::to_string(moveNumber)).c_str(), screenWidth - moveStringLength - textOffset, textOffset, 40, BLACK);

        //Draw the three rods
        for (int i = 0; i < 3; i++)
        {
            drawRod(rodXs[i], rodY, rodWidth, rodHeight, baseDepth, rodDiameter);
        }
        
        //Draw the disks
        if (!drawDisks(rodWidth, rodHeight - baseDepth, rodDiameter, rodY, rodXs, disks, (rodHeight - baseDepth) / (numberOfRings + 1), selectedDisk)) {
            errorHasOccured = true;
        }

        EndDrawing();
        //----------------------------------------------------------------------------------

        if (moveNumber < solution->size())
        {
            disks = moveDisk(((*solution)[moveNumber]).first, ((*solution)[moveNumber]).second, disks);
        }
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void drawRod(int x, int y, int width, int height, int baseDepth, int rodDiameter)
{
    //Draw the main rod
    DrawRectangle(x+(width/2)-(rodDiameter/2), y, rodDiameter, height-baseDepth, rodColour);
    //Draw the base
    DrawRectangle(x, y + height - baseDepth, width, baseDepth, baseColour);
}

bool drawDisks(int width, int height, int rodDiameter, int rodY, std::array<int, 3> rodXs, std::array<unsigned int, numberOfRings> disks, int diskHeight, int selectedDisk)
{
    //Initialise the variables to hold the number of disks in each column
    int lDisks = 0;
    int mDisks = 0;
    int rDisks = 0;
    int lDisksNum = std::count(disks.begin(), disks.end(), ROD::LEFT);
    int mDisksNum = std::count(disks.begin(), disks.end(), ROD::MIDDLE);
    int rDisksNum = std::count(disks.begin(), disks.end(), ROD::RIGHT);
    int diskWidth;
    Color tokenColour;

    for (int disk = 0; disk < numberOfRings; disk++)
    {
        //setting disk specific variables
        diskWidth = rodDiameter + (width - rodDiameter) * (disk + 1) / (numberOfRings + 1);
        if (disk == selectedDisk) {
            tokenColour = selectedDiskColour;
        }
        else {
            tokenColour = diskColour;
        }

        //Drawing the disks
        switch (disks[disk]) {
        case ROD::LEFT:
            DrawRectangle(rodXs[ROD::LEFT] + (width / 2) - (diskWidth / 2), rodY + height - (lDisksNum - lDisks) * diskHeight, diskWidth, diskHeight, tokenColour);
            lDisks++;
            break;
        case ROD::MIDDLE:
            DrawRectangle(rodXs[ROD::MIDDLE] + (width / 2) - (diskWidth / 2), rodY + height - (mDisksNum - mDisks) * diskHeight, diskWidth, diskHeight, tokenColour);
            mDisks++;
            break;
        case ROD::RIGHT:
            DrawRectangle(rodXs[ROD::RIGHT] + (width / 2) - (diskWidth /2), rodY + height - (rDisksNum - rDisks) * diskHeight, diskWidth, diskHeight, tokenColour);
            rDisks++;
            break;
        default:
            std::cout << "There is an error with disk storage.\n";
            return false;
        }
    }

    return true;
}

std::array<unsigned int, numberOfRings> moveDisk(int from, int to, std::array<unsigned int, numberOfRings> disks)
{
    int topFromDisk = 0;
    while ((disks[topFromDisk] != from))
    {
        topFromDisk++;
        if (topFromDisk >= numberOfRings) {
            return disks;
        }
    }

    int topToDisk = numberOfRings;
    for (int disk = numberOfRings - 1; disk >= 0; disk--)
    {
        if (disks[disk] == to)
        {
            topToDisk = disk;
        }
    }

    if (topToDisk > topFromDisk)
    {
        disks[topFromDisk] = to;
        moveNumber++;
    }
    
    return disks;
}

std::array<Rectangle, 3> generateBoundingBoxes(std::array<int, 3> rodXs, int rodY, int rodWidth, int rodHeight)
{
    std::array<Rectangle, 3> boundingBoxes;
    for (int i = 0; i < 3; i++)
    {
        boundingBoxes[i].x = rodXs[i];
        boundingBoxes[i].y = rodY;
        boundingBoxes[i].width = rodWidth;
        boundingBoxes[i].height = rodHeight;
    }
    return boundingBoxes;
}

int findSelectedDisk(int rod, std::array<unsigned int, numberOfRings> disks)
{
    int disk = -1;
    for (int i = numberOfRings - 1; i >= 0; i--)
    {
        if (disks[i] == rod)
        {
            disk = i;
        }
    }
    return disk;
}

void moveStack(char from, char to, char auxiliary, unsigned int size, std::vector<std::pair<char, char>>* moves)
{
    if (size < 2)
    {
        moves->push_back({ from, to });
    }
    else
    {
        //Move all but the bottom to the middle
        moveStack(from, auxiliary, to, size - 1, moves);
        //move the bottom to the right
        moves->push_back({ from, to });
        //move the middle on top of it
        moveStack(auxiliary, to, from, size - 1, moves);
    }
}
