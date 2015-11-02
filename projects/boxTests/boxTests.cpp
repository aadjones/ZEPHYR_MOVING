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
#include "BOX.h"
#include <cmath>
#include <iostream>

VEC3F center(0.5, 0.5, 0.5);
VEC3F lengths(0.1, 0.1, 0.1);
VEC3F point(0.0, 0.0, 0.0);
MATRIX3* rotation = NULL;
double period = 4.0;
const double dt = 0.1;
int iterationsPerPeriod = period / dt;


void isInside(const VEC3F& point, BOX* box)
{
  cout << "Point: " << endl;
  cout << point << endl;
  cout << "is inside the box: " << box->inside(point) << endl;
}

VEC3F rotatePoint(const MATRIX3& rotate, const VEC3F& point)
{
  // translate point to center 
  VEC3F rotatedPoint = point - center;
  rotatedPoint = rotate * rotatedPoint;
  // undo the translation
  rotatedPoint += center;
  return rotatedPoint;
}

int main(int argc, char *argv[])
{
  VEC3F halfLengths = 0.5 * lengths;
  point = center + halfLengths;
  BOX* box = new BOX(center, lengths, period);
  box->set_dt(dt);
  rotation = box->get_rotation();


  VEC3F originalPoint = point;
  for (int i = 0; i < iterationsPerPeriod; i++) {
  isInside(point, box);
  box->spin();
  box->update_rotationMatrix();
  // VEC3F rotatedPoint = rotatePoint(rotation->transpose(), originalPoint);
  // point = rotatedPoint;
  }


  delete box;
  return 0;
}


