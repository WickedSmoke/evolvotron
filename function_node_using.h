// Source file for evolvotron
// Copyright (C) 2002,2003 Tim Day
/*
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/*! \file 
  \brief Interface for class FunctionNodeUsing.
  And implementation given that it's a template.
*/

#ifndef _function_node_using_h_
#define _function_node_using_h_

#include "function_node.h"
#include "margin.h"

//! Template class to generate boilerplate for virtual methods.
template <typename F> class FunctionNodeUsing : public FunctionNode
{
 public:
  typedef FunctionNode Superclass;
  
  //! Registration member encapsulates class meta-information needed to 
  static FunctionRegistration _registration;
  
 protected:
  
  //! Evaluation supplied by the wrapped class.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return F::evaluate(*this,p);
    }
  
 public:
  
  //! Constant-ness supplied by the wrapped class.
  virtual const bool is_constant() const
    {
      return F::is_constant(*this);
    }
  
  //! Constructor
  /*! \warning Careful to pass an appropriate initial iteration count for iterative functions.
   */
  FunctionNodeUsing(const std::vector<float>& p,const std::vector<FunctionNode*>& a,uint iter)
    :FunctionNode(p,a,iter)
    {
      assert(params().size()==F::parameters());
      assert(args().size()==F::arguments());
      assert((iter==0 && !F::iterative()) || (iter!=0 && F::iterative()));
    }
  
  //! Destructor.
  virtual ~FunctionNodeUsing()
    {}

  //! Factory method to create a stub node for this type
  static FunctionNode*const stubnew(const MutationParameters& parameters)
    {
      return new FunctionNodeUsing<F>
	(
	 stubparams(parameters,F::parameters()),
	 stubargs(parameters,F::arguments()),
	 (F::iterative() ? stubiterations(parameters) : 0)
	 );
    }
  
  //! Return a deeploned copy.
  virtual FunctionNode*const deepclone() const
    {
      return new FunctionNodeUsing<F>(cloneparams(),cloneargs(),iterations());
    }
  
  //! Internal self-consistency check.  We can add some extra checks.
  virtual const bool ok() const
    {
      return 
	(
	 params().size()==F::parameters()
	 &&
	 args().size()==F::arguments() 
	 &&
	 ((iterations()==0 && !F::iterative()) || (iterations()!=0 && F::iterative()))
	 &&
	 FunctionNode::ok()
	 );
    }

  //! Save this node.
  virtual std::ostream& save_function(std::ostream& out,uint indent) const
    {
      out << Margin(indent) << "<f>\n";
      out << Margin(indent+1) << "<type>" << _registration.name() << "</type>\n";
      Superclass::save_function(out,indent+1);
      out << Margin(indent) << "</f>\n";
      return out;
    }

  //! Implementation depends on template parameter
  virtual const FunctionNodeUsing<FunctionPreTransform>*const is_a_FunctionNodeUsingFunctionPreTransform() const;

  //! Implementation depends on template parameter
  virtual FunctionNodeUsing<FunctionPreTransform>*const is_a_FunctionNodeUsingFunctionPreTransform();
};

//! In the general case this still returns 0
template <typename F> inline const FunctionNodeUsing<FunctionPreTransform>*const FunctionNodeUsing<F>::is_a_FunctionNodeUsingFunctionPreTransform() const
{
  return 0;
}

//! In the general case this still returns 0
template <typename F> inline FunctionNodeUsing<FunctionPreTransform>*const FunctionNodeUsing<F>::is_a_FunctionNodeUsingFunctionPreTransform()
{
  return 0;
}

//! Specialisation for FunctionPreTransform
template <> inline const FunctionNodeUsing<FunctionPreTransform>*const FunctionNodeUsing<FunctionPreTransform>::is_a_FunctionNodeUsingFunctionPreTransform() const
{
  return this;
}

//! Specialisation for FunctionPreTransform
template <> inline FunctionNodeUsing<FunctionPreTransform>*const FunctionNodeUsing<FunctionPreTransform>::is_a_FunctionNodeUsingFunctionPreTransform()
{
  return this;
}

//! You'd expect this to live in the .cpp, but instantiation should only be triggered ONCE by REGISTER macros in function.h which is only included in function_node.cpp.
/*! We could obtain a type name obtained from typeid BUT:
  - it has some strange numbers attached (with gcc 3.2 anyway) although we overwrite them later anyway.
  - the strings returned from it seem to bomb if you try and do anything with them during static initialisation.
  So we use the no-name registration and it gets filled in by the REGISTER macro later.
 */
template <typename F> FunctionRegistration FunctionNodeUsing<F>::_registration
(
 /*typeid(F).name(),*/
 &FunctionNodeUsing::stubnew
);

#endif

