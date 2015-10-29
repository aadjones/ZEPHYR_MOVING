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

class BOX 
{
public:
  BOX(const VEC3F& center, const VEC3F& lengths, int framesPerRevolution);
	BOX();
	~BOX();

  void set_center(const VEC3F& center) { _center = center; };

  // set the x-, y-, and z-dimensions of the box
  void set_lengths(const VEC3F& lengths) { _lengths = lengths; };

  void set_halfLengths() { _halfLengths = 0.5 * _lengths; };

  void initializeRotationMatrix();
  void updateRotationMatrix();

  // update the time step since the box moves in time
  void update_step(int step) { _step = step; };

  // is the passed in point inside the box?
  bool inside(const VEC3F& point);

  // compute the radial vector from a point to the center of rotation of the box
  void update_r(const VEC3F& point, VEC3F* r);

  // set a constant angular velocity, assuming an axis of rotation (0,0,1)
  void set_angularVelocity() {
  _angularVelocity[0] = 0.0;
  _angularVelocity[1] = 0.0;
  _angularVelocity[2] = M_2_PI / (float)_framesPerRevolution;
  };

  // compute the linear velocity from the angular velocity and passed in radial direction
  // it is assumed that the center of rotation is the origin
  void update_linearVelocity(const VEC3F& r) {
  _velocity = cross(_angularVelocity, r);
  };

protected:
  VEC3F _center;
  VEC3F _lengths;
  VEC3F _halfLengths;

  VEC3F _angularVelocity;
  VEC3F _velocity;
  MATRIX3 _rotation;

  int _step;
  int _framesPerRevolution;

};

#endif
