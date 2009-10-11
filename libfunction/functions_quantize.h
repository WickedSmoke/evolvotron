/**************************************************************************/
/*  Copyright 2009 Tim Day                                                */
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
  \brief Functions inspired by tartan patterns.
*/

#ifndef _functions_quantize_h_
#define _functions_quantize_h_

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionPixelize,3,0,false,FnStructure)

  //! Evaluate function.
  /*! Quantize coordinates to 2D grid
   */
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ
	(
	 param(0)*round(p.x()/param(0)),
	 param(1)*round(p.y()/param(1)),
	 p.z()
	 );
    }

FUNCTION_END(FunctionPixelize)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionVoxelize,3,0,false,FnStructure)

  //! Evaluate function.
  /*! Quantize coordinates to 3D grid.
   */
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ
	(
	 param(0)*round(p.x()/param(0)),
	 param(1)*round(p.y()/param(1)),
	 param(2)*round(p.z()/param(2))
	 );
    }

FUNCTION_END(FunctionVoxelize)

//------------------------------------------------------------------------------------------

#endif
