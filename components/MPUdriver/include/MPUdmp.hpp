// =========================================================================
// This library is placed under the MIT License
// Copyright 2017-2018 Natanael Josue Rabello. All rights reserved.
// For the license information refer to LICENSE file in root directory.
// =========================================================================

/**
 * @file MPUdmp.hpp
 * Declare MPU class with DMP interface.
 */

#ifndef _MPU_DMP_HPP_
#define _MPU_DMP_HPP_

#include "MPU.hpp"

/*! MPU Driver namespace */
namespace mpud
{
/*! DMP namespace */
namespace dmp
{
/*! MPU with DMP interface */
class MPUdmp : public mpud::MPU
{
};

typedef MPUdmp MPUdmp_t;

}  // namespace dmp

}  // namespace mpud

#endif /* end of include guard: _MPU_DMP_HPP_ */
