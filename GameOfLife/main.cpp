// OneLoneCoder's olcPixelGameEngine
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

// CUDA code
#include "kernel.cuh"

// cpp libs
#include <iostream>


class GameOfLife : public olc::PixelGameEngine
{
public:
    GameOfLife()
    {
        // Name of the application
        sAppName = "GameOfLife by Ediu3095";
    }

public:
    bool OnUserCreate() override
    {
        // Allocate memory for both grids
        grid[0] = (bool*)malloc(ScreenWidth() * ScreenHeight() * sizeof(bool));
        grid[1] = (bool*)malloc(ScreenWidth() * ScreenHeight() * sizeof(bool));

        memset(grid[0], 0, ScreenWidth() * ScreenHeight() * sizeof(bool));
        memset(grid[1], 0, ScreenWidth() * ScreenHeight() * sizeof(bool));

        grid[0][2 * ScreenHeight() + 1] =
        grid[0][2 * ScreenHeight() + 2] =
        grid[0][2 * ScreenHeight() + 3] = 1;

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        // Update the time
        totalTime += fElapsedTime;
        if (totalTime >= updateTime)
        {
            totalTime -= updateTime;

            // Draw the currently displayed grid, pixel by pixel
            for (int x = 0; x < ScreenWidth(); x++)
                for (int y = 0; y < ScreenHeight(); y++)
                    if (grid[display][x * ScreenHeight() + y])
                        Draw(x, y, olc::Pixel(255, 255, 255));
                    else
                        Draw(x, y, olc::Pixel(0, 0, 0));

            // Compute the next generation
            updateWithCuda(ScreenWidth(), ScreenHeight(), grid[display], grid[!display]);

            // Swap the grids
            display = !display;
        }

        return true;
    }

    bool OnUserDestroy() override
    {
        // Free the previously allocated memory
        free(grid[0]);
        free(grid[1]);

        return true;
    }

private:
    bool* grid[2];
    bool  display    = 0;
    float totalTime  = 1.;
    float updateTime = .001;
};

int main()
{
    //const int rows = 5, cols = 5;
    //bool in [rows * cols] = { 0 };
    //bool out[rows * cols] = { 0 };

    //in[2 * cols + 1] = in[2 * cols + 2] = in[2 * cols + 3] = 1;

    //for (int i = 0; i < rows; i++) {
    //    for (int j = 0; j < cols; j++)
    //        std::cout << in[i * cols + j] << "\t";
    //    std::cout << std::endl;
    //}

    //updateWithCuda(rows, cols, in, out);
    //std::cout << std::endl;

    //for (int i = 0; i < rows; i++) {
    //    for (int j = 0; j < cols; j++)
    //        std::cout << out[i * cols + j] << "\t";
    //    std::cout << std::endl;
    //}
     
    GameOfLife gol;
    if (gol.Construct(25, 25, 10, 10))
        gol.Start();

    return 0;
}
