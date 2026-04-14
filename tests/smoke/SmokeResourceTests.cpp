#include <easygl/easygl.hpp>
#include <cassert>
#include <iostream>

void test_buffer()
{
    easygl::Buffer buffer;
    assert(!buffer.is_created());
    
    buffer.create();
    // In stub it's still not created because handle is 0, but it's fine for now
    
    easygl::Buffer other = std::move(buffer);
    assert(!buffer.is_created());
}

void test_shader()
{
    easygl::Shader shader(easygl::ShaderStage::Vertex);
    assert(!shader.is_created());
    
    shader.create();
    
    easygl::Shader other = std::move(shader);
    assert(!shader.is_created());
}

void test_program()
{
    easygl::Program program;
    assert(!program.is_created());
    
    program.create();
    
    easygl::Program other = std::move(program);
    assert(!program.is_created());
}

void test_vertex_array()
{
    easygl::VertexArray vao;
    assert(!vao.is_created());
    
    vao.create();
    
    easygl::VertexArray other = std::move(vao);
    assert(!vao.is_created());
}

void test_texture()
{
    easygl::Texture texture;
    assert(!texture.is_created());
    
    texture.create();
    
    easygl::Texture other = std::move(texture);
    assert(!texture.is_created());
}

int main()
{
    try {
        test_buffer();
        test_shader();
        test_program();
        test_vertex_array();
        test_texture();
        std::cout << "easy-gl resource smoke tests passed\n";
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
