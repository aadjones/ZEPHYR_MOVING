/*
 This file is part of SSFR (Zephyr).
 
 Zephyr is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 Zephyr is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Zephyr.  If not, see <http://www.gnu.org/licenses/>.
 
 Copyright 2013 Theodore Kim
 */
#include "EIGEN.h"
#include <cmath>
#include "QUICKTIME_MOVIE.h"

#include <glvu.h>
#include <VEC3.h>
#include <iostream>
#if __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#include <GL/glu.h>
#endif
#include "SUBSPACE_FLUID_3D_EIGEN.h"
#include "MATRIX.h"
#include "SIMPLE_PARSER.h"

using namespace std;

int main(int argc, char *argv[])
{
  // read in the cfg file
  if (argc != 2)
  {
    cout << " Usage: " << argv[0] << " *.cfg" << endl;
    return 0;
  }


  SIMPLE_PARSER parser(argv[1]);
  string snapshotPath = parser.getString("snapshot path", "./data/dummy/");
  int xRes = parser.getInt("xRes", 48);
  int yRes = parser.getInt("yRes", 64);
  int zRes = parser.getInt("zRes", 48);
  printf("Using resolutions: (%i, %i, %i)\n", xRes, yRes, zRes);

  FLUID_3D_MIC* ground = new FLUID_3D_MIC(xRes, yRes, zRes, 0);
  char buffer[256];
  string path = snapshotPath;
  // choose which density field to load up
  int step = 99;
  sprintf(buffer, "%sfluid.%04i.fluid3d", path.c_str(), step);
  string filename(buffer);
  ground->readGz(filename);

  const char* out = "fan_example_frame.pbrt";
  FIELD_3D::exportPbrt(ground->density(), out); 

  return 0;
  
}

 
   

