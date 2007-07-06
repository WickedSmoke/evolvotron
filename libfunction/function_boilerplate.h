// Source file for evolvotron
// Copyright (C) 2007 Tim Day
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

#include "function_node.h"
#include "function_node_info.h"
#include "function_registry.h"
#include "margin.h"

//! Template class to generate boilerplate for virtual methods.
template <typename FUNCTION,uint PARAMETERS,uint ARGUMENTS,bool ITERATIVE,uint CLASSIFICATION> class FunctionBoilerplate : public FunctionNode
{
 public:
  typedef FunctionNode Superclass;
  
  //! Constructor
  /*! \warning Careful to pass an appropriate initial iteration count for iterative functions.
   */
  FunctionBoilerplate(const std::vector<real>& p,boost::ptr_vector<FunctionNode>& a,uint iter);
  
  //! Destructor.
  virtual ~FunctionBoilerplate();

  //! Registration member returns a reference to class meta-information.
  /*! Must be hit by the REGISTER macro if it is to have it's name filled in
   */
  static FunctionRegistration& get_registration();
    
  //! Bits give some classification of the function type
  static const uint type_classification() {return CLASSIFICATION;}

  //! Bits give some classification of the function type
  virtual const uint self_classification() const;

  //! Factory method to create a stub node for this type
  static std::auto_ptr<FunctionNode> stubnew(const MutationParameters& mutation_parameters,bool exciting);

  //! Factory method to create a node given contents.
  /*! Returns null if there's a problem, in which case explanation is in report
   */
  static std::auto_ptr<FunctionNode> create(const FunctionRegistry& function_registry,const FunctionNodeInfo& info,std::string& report);

  //! Return a deeploned copy.
  virtual std::auto_ptr<FunctionNode> deepclone() const;

  //! Return a deeploned copy with more specific type (but of course this can't be virtual).
  std::auto_ptr<FUNCTION> typed_deepclone() const;
    
  //! Internal self-consistency check.  We can add some extra checks.
  virtual const bool ok() const;

  //! Save this node.
  virtual std::ostream& save_function(std::ostream& out,uint indent) const;
};

template <typename FUNCTION,uint PARAMETERS,uint ARGUMENTS,bool ITERATIVE,uint CLASSIFICATION> 
FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::FunctionBoilerplate(const std::vector<real>& p,boost::ptr_vector<FunctionNode>& a,uint iter)
  :FunctionNode(p,a,iter)
{
  assert(params().size()==PARAMETERS);
  assert(args().size()==ARGUMENTS);
  assert((iter==0 && !ITERATIVE) || (iter!=0 && ITERATIVE));
}

template <typename FUNCTION,uint PARAMETERS,uint ARGUMENTS,bool ITERATIVE,uint CLASSIFICATION> 
FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::~FunctionBoilerplate()
{}

/*! We could obtain a type name obtained from typeid BUT:
  - it has some strange numbers attached (with gcc 3.2 anyway).
  - the strings returned from it seem to bomb if you try and do anything with them too soon (ie during static initialisation).
  So we use the no-name registration and the programmer-friendly name gets filled in by the REGISTER macro.
  \warning Returns a new instance of the registration for each invokation,
  but should only be being called once if REGISTER is used correctly (once for each class).
*/
template <typename FUNCTION,uint PARAMETERS,uint ARGUMENTS,bool ITERATIVE,uint CLASSIFICATION> 
FunctionRegistration& FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::get_registration()
{
  static FunctionRegistration reg
    (
     /*typeid(FUNCTION).name(),*/
     &FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::stubnew,
     &FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::create,
     PARAMETERS,
     ARGUMENTS,
     ITERATIVE,
     FUNCTION::type_classification()
     );
  return reg;
}

template <typename FUNCTION,uint PARAMETERS,uint ARGUMENTS,bool ITERATIVE,uint CLASSIFICATION>
const uint FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::self_classification() const
{
  return CLASSIFICATION;
}

template <typename FUNCTION,uint PARAMETERS,uint ARGUMENTS,bool ITERATIVE,uint CLASSIFICATION>
std::auto_ptr<FunctionNode> FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::stubnew(const MutationParameters& mutation_parameters,bool exciting)
{
  std::vector<real> params;
  stubparams(params,mutation_parameters,PARAMETERS);
  
  boost::ptr_vector<FunctionNode> args;
  stubargs(args,mutation_parameters,ARGUMENTS,exciting);
  
  return std::auto_ptr<FunctionNode>
    (
     new FUNCTION
     (
      params,
      args,
      (ITERATIVE ? stubiterations(mutation_parameters) : 0)
      )
     );
}

template <typename FUNCTION,uint PARAMETERS,uint ARGUMENTS,bool ITERATIVE,uint CLASSIFICATION>
std::auto_ptr<FunctionNode> FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::create(const FunctionRegistry& function_registry,const FunctionNodeInfo& info,std::string& report)
{
  if (!verify_info(info,PARAMETERS,ARGUMENTS,ITERATIVE,report)) return std::auto_ptr<FunctionNode>();
  
  boost::ptr_vector<FunctionNode> args;
  if (!create_args(function_registry,info,args,report)) return std::auto_ptr<FunctionNode>();
  
  return std::auto_ptr<FunctionNode>(new FUNCTION(info.params(),args,info.iterations()));
}

template <typename FUNCTION,uint PARAMETERS,uint ARGUMENTS,bool ITERATIVE,uint CLASSIFICATION>
std::auto_ptr<FunctionNode> FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::deepclone() const
{
  return std::auto_ptr<FunctionNode>(typed_deepclone());
}

template <typename FUNCTION,uint PARAMETERS,uint ARGUMENTS,bool ITERATIVE,uint CLASSIFICATION>
std::auto_ptr<FUNCTION> FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::typed_deepclone() const
{
  return std::auto_ptr<FUNCTION>(new FUNCTION(cloneparams(),*cloneargs(),iterations()));
}

template <typename FUNCTION,uint PARAMETERS,uint ARGUMENTS,bool ITERATIVE,uint CLASSIFICATION>
const bool FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::ok() const
{
  return (
	  params().size()==PARAMETERS
	  &&
	  args().size()==ARGUMENTS 
	  &&
	  ((iterations()==0 && !ITERATIVE) || (iterations()!=0 && ITERATIVE))
	  &&
	  FunctionNode::ok()
	  );
}

template <typename FUNCTION,uint PARAMETERS,uint ARGUMENTS,bool ITERATIVE,uint CLASSIFICATION>
std::ostream& FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::save_function(std::ostream& out,uint indent) const
{
  return Superclass::save_function(out,indent,get_registration().name());
}

#define FN_CTOR_DCL(FN) FN(const std::vector<real>& p,boost::ptr_vector<FunctionNode>& a,uint iter);
#define FN_CTOR_IMP(FN) FN::FN(const std::vector<real>& p,boost::ptr_vector<FunctionNode>& a,uint iter) :Superclass(p,a,iter) {}

#define FN_DTOR_DCL(FN) virtual ~FN();
#define FN_DTOR_IMP(FN) FN::~FN() {}

#define FUNCTION_BEGIN(FN,NP,NA,IT,CL) \
   class FN : public FunctionBoilerplate<FN,NP,NA,IT,CL> \
   {public: \
     typedef FunctionBoilerplate<FN,NP,NA,IT,CL> Superclass; \
     FN_CTOR_DCL(FN) \
     FN_DTOR_DCL(FN)

//! Macro to push registrations through to registry.
/*! Used by register_all_functions.h/register_all_functions.cpp
*/
#define REGISTER(R,FN) R.name_and_register(#FN,FN::get_registration());
#define REGISTER_DCL(FN) extern void register_ ## FN(FunctionRegistry&);
#define REGISTER_IMP(FN) void register_ ## FN(FunctionRegistry& r){REGISTER(r,FN);}

#define FUNCTION_END(FN) };REGISTER_DCL(FN);

#endif
