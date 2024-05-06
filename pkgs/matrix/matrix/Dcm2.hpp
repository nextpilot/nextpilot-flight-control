/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * @file Dcm2.hpp
 *
 * A givens rotation matrix 2x2.
 * All rotations and axis systems follow the right-hand rule.
 *
 * This library uses the convention that premultiplying a two dimensional
 * vector represented in coordinate system 1 will apply a rotation from coordinate system
 * 1 to coordinate system 2 to the vector.
 * Likewise, a matrix instance of this class also represents a coordinate transformation
 * from frame 2 to frame 1.
 *
 * @author Matthias Grob <maetugr@gmail.com>
 */

#pragma once

#include "math.hpp"

namespace matrix
{

template<typename Type>
class Dcm2 : public SquareMatrix<Type, 2>
{
public:
	/**
	 * Standard constructor
	 *
	 * Initializes to identity
	 */
	Dcm2() : SquareMatrix<Type, 2>(eye<Type, 2>()) {}

	/**
	 * Constructor from array
	 *
	 * @param _data pointer to array
	 */
	explicit Dcm2(const Type data_[2][2]) : SquareMatrix<Type, 2>(data_)
	{
	}

	/**
	 * Constructor from array
	 *
	 * @param _data pointer to array
	 */
	explicit Dcm2(const Type data_[4]) : SquareMatrix<Type, 2>(data_)
	{
	}

	/**
	 * Copy constructor
	 *
	 * @param other Matrix22 to set dcm to
	 */
	Dcm2(const Matrix<Type, 2, 2> &other) : SquareMatrix<Type, 2>(other)
	{
	}

	/**
	 * Constructor from an angle
	 *
	 * This sets the transformation matrix from frame 2 to frame 1 where the rotation
	 * from frame 1 to frame 2 is described by an angle in radians.
	 *
	 * @param angle from frame 1 to frame 2 in radians
	 */
	Dcm2(const Type angle)
	{
		Dcm2 &dcm = *this;
		Type sin_angle = sin(angle);
		Type cos_angle = cos(angle);

		dcm(0, 0) = cos_angle;
		dcm(0, 1) = -sin_angle;
		dcm(1, 0) = sin_angle;
		dcm(1, 1) = cos_angle;
	}

	void renormalize()
	{
		/* renormalize rows */
		for (size_t r = 0; r < 2; r++) {
			matrix::Vector2<Type> rvec(Matrix<Type, 1, 2>(this->Matrix<Type, 2, 2>::row(r)).transpose());
			this->Matrix<Type, 2, 2>::row(r) = rvec.normalized();
		}
	}
};

using Dcm2f = Dcm2<float>;
using Dcm2d = Dcm2<double>;

} // namespace matrix
