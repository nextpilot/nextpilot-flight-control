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
 * @file Vector4.hpp
 *
 * 4D vector class.
 *
 * @author Matthias Grob <maetugr@gmail.com>
 */

#pragma once

#include "math.hpp"

namespace matrix
{

template <typename Type, size_t M, size_t N>
class Matrix;

template <typename Type, size_t M>
class Vector;

template<typename Type>
class Vector4 : public Vector<Type, 4>
{
public:
	using Matrix41 = Matrix<Type, 4, 1>;

	Vector4() = default;

	Vector4(const Matrix41 &other) :
		Vector<Type, 4>(other)
	{
	}

	explicit Vector4(const Type data_[3]) :
		Vector<Type, 4>(data_)
	{
	}

	Vector4(Type x1, Type x2, Type x3, Type x4)
	{
		Vector4 &v(*this);
		v(0) = x1;
		v(1) = x2;
		v(2) = x3;
		v(3) = x4;
	}

	template<size_t P, size_t Q>
	Vector4(const Slice<Type, 4, 1, P, Q> &slice_in) : Vector<Type, 4>(slice_in)
	{
	}

	template<size_t P, size_t Q>
	Vector4(const Slice<Type, 1, 4, P, Q> &slice_in) : Vector<Type, 4>(slice_in)
	{
	}
};

using Vector4f = Vector4<float>;

} // namespace matrix
