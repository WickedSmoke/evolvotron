#include "colors.inc"

camera {perspective location <0,1,-4.5> look_at <0,0,0> angle 45}
light_source {<100,100,-100> color White}
sphere 
{ 
  <0,0,0>,1 pigment { image_map {png "spheremap.png" map_type 1} } 
  rotate <0,clock*360,0.0>
}

