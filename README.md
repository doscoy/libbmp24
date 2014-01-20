# libbmp24

#DESCRIPTION
libbmp24 is SIMPLE and TINY bitmap image C++ library.
This library is composed of ONE header file, so it is very portable program. Windows, Mac, UNIX, and more platform can use libbmp24.

# SUPPORT FORMAT
24bit bitmap only.
8bit, 16bit color pallet format is not support.


##SAMPLES

    #include "libbmp24.hpp"

    
### Sample 01.
Load bmp and rename save.

    int main() {
        libbmp24::Bitmap bmp;
        
        // Load bmp.
        std::ifstream is("src.bmp", std::ios::in);
        bmp.deserialize(is);
        // Rename and Save.
        std::ofstream os("dest.bmp", std::ios::out);
        bmp.serialize(os);
        return 0;
    }



### Sample02.
Dump bitmap file information to console window.

    int main() {
        std::ifstream is("src.bmp", std::ios::in);
        libbmp24::Bitmap bmp;
        bmp.deserialize(is);
        bmp.dump();
        return 0;
    }
    
### Sample03.
Create new bitmap file.

    int main() {
        libbmp24::Bitmap bmp;
        const int width = 64;
        const int height = 100;
        bmp.createBitmap(width, height); // create new bitmap.
        bmp.fill(255, 0, 30); // r, g, b
        // Save.
        std::ofstream os("dest.bmp", std::ios::out);
        bmp.serialize(os);
        return 0;
    }
    
### Sample04.
Use bmp to OpenGL texture.

    // Global variable
    libbmp24::Bitmap bmp_;

    // Your OpenGL setup function.
    void setup_opengl() {
        // Load texture.
        std::ifstream is("my_texture.bmp", std::ios::in);
        bmp_.deserialize(is);
        
        // Setup GL Status.
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,    // 24bit bitmap don't have Alpha color.
            bmp_.getWidth(),
            bmp_.getHeight(),
            0,
            GL_BGR,    // bitmap color format is BGR. Not RGB.
            GL_UNSIGNED_BYTE,
            bmp_.getData()
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glEnable(GL_TEXTURE_2D);
    }




##LICENSE

libbmp24 is released under the zlib license.

Please read [LICENSE](https://github.com/doscoy/libbmp24/blob/master/LICENSE) file.

##CONTRIBUTERS

* Murilo Pontes.
