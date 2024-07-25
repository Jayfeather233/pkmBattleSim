#include "image_utils.hpp"

image readImage(std::string filepath){
    if(!filepath.empty())
        return Magick::Image(filepath);
    else return Magick::Image();
}