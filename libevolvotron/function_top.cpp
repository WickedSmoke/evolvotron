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

  const TransformIdentity ti;
  std::vector<real> tiv=ti.get_columns();
  std::vector<real> p;
  p.insert(p.end(),tiv.begin(),tiv.end());
  p.insert(p.end(),tiv.begin(),tiv.end());

  FunctionTop* fn_top=new FunctionTop(p,a,0);
  if (unwrapped)
    {
      // For unwrapped just allow a scale factor and scramble colours
      fn_top->concatenate_pretransform_on_right(TransformScale(parameters.rnegexp()));
      fn_top->reset_posttransform_parameters(parameters);
    }
  else
    {
      fn_top->reset_pretransform_parameters(parameters);
      fn_top->reset_posttransform_parameters(parameters);
    }
  return fn_top;
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
      mutate_pretransform_parameters(parameters);
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

void FunctionTop::concatenate_pretransform_on_right(const Transform& transform)
{
  Transform current_transform(params(),0);
  current_transform.concatenate_on_right(transform);
  for (uint i=0;i<12;i++)
    params()[i]=current_transform.get_columns()[i];
}

const Transform FunctionTop::interesting_pretransform(const MutationParameters& parameters,const real k)
{
  Transform t=TransformIdentity();
  
  XYZ origin(0.0,0.0,0.0);
  if (parameters.r01()<0.5)
    {
      origin.x(parameters.rnegexp());
      origin.y(parameters.rnegexp());
      if (parameters.r01()<0.5) origin.z(parameters.rnegexp());
    }
  
  t.concatenate_on_right(TransformTranslate(-origin));

  if (parameters.r01()<0.5)
    {
      t.concatenate_on_right(TransformScale(1.0+k*(parameters.rnegexp()-1.0)));
    }
  else
    {
      t.concatenate_on_right(TransformScale(XYZ(1.0+k*(parameters.rnegexp()-1.0),1.0+k*(parameters.rnegexp()-1.0),1.0+k*(parameters.rnegexp()-1.0))));
    }

  if (k==1.0)
    {
      while (parameters.r01()<0.125) t.concatenate_on_right(TransformScale(2.0));
      while (parameters.r01()<0.125) t.concatenate_on_right(TransformScale(0.5));
    }
  else if (parameters.r01()<parameters.probability_parameter_reset())
    {
      do 
	{
	  t.concatenate_on_right(TransformScale(2.0));
	}
      while (parameters.r01()<0.125);
      while (parameters.r01()<0.125);
	{
	  t.concatenate_on_right(TransformScale(0.5));
	}
    }

  if (k==1.0)
    {
      if (parameters.r01()<0.0625) t.concatenate_on_right(TransformRotateX(0.5*M_PI*(parameters.r01()<0.5 ? 1.0 : -1.0)));
      if (parameters.r01()<0.0625) t.concatenate_on_right(TransformRotateY(0.5*M_PI*(parameters.r01()<0.5 ? 1.0 : -1.0)));
      if (parameters.r01()<0.0625) t.concatenate_on_right(TransformRotateZ(0.5*M_PI*(parameters.r01()<0.5 ? 1.0 : -1.0)));
    }

  if (parameters.r01()<0.5)
    {
      t.concatenate_on_right(TransformRotateZ(k*2.0*M_PI*parameters.r01()));
    }
  if (parameters.r01()<0.125)
    {
      t.concatenate_on_right(TransformRotateX(k*2.0*M_PI*parameters.r01()));
    }
  if (parameters.r01()<0.125)
    {
      t.concatenate_on_right(TransformRotateY(k*2.0*M_PI*parameters.r01()));
    }
  
  t.concatenate_on_right(TransformTranslate(origin));
  
  XYZ translate(0.0,0.0,0.0);
  if (parameters.r01()<0.5)
    {
      translate.x(k*parameters.rnegexp());
      translate.y(k*parameters.rnegexp());
      if (parameters.r01()<0.5) translate.z(k*parameters.rnegexp());
    }
  t.concatenate_on_right(TransformTranslate(k*translate));

  return t;
}

void FunctionTop::mutate_pretransform_parameters(const MutationParameters& parameters)
{
  concatenate_pretransform_on_right(interesting_pretransform(parameters,parameters.magnitude_parameter_variation()));
  // TODO: Chance of re-othogonalizing basis vectors.
}

void FunctionTop::reset_pretransform_parameters(const MutationParameters& parameters)
{
  const Transform t=interesting_pretransform(parameters,1.0);
  const std::vector<real> p(t.get_columns());
  for (uint i=0;i<11;i++)
    params()[i]=p[i];
}

void FunctionTop::mutate_posttransform_parameters(const MutationParameters& parameters)
{
  for (uint i=12;i<23;i++)
    params()[i]+=parameters.magnitude_parameter_variation()*(parameters.r01()<0.5 ? -parameters.rnegexp() : parameters.rnegexp());
}

void FunctionTop::reset_posttransform_parameters(const MutationParameters& parameters)
{
  const std::vector<real> p(stubparams(parameters,12));
  for (uint i=0;i<11;i++)
    params()[12+i]=p[i];
}
