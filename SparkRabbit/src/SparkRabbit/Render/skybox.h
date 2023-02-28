#pragma once
#include "PreCompile.h"

namespace SparkRabbit {
    std::vector<float> skyboxVerts = {
        // positions

        // north
         -1.0f,  1.0f, -1.0f,
         -1.0f, -1.0f, -1.0f,
          1.0f, -1.0f, -1.0f,
          1.0f, -1.0f, -1.0f,
          1.0f,  1.0f, -1.0f,
         -1.0f,  1.0f, -1.0f,

         // west
          -1.0f, -1.0f,  1.0f,
          -1.0f, -1.0f,  1.0f,
          -1.0f, -1.0f, -1.0f,
          -1.0f,  1.0f, -1.0f,
          -1.0f,  1.0f, -1.0f,
          -1.0f,  1.0f,  1.0f,

          // east
           1.0f, -1.0f, -1.0f,
           1.0f, -1.0f,  1.0f,
           1.0f,  1.0f,  1.0f,
           1.0f,  1.0f,  1.0f,
           1.0f,  1.0f, -1.0f,
           1.0f, -1.0f, -1.0f,

           // south
           -1.0f, -1.0f,  1.0f,
           -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
           -1.0f, -1.0f,  1.0f,


           // up
           -1.0f,  1.0f,  1.0f,
           -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
           -1.0f,  1.0f,  1.0f,

           // down
           -1.0f, -1.0f, -1.0f,
           -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
           -1.0f, -1.0f, -1.0f,
    };



#define SkyResourceDir_ "../SparkRabbit/src/SparkRabbit/Resource/skybox/"


    // skybox texture path
    std::vector<std::string> skyboxTex
    {
        SkyResourceDir_"clouds1_north.bmp",
        SkyResourceDir_"clouds1_west.bmp",
        SkyResourceDir_"clouds1_east.bmp",
        SkyResourceDir_"clouds1_south.bmp",
        SkyResourceDir_"clouds1_up.bmp",
        SkyResourceDir_"clouds1_down.bmp"
    };







}




