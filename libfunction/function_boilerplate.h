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
  \brief Interface for abstract base class FunctionBoilerplate.
  And implementation given that it's a template.
*/

#ifndef _function_boilerplate_h_
#define _function_boilerplate_h_

#include "common.h"

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

  //! Accessor providing function name
  virtual const char* thisname() const
    =0;

  //! Registration member returns a reference to class meta-information.
  static const FunctionRegistration get_registration(const char* fn_name);
    
  //! Bits give some classification of the function type
  static uint type_classification() {return CLASSIFICATION;}

  //! Bits give some classification of the function type
  virtual uint self_classification() const;

  //! Factory method to create a stub node for this type
  static std::unique_ptr<FunctionNode> stubnew(const MutationParameters& mutation_parameters,bool exciting);

  //! Factory method to create a node given contents.
  /*! Returns null if there's a problem, in which case explanation is in report
   */
  static std::unique_ptr<FunctionNode> create(const FunctionRegistry& function_registry,const FunctionNodeInfo& info,std::string& report);

  //! Return a deeploned copy.
  virtual std::unique_ptr<FunctionNode> deepclone() const;

  //! Return a deeploned copy with more specific type (but of course this can't be virtual).
  std::unique_ptr<FUNCTION> typed_deepclone() const;
    
  //! Internal self-consistency check.  We can add some extra checks.
  virtual bool ok() const;

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

//! Provide a complete registration for the function
template <typename FUNCTION,uint PARAMETERS,uint ARGUMENTS,bool ITERATIVE,uint CLASSIFICATION> 
const FunctionRegistration FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::get_registration(const char* fn_name)
{
  return FunctionRegistration
    (
     std::string(fn_name),
     &FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::stubnew,
     &FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::create,
     PARAMETERS,
     ARGUMENTS,
     ITERATIVE,
     FUNCTION::type_classification()
     );
}

template <typename FUNCTION,uint PARAMETERS,uint ARGUMENTS,bool ITERATIVE,uint CLASSIFICATION>
uint FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::self_classification() const
{
  return CLASSIFICATION;
}

template <typename FUNCTION,uint PARAMETERS,uint ARGUMENTS,bool ITERATIVE,uint CLASSIFICATION>
std::unique_ptr<FunctionNode> FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::stubnew(const MutationParameters& mutation_parameters,bool exciting)
{
  static constexpr uint _PARAMETERS=PARAMETERS;
  static constexpr uint _ARGUMENTS=ARGUMENTS;
  
  std::vector<real> params;
  stubparams(params,mutation_parameters,_PARAMETERS);
  
  boost::ptr_vector<FunctionNode> args;
  stubargs(args,mutation_parameters,_ARGUMENTS,exciting);
  
  return std::unique_ptr<FunctionNode>
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
std::unique_ptr<FunctionNode> FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::create(const FunctionRegistry& function_registry,const FunctionNodeInfo& info,std::string& report)
{
  if (!verify_info(info,PARAMETERS,ARGUMENTS,ITERATIVE,report)) return std::unique_ptr<FunctionNode>();
  
  boost::ptr_vector<FunctionNode> args;
  if (!create_args(function_registry,info,args,report)) return std::unique_ptr<FunctionNode>();
  
  return std::unique_ptr<FunctionNode>(new FUNCTION(info.params(),args,info.iterations()));
}

template <typename FUNCTION,uint PARAMETERS,uint ARGUMENTS,bool ITERATIVE,uint CLASSIFICATION>
std::unique_ptr<FunctionNode> FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::deepclone() const
{
  return std::unique_ptr<FunctionNode>(typed_deepclone());
}

template <typename FUNCTION,uint PARAMETERS,uint ARGUMENTS,bool ITERATIVE,uint CLASSIFICATION>
std::unique_ptr<FUNCTION> FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::typed_deepclone() const
{
  return std::unique_ptr<FUNCTION>(new FUNCTION(cloneparams(),*cloneargs(),iterations()));
}

template <typename FUNCTION,uint PARAMETERS,uint ARGUMENTS,bool ITERATIVE,uint CLASSIFICATION>
bool FunctionBoilerplate<FUNCTION,PARAMETERS,ARGUMENTS,ITERATIVE,CLASSIFICATION>::ok() const
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
  return Superclass::save_function(out,indent,thisname());
}

#define FN_CTOR_DCL(FN) FN(const std::vector<real>& p,boost::ptr_vector<FunctionNode>& a,uint iter);
#define FN_CTOR_IMP(FN) FN::FN(const std::vector<real>& p,boost::ptr_vector<FunctionNode>& a,uint iter) :Superclass(p,a,iter) {}

#define FN_DTOR_DCL(FN) virtual ~FN();
#define FN_DTOR_IMP(FN) FN::~FN() {}

#define FN_VNAME_DCL(FN) virtual const char* thisname() const;
#define FN_VNAME_IMP(FN) const char* FN::thisname() const {return FN::classname();}

#define FN_SNAME_DCL(FN) static const char* classname();
#define FN_SNAME_IMP(FN) const char* FN::classname() {return #FN;}

#define FUNCTION_BEGIN(FN,NP,NA,IT,CL) \
   class FN : public FunctionBoilerplate<FN,NP,NA,IT,CL> \
   {public: \
     typedef FunctionBoilerplate<FN,NP,NA,IT,CL> Superclass; \
     FN_CTOR_DCL(FN) \
     FN_DTOR_DCL(FN) \
     FN_VNAME_DCL(FN) \
     FN_SNAME_DCL(FN)

//! Macro to push registrations through to registry.
/*! Used by register_all_functions.h/register_all_functions.cpp
*/
#define REGISTER(R,FN) R.register_function(FN::get_registration(#FN));
#define REGISTER_DCL(FN) extern void register_ ## FN(FunctionRegistry&);
#define REGISTER_IMP(FN) void register_ ## FN(FunctionRegistry& r){REGISTER(r,FN);}

#ifdef FUNCTION_BOILERPLATE_INSTANTIATE
#define FUNCTION_END(FN) };FN_CTOR_IMP(FN);FN_DTOR_IMP(FN);FN_VNAME_IMP(FN);FN_SNAME_IMP(FN);REGISTER_IMP(FN);
#else
#define FUNCTION_END(FN) };REGISTER_DCL(FN);
#endif

#endif
