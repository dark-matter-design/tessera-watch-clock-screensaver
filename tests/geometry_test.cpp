#include "components/watch_clock/geometry.h"
#include "components/watch_clock/numerals.h"
#include <cassert>
#include <cmath>
#include <limits>
#include <iostream>
using namespace esphome::watch_clock::geometry;
void near(float a, float b) { assert(std::abs(a - b) < .001f); }
int main() {
  auto a = angles(15, 30, 30);
  near(a.hour, 105.25f); near(a.minute, 183); near(a.second, 180);
  a = angles(0, 0, 0); near(a.hour, 0); near(a.minute, 0);
  a = angles(23, 59, 59); assert(a.hour < 360 && a.hour > 359.9f);
  const Point expected[] = {{100,0},{200,100},{100,200},{0,100}};
  for (int i=0; i<4; ++i) {
    auto p=hand(100,100,100,90.0f*i); near(p.x,expected[i].x); near(p.y,expected[i].y);
  }
  auto l=layout(1280,800,100,false,0);
  near(l.width,800); near(l.x,240); near(l.height,800);
  l=layout(1920,1080,100,true,0); near(l.width,1782); near(l.dx,71.5f);
  l=layout(100,100,0,false,0); near(l.width,50);
  l=layout(100,100,999,false,0); near(l.width,100);
  l=layout(100,100,std::numeric_limits<float>::quiet_NaN(),false,0); near(l.width,100);
  // Every source stroke stays inside the art at all supported aspect ratios,
  // scales, layouts and alignments, with room for its rounded cap.
  const int sizes[][2]={{320,240},{480,480},{480,800},{1024,600},{600,1024},{1280,800},{1920,1080},{240,320}};
  for (auto &size:sizes) for (int adaptive=0;adaptive<2;++adaptive)
    for (int scale=50;scale<=100;scale+=5) for (int align=0;align<5;++align) {
      l=layout(size[0],size[1],scale,adaptive,align);
      assert(l.x>=0 && l.y>=0 && l.x+l.width<=size[0]+.01f && l.y+l.height<=size[1]+.01f);
      if(align==1) near(l.x,0); if(align==2) near(l.x+l.width,size[0]);
      if(align==3) near(l.y,0); if(align==4) near(l.y+l.height,size[1]);
      const Point *paths[]={NUMERAL_0,NUMERAL_1,NUMERAL_2,NUMERAL_3,NUMERAL_4};
      const size_t count[]={3,26,73,49,61};
      const Point offset[]={{10+l.dx,2},{10+l.dx,2},{2,-13+l.dy},{14+2*l.dx,-13+l.dy},{10+l.dx,-4+2*l.dy}};
      for(int n=0;n<5;++n) for(size_t i=0;i<count[n];++i) {
        const float x=(paths[n][i].x+offset[n].x)*l.unit;
        const float y=(paths[n][i].y+offset[n].y)*l.unit;
        assert(x>=3.2f*l.unit && y>=3.2f*l.unit);
        assert(x+3.2f*l.unit<=l.width && y+3.2f*l.unit<=l.height);
      }
    }
  std::cout << "Geometry: angles, stroke bounds, scales, aspect ratios and alignments passed\n";
}
