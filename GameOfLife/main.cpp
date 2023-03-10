// OneLoneCoder's olcPixelGameEngine
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

// CUDA code
#include "kernel.cuh"

// c++ libs
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
		// Compute the grids dimensions
		width = ScreenWidth() / cellSize;
		height = ScreenHeight() / cellSize;

		// Allocate memory for both grids
		grid[0] = (bool*)malloc(width * height * sizeof(bool));
		grid[1] = (bool*)malloc(width * height * sizeof(bool));

		// Clears the grids
		memset(grid[0], 0, width * height * sizeof(bool));
		memset(grid[1], 0, width * height * sizeof(bool));

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		return update(fElapsedTime) && draw(fElapsedTime);
	}

	bool OnUserDestroy() override
	{
		// Free the previously allocated memory
		free(grid[0]);
		free(grid[1]);

		return true;
	}

	bool update(float fElapsedTime)
	{
		// If SPACE is pressed pause the game
		if (GetKey(olc::SPACE).bPressed)
			pause = !pause;

		// If ESCAPE is pressed close the game
		if (GetKey(olc::ESCAPE).bPressed)
			return false;

		if (pause)
		{
			// Draw living cells
			if (GetMouse(olc::Mouse::LEFT).bHeld)
			{
				int x = GetMouseX() / cellSize, y = GetMouseY() / cellSize;
				grid[ display][x * height + y] = 1;
				grid[!display][x * height + y] = 1;
				forceDraw = 1;
			}

			// Draw dead cells
			if (GetMouse(olc::Mouse::RIGHT).bHeld)
			{
				int x = GetMouseX() / cellSize, y = GetMouseY() / cellSize;
				grid[ display][x * height + y] = 0;
				grid[!display][x * height + y] = 1;
				forceDraw = 1;
			}
		}
		else
		{
			// Update the time
			totalTime += fElapsedTime;
			if (totalTime >= updateTime)
			{
				totalTime -= updateTime;

				// Compute the next generation
				cudaStatus = updateWithCuda(width, height, grid[display], grid[!display]);

				// Swap the grids
				display = !display;

				forceDraw = 1;
			}
		}

		return true;
	}

	bool draw(float fElapsedTime)
	{
		// Clear messages
		FillRect((ScreenWidth() - 9 * 8) / 2, (ScreenHeight() - 8) / 2, 9 * 8 * 2, 8 * 2, olc::BLACK);

		if (forceDraw) {
			forceDraw = 0;

			// Draw the currently displayed grid, pixel by pixel
			for (int x = 0; x < width; x++)
				for (int y = 0; y < height; y++)
					if (grid[display][x * height + y])
						FillRect(x * cellSize, y * cellSize, cellSize, cellSize, olc::WHITE);
					else if (grid[!display][x * height + y])
						FillRect(x * cellSize, y * cellSize, cellSize, cellSize, olc::BLACK);
		}

		// Draw messages
		if (pause)
		{
			DrawString((ScreenWidth() - 5 * 8) / 2, (ScreenHeight() - 8) / 2, "pause", olc::RED, 2);
		}
		else if (cudaStatus != cudaSuccess)
		{
			DrawString((ScreenWidth() - 9 * 8) / 2, (ScreenHeight() - 8) / 2, "cudaError", olc::RED, 2);
		}

		return true;
	}

private:
	int width;
	int height;
	int cellSize = 5;
	bool display = 0;
	bool* grid[2];

	float updateTime = 0; // .1;
	float totalTime = updateTime;

	bool pause = 1;
	bool forceDraw = 1;

	cudaError_t cudaStatus;
};

int main()
{
	// Display the controls in the terminal
	std::cout << "## CONTROLS ##" << std::endl;
	std::cout << "    SPACE                  : Pause the game" << std::endl;
	std::cout << "    ESCAPE                 : Close the game" << std::endl;
	std::cout << "    MOUSE 0 (while paused) : Draw a living cell" << std::endl;
	std::cout << "    MOUSE 1 (while paused) : Erase a living cell" << std::endl;

	// Launch the game window
	GameOfLife gol;
	if (gol.Construct(1280, 720, 1, 1, false, false))
		gol.Start();

	return 0;
}
