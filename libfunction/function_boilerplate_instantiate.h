/**************************************************************************/
/*  Copyright 2012 Tim Day                                                */
/*                                                                        */
/*  This file is part of Evolvotron                                       */
/*                                                                        */
/*  Evolvotron is free software: you can redistribute it and/or modify    */
/*  it under the terms of the GNU General Public License as published by  */
/*  the Free Software Foundation, either version 3 of the License, or     */
/*  (at your option) any later version.                                   */
/*                                                                        */
/*  Evolvotron is distributed in the hope that it will be useful,         */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*  GNU General Public License for more details.                          */
/*                                                                        */
/*  You should have received a copy of the GNU General Public License     */
/*  along with Evolvotron.  If not, see <http://www.gnu.org/licenses/>.   */
/**************************************************************************/

/*! \file 
  \brief Modifications to function_boilerplate macros.
  Include this in function .cpp files before the header is included.
  (This used to be simulated by putting #define INSTANTIATE_FN in front of function_boilerplate.h,
  but we wanted function_boilerplate.h to go in precompiled header.
*/

#ifndef _function_boilerplate_instantiate_h_
#define _function_boilerplate_instantiate_h_

#undef FUNCTION_END

//! Replace definition to obtain concrete implementations
#define FUNCTION_END(FN) };FN_CTOR_IMP(FN);FN_DTOR_IMP(FN);FN_VNAME_IMP(FN);FN_SNAME_IMP(FN);REGISTER_IMP(FN);

#endif
