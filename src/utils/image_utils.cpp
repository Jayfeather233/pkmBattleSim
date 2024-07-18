#include "image_utils.hpp"

image readImage(std::string filepath){
    return Magick::Image(filepath);
}