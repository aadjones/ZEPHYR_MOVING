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
#ifndef BOX_H
#define BOX_H

#include "EIGEN.h"
#include <glvu.h>
#include "VEC3.h"
#include "MATRIX3.h"
#include "FIELD_3D.h"
#include <iostream>

class BOX: public OBSTACLE  
{
public:
	BOX() {};
	~BOX() {};


  // set a constant angular velocity, assuming an axis of rotation (0,0,1)
  // void set_angularVelocity(int framesPerRevolution) {
  //  _angularVelocity[0] = 0.0;
  //  _angularVelocity[1] = 0.0;
  //  _angularVelocity[2] = M_2_PI / (float)framesPerRevolution;
  // };

  // compute the linear velocity from the angular velocity and passed in radial direction
  // void set_linearVelocity(const VEC3F& r) {
  //  _velocity = cross(_angularVelocity, r);
  // }

  

protected:
};

#endif
