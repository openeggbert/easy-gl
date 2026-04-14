#include <easygl/easygl.hpp>
#include <iostream>

int main()
{
    easygl::Device device;

    try
    {
        device.initialize();
        std::cout << "easy-gl smoke test passed\n";
    }
    catch (const std::exception& ex)
    {
        std::cerr << "easy-gl smoke test failed: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}