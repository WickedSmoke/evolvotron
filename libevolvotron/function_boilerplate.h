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
  \brief Interface for abstract base class FunctionBoilerplate.
  And implementation given that it's a template.
*/

#ifndef _function_boilerplate_h_
#define _function_boilerplate_h_

#include "useful.h"
#include "function_node.h"
#include "function_node_info.h"
#include "margin.h"

//! Enum for classification bits
enum
  {
    FnCore=1,           // Constant, Identity or Transform: the 3 zero-child diluting functions
    FnStructure=2,      // Functions which give rise to a lot of structure e.g spirals and grids
    FnIterative=4,      // Iterative functions
    FnFractal=8         // Fractal functions
  };

//! Template class to generate boilerplate for virtual methods.
template <typename FUNCTION,uint PARAMETERS,uint ARGUMENTS,bool ITERATIVE> class FunctionBoilerplate : public FunctionNode
{
 public:
  typedef FunctionNode Superclass;
  
  //! Registration member returns a pointer to class meta-information.
  static FunctionRegistration*const get_registration();
  
  //! Evaluation is supplied by the specific class.
  virtual const XYZ evaluate(const XYZ& p) const
    =0;
  
  //! Constant-ness is supplied by the specific class.
  virtual const bool is_constant() const
    =0;

  //! Bits give some classification of the function type
  virtual const uint self_classification() const
    =0;

  //! Constructor
  /*! \warning Careful to pass an appropriate initial iteration count for iterative functions.
   */
  FunctionBoilerplate(const std::vector<float>& p,const std::vector<FunctionNode*>& a,uint iter)
    :FunctionNode(p,a,iter)
    {
      assert(params().size()==PARAMETERS);
      assert(args().size()==ARGUMENTS);
      assert((iter==0 && !ITERATIVE) || (iter!=0 && ITERATIVE));
    }
  
  //! Destructor.
  virtual ~FunctionBoilerplate()
    {}

  //! Factory method to create a stub node for this type
  static FunctionNode*const stubnew(const MutationParameters& mutation_parameters,bool exciting)
    {
      //! \todo Needs attention.  Will need to create then assign.
      return new FUNCTION
	(
	 stubparams(mutation_parameters,PARAMETERS),
	 stubargs(mutation_parameters,ARGUMENTS,exciting),
	 (ITERATIVE ? stubiterations(mutation_parameters) : 0)
	 );
    }

  //! Factory method to create a node given contents.
  /*! Returns null if there's a problem, in which case explanation is in report
   */
  static FunctionNode*const create(const FunctionNodeInfo* info,std::string& report)
    {
      if (!verify_info(info,PARAMETERS,ARGUMENTS,ITERATIVE,report)) return 0;

      std::vector<FunctionNode*> args;
      if (!create_args(info,args,report)) return 0;

      return new FUNCTION(info->params(),args,info->iterations());
    }
  
  //! Return a deeploned copy.
  virtual FunctionNode*const deepclone() const
    {
      return new FUNCTION(cloneparams(),cloneargs(),iterations());
    }
  
  //! Internal self-consistency check.  We can add some extra checks.
  virtual const bool ok() const
    {
      return 
	(
	 params().size()==PARAMETERS
	 &&
	 args().size()==ARGUMENTS 
	 &&
	 ((iterations()==0 && !ITERATIVE) || (iterations()!=0 && ITERATIVE))
	 &&
	 FunctionNode::ok()
	 );
    }

  //! Save this node.
  virtual std::ostream& save_function(std::ostream& out,uint indent) const
    {
      out << Margin(indent) << "<f>\n";
      out << Margin(indent+1) << "<type>" << get_registration()->name() << "</type>\n";
      Superclass::save_function(out,indent+1);
      out << Margin(indent) << "</f>\n";
      return out;
    }
};

//! You'd expect this to live in the .cpp, but instantiation should only be triggered ONCE by REGISTER macros in function.h which is only included in function_node.cpp.
/*! We could obtain a type name obtained from typeid BUT:
  - it has some strange numbers attached (with gcc 3.2 anyway) although we overwrite them later anyway.
  - the strings returned from it seem to bomb if you try and do anything with them during static initialisation.
  So we use the no-name registration and the programmer-friendly name gets filled in by the REGISTER macro later.
  We could declare r as a static class member instead, but the static initializer fiasco strikes again.
*/
template <typename FUNCTION,uint PARAMETERS,uint ARGUMENTS,bool ITERATIVE> 
	     FunctionRegistration*const FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE>::get_registration()
{
  static FunctionRegistration r
    (
     /*typeid(FUNCTION).name(),*/
     &FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE>::stubnew,
     &FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE>::create,
     PARAMETERS,
     ARGUMENTS,
     ITERATIVE,
     FUNCTION::type_classification()
     );
  return &r;
}

#define FUNCTION_BEGIN(FN,NP,NA,IT,CL) \
   class FN : public FunctionBoilerplate<FN,NP,NA,IT> \
   {public: \
     FN(const std::vector<float>& p,const std::vector<FunctionNode*>& a,uint iter) \
       :FunctionBoilerplate<FN,NP,NA,IT>(p,a,iter) {} \
     virtual ~FN() {} \
     static const uint type_classification() {return CL;} \
     virtual const uint self_classification() const {return CL;}

#define FUNCTION_END(FN) \
   }; \
   REGISTER(FN);

#endif

