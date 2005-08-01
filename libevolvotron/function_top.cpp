#include "function_top.h"

/*! \file
  \brief Implementation for FunctionTop
*/

const XYZ FunctionTop::evaluate(const XYZ& p) const
{
  const Transform space_transform(params(),0);
  const XYZ sp(space_transform.transformed(p)); 
  const XYZ v(arg(0)(sp));
  const XYZ tv(tanh(0.5*v.x()),tanh(0.5*v.y()),tanh(0.5*v.z()));
  // ...each component of tv is in [-1,1] so the transform parameters define a rhomboid in colour space.
  const Transform colour_transform(params(),12);
  return colour_transform.transformed(tv);
}

FunctionTop*const FunctionTop::initial(const MutationParameters& parameters,const FunctionRegistration* specific_fn,bool unwrapped)
{
  FunctionNode* fn=0;
  
  do
    {
      if (specific_fn)
	{
	  fn=(*(specific_fn->stubnew_fn()))(parameters,true);
	}
      else
	{
	  // This one is crucial: we REALLY want something interesting to happen within here.
	  fn=FunctionNode::stub(parameters,true);
	}
  
      assert(fn->ok());
      
      if (fn->is_constant() && !(specific_fn && specific_fn->name()=="FunctionConstant"))
	{
	  delete fn;
	  fn=0;
	}
    }
  while (!fn);
  
  assert(fn->ok());
  
  std::vector<FunctionNode*> a;
  a.push_back(fn);
  std::vector<real> p;

  if (unwrapped)
    {
      const TransformScale t0(parameters.rnegexp());
      std::vector<real> t0v=t0.get_columns();
      p.insert(p.end(),t0v.begin(),t0v.end());

      std::vector<real> t1=stubparams(parameters,12);
      p.insert(p.end(),t1.begin(),t1.end());
    }
  else
    {
      p=stubparams(parameters,24);
    }
  
  return new FunctionTop(p,a,0);
}

void FunctionTop::mutate(const MutationParameters& parameters,bool mutate_own_parameters)
{
  FunctionNode::mutate(parameters,false);
  
  if (parameters.r01()<parameters.probability_parameter_reset())
    {
      reset_pretransform_parameters(parameters);
    }
  else
    {
      if (parameters.r01()<0.5) mutate_pretransform_parameters(parameters);
    }
  
  if (parameters.r01()<parameters.probability_parameter_reset())
    {
      reset_posttransform_parameters(parameters);
    }
  else
    {
      mutate_posttransform_parameters(parameters);
    }
}
