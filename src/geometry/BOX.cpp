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

#include "BOX.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


BOX::BOX(const VEC3F& center, const VEC3F& lengths, int framesPerRevolution) :
  _center(center), _lengths(lengths), _framesPerRevolution(framesPerRevolution)
{
  set_halfLengths();
  set_angularVelocity();
}


BOX::BOX() 
{
}

BOX::~BOX()
{
}

void BOX::updateRotationMatrix()
{
  // use the z-axis for rotation for now
  VEC3F axis(0.0, 0.0, 1.0);
  float theta = _step * M_2_PI / float(_framesPerRevolution);
  // we must rotate clockwise, hence the minus sign
  _rotation.rotation(axis, -theta); 
}

bool BOX::inside(const VEC3F& point) 
{
  // translate so that the center of rotation is the origin
  VEC3F pointTranslated = point - _center;
  // rotate *clockwise* back to the original position
  pointTranslated = _rotation * pointTranslated;
  return abs(pointTranslated[0]) <= _halfLengths[0] &&
         abs(pointTranslated[1]) <= _halfLengths[1] &&
         abs(pointTranslated[2]) <= _halfLengths[2];
}

void BOX::update_r(const VEC3F& point, VEC3F* r)
{
  *r = _center - point;
}

