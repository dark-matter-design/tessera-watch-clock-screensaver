#include "components/watch_clock/geometry.h"
#include "components/watch_clock/numerals.h"
#include "components/watch_clock/outline_raster.h"
#include <cassert>
#include <cmath>
#include <limits>
#include <iostream>
#include <vector>
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
      for(int n=0;n<4;++n) {
        const auto box = esphome::watch_clock::outline::place(l,n);
        assert(box.x>=l.x && box.y>=l.y);
        assert(box.x+box.width<=l.x+l.width+.01f && box.y+box.height<=l.y+l.height+.01f);
      }
    }
  // Filled area must match the signed contour area, including holes. Coverage
  // contains partial pixels, and clipping must preserve the same edge values.
  for (const auto &glyph : OUTLINES) for (int scale : {20,80,257}) {
    const int width = std::ceil(glyph.width*scale), height=scale;
    std::vector<uint8_t> pixels(width*height);
    esphome::watch_clock::outline::paint(glyph,scale,0,0,width-1,height-1,
      [&](int x1,int x2,int y,uint8_t alpha) {
        assert(x1>=0 && x2<width && y>=0 && y<height);
        for(int x=x1;x<=x2;++x) pixels[y*width+x]=alpha;
      });
    double area=0, expected_area=0; int partial=0; size_t begin=0;
    for(auto p:pixels) { area+=p/255.0; if(p>0 && p<255) ++partial; }
    for(size_t c=0;c<glyph.contours;++c) {
      auto previous=glyph.points[glyph.ends[c]-1];
      for(size_t i=begin;i<glyph.ends[c];++i) {
        auto p=glyph.points[i]; expected_area+=previous.x*p.y-p.x*previous.y; previous=p;
      }
      begin=glyph.ends[c];
    }
    expected_area=std::abs(expected_area)*scale*scale/2;
    assert(std::abs(area-expected_area)<std::max(2.0,expected_area*.01));
    assert(partial>0);
    esphome::watch_clock::outline::paint(glyph,scale,3,4,width-4,height-5,
      [&](int x1,int x2,int y,uint8_t alpha) {
        assert(x1>=3 && x2<=width-4 && y>=4 && y<=height-5);
        for(int x=x1;x<=x2;++x) assert(pixels[y*width+x]==alpha);
      });
  }
  std::cout << "Geometry: angles, outline bounds, holes, coverage, clipping, scales and alignments passed\n";
}
