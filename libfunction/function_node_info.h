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
  \brief Interface for class FunctioNodeInfo.
*/

#ifndef _function_node_info_h_
#define _function_node_info_h_

#include "common.h"

//! Trivial class used during XML parsing.
/*! Encapsulates enough info to build a function tree from
 */
class FunctionNodeInfo
{
public:
  FunctionNodeInfo()
    :_type("UNKNOWN")
    ,_iterations(0)
  {}
  
  ~FunctionNodeInfo()
  {}
  
  //! Accessor.
  const std::string& type() const
  {
    return _type;
  }

  //! Accessor.
  void type(const std::string& t)
  {
    _type=t;
  }

  //! Accessor.
  const std::vector<real>& params() const
  {
    return _params;
  }

  //! Accessor.
  std::vector<real>& params()
  {
    return _params;
  }
  
  //! Accessor.
  const boost::ptr_vector<FunctionNodeInfo>& args() const
    {
      return _args;
    }

  //! Accessor.
  boost::ptr_vector<FunctionNodeInfo>& args()
    {
      return _args;
    }

  //! Accessor.
  uint iterations() const
  {
    return _iterations;
  }
  
  //! Accessor.
  void iterations(uint i)
  {
    _iterations=i;
  }

protected:
  std::string _type;
  std::vector<real> _params;
  boost::ptr_vector<FunctionNodeInfo> _args;
  uint _iterations;
};

#endif
