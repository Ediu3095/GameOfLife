#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "kernel.cuh"

#include <iostream>

//// Override base class with your custom functionality
//class Example : public olc::PixelGameEngine
//{
//public:
//    Example()
//    {
//        // Name your application
//        sAppName = "Example";
//    }
//
//public:
//    bool OnUserCreate() override
//    {
//        // Called once at the start, so create things here
//        return true;
//    }
//
//    bool OnUserUpdate(float fElapsedTime) override
//    {
//        // Called once per frame, draws random coloured pixels
//        for (int x = 0; x < ScreenWidth(); x++)
//            for (int y = 0; y < ScreenHeight(); y++)
//                Draw(x, y, olc::Pixel(rand() % 256, rand() % 256, rand() % 256));
//        return true;
//    }
//};

int main()
{
    //Example demo;
    //if (demo.Construct(25, 25, 10, 10))
    //    demo.Start();

    const int rows = 5, cols = 5;
    bool in [rows * cols] = { 0 };
    bool out[rows * cols] = { 0 };

    in[2 * cols + 1] = in[2 * cols + 2] = in[2 * cols + 3] = 1;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++)
            std::cout << in[i * cols + j] << "\t";
        std::cout << std::endl;
    }

    updateWithCuda(rows, cols, in, out);
    std::cout << std::endl;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++)
            std::cout << out[i * cols + j] << "\t";
        std::cout << std::endl;
    }

    return 0;
}
