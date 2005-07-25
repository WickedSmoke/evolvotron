// Source file for evolvotron
// Copyright (C) 2002,2003,2004,2005 Tim Day
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
  \brief Implementation of class FunctionNode and derived classes.
  Also include all function definition headers here becuase code from this compile module
  is included in any built executable and this sucks all the classes in.
  Trying to leave registration in functions.cpp doesn't trigger a link.
*/

#include <algorithm>
#include <iostream>

#include "margin.h"

#include "function_node.h"
#include "function_node_info.h"

#include "mutatable_image.h"

#include "function_core.h"
#include "function_transform_generalised.h"
#include "function_compose_triple.h"
#include "function_compose_pair.h"
#include "function_pre_transform.h"
#include "function_post_transform.h"
#include "function_null.h"
#include "functions_noise.h"
#include "functions.h"
#include "function_top.h"

const std::vector<FunctionNode*> FunctionNode::cloneargs() const
{
  std::vector<FunctionNode*> ret;
  for (std::vector<FunctionNode*>::const_iterator it=args().begin();it!=args().end();it++)
    {
      ret.push_back((*it)->deepclone());
    }
  return ret;
}

const std::vector<real> FunctionNode::cloneparams() const
{
  return params();
}

//! Obtain some statistics about the image function
void FunctionNode::get_stats(uint& total_nodes,uint& total_parameters,uint& depth,uint& width,real& proportion_constant) const
{
  uint total_sub_nodes=0;
  uint total_sub_parameters=0;
  uint max_sub_depth=0;
  uint total_sub_width=0;
  real sub_constants=0.0;

  // Traverse child nodes.  Need to reconstruct the actual numbers from the proportions
  for (std::vector<FunctionNode*>::const_iterator it=args().begin();it!=args().end();it++)
    {
      uint sub_nodes;
      uint sub_parameters;
      uint sub_depth;
      uint sub_width;
      real sub_proportion_constant;

      (*it)->get_stats(sub_nodes,sub_parameters,sub_depth,sub_width,sub_proportion_constant);

      total_sub_nodes+=sub_nodes;
      total_sub_parameters+=sub_parameters;
      if (sub_depth>max_sub_depth) max_sub_depth=sub_depth;
      total_sub_width+=sub_width;
      sub_constants+=sub_nodes*sub_proportion_constant;
    }

  // And add our own details
  total_nodes=1+total_sub_nodes;  

  total_parameters=params().size()+total_sub_parameters;

  depth=1+max_sub_depth;

  if (total_sub_width==0)
    {
      width=1;
    }
  else 
    {
      width=total_sub_width;
    }

  if (is_constant())
    {
      proportion_constant=1.0;
    }
  else
    {
      proportion_constant=sub_constants/(1+total_sub_nodes);
    }
}

const bool FunctionNode::verify_info(const FunctionNodeInfo* info,unsigned int np,unsigned int na,bool it,std::string& report)
{
  if (info->params().size()!=np)
    {
      std::stringstream msg;
      msg << "Error: For function " << info->type() << ": expected " << np << " parameters, but found " << info->params().size() << "\n";
      report+=msg.str();
      return false;
    }
  if (info->args().size()!=na)
    {
      std::stringstream msg;
      msg << "Error: For function " << info->type() << ": expected " << na << " arguments, but found " << info->args().size() << "\n";
      report+=msg.str();
      return false;
    }
  if (info->iterations()!=0 && !it)
    {
      std::stringstream msg;
      msg << "Error: For function " << info->type() << ": unexpected iteration count\n";
      report+=msg.str();
      return false;
    }
  if (info->iterations()==0 && it)
    {
      std::stringstream msg;
      msg << "Error: For function " << info->type() << ": expected iteration count but none found\n";
      report+=msg.str();
      return false;
    }
  return true;
}

const bool FunctionNode::is_constant() const
{
  if (args().empty()) return false;
  for (unsigned int i=0;i<args().size();i++)
    {
      if (!arg(i).is_constant()) return false;
    }
  return true;
}

const bool FunctionNode::create_args(const FunctionNodeInfo* info,std::vector<FunctionNode*>& args,std::string& report)
{
  for (std::vector<FunctionNodeInfo*>::const_iterator it=info->args().begin();it!=info->args().end();it++)
    {
      args.push_back(FunctionNode::create(*it,report));
      
      // Check whether something has gone wrong.  If it has, delete everything allocated so far and return false.
      if (args.back()==0)
	{
	  args.pop_back();
	  
	  for (std::vector<FunctionNode*>::iterator dit=args.begin();dit!=args.end();dit++)
	    {
	      delete (*dit);
	    }
	  return false;
	}
    }
  return true;
}

FunctionNode*const FunctionNode::stub(const MutationParameters& parameters,bool exciting)
{
  return parameters.random_function_stub(exciting);
}

/*! The choice of initial structure to start from is quite crucial to giving a good user experience.
  
  We concatenate 3 functions.  The outer 2 are transforms.
  You can think of the first function as a co-ordinate transform,
  the second function as being the "actual" image (we use an "exciting" stub to avoid boring constants or identities),
  and the final function as being a colour-space transform.
  Basically the idea is to give lots of opportunities for stuff to happen.
  If a specific function's registration (ie meta info) is provided then that
  will be used as the wrapped function type, but this overrides the constant checking (as a constant might have been specified).
 */
FunctionNode*const FunctionNode::initial(const MutationParameters& parameters,const FunctionRegistration* specific_fn)
{
  FunctionNode* root;
  
  do
    {
      std::vector<real> params_toplevel;
      std::vector<FunctionNode*> args_toplevel;
      
      {
	const real which=parameters.r01();
	if (which<0.45)
	  {
	    args_toplevel.push_back(FunctionTransformGeneralised::stubnew(parameters,false));
	  }
	else if (which<0.9)
	  {
	    args_toplevel.push_back(FunctionTransform::stubnew(parameters,false));
	  }
	else
	  {
	    args_toplevel.push_back(FunctionIsotropicScale::stubnew(parameters,false));
	  }
      }

      if (specific_fn)
	{
	  args_toplevel.push_back((*(specific_fn->stubnew_fn()))(parameters,true));
	}
      else
	{
	  // This one is crucial: we REALLY want something interesting to happen within here.
	  args_toplevel.push_back(FunctionNode::stub(parameters,true));
	}
      
      {
	const real which=parameters.r01();

	if (which<0.45)
	  {
	    args_toplevel.push_back(FunctionTransformGeneralised::stubnew(parameters,false));
	  }
	else if (which<0.9)
	  {
	    args_toplevel.push_back(FunctionTransform::stubnew(parameters,false));
	  }
	else
	  {
	    args_toplevel.push_back(FunctionIsotropicScale::stubnew(parameters,false));
	  }	
      }

      root=new FunctionComposeTriple(params_toplevel,args_toplevel,0);
      
      assert(root->ok());
      
      if (root->is_constant() && !(specific_fn && specific_fn->name()=="FunctionConstant"))
	{
	  delete root;
	  root=0;
	}
    }
  while (!root);
  
  assert(root->ok());
  
  return root;
}

/*! This returns a vector of random bits of stub, used for initialiing nodes with children. 
 */
const std::vector<FunctionNode*> FunctionNode::stubargs(const MutationParameters& parameters,uint n,bool exciting)
{
  std::vector<FunctionNode*> ret;
  for (uint i=0;i<n;i++)
    ret.push_back(stub(parameters,exciting));
  return ret;
}


const std::vector<real> FunctionNode::stubparams(const MutationParameters& parameters,uint n)
{
  std::vector<real> ret;
  for (uint i=0;i<n;i++)
    {
      //real v=-1.0+2.0*parameters.r01();
      //while (parameters.r01()<0.125) v*=2.0;
      real v=parameters.rnegexp();
      ret.push_back(parameters.r01() < 0.5 ? -v : v);
    }
  return ret;
}

const uint FunctionNode::stubiterations(const MutationParameters& parameters)
{
  return 1+static_cast<uint>(floor(parameters.r01()*parameters.max_initial_iterations()));
}

FunctionNode::FunctionNode(const std::vector<real>& p,const std::vector<FunctionNode*>& a,uint iter)
  :_args(a)
   ,_params(p)
   ,_iterations(iter)
{
  assert(ok());
}

/*! Returns null ptr if there's a problem, in which case there will be an explanation in report.
 */
FunctionNode*const FunctionNode::create(const FunctionNodeInfo* info,std::string& report)
{
  const FunctionRegistration*const reg=FunctionRegistry::get()->lookup(info->type());
  if (reg)
    {
      return (*(reg->create_fn()))(info,report);
    }
  else
    {
      report+="Error: Unrecognised function name: "+info->type()+"\n";
      return 0;
    }
}


/*! Deletes all arguments.  No one else should be referencing nodes except the root node of an image.
 */
FunctionNode::~FunctionNode()
{
  assert(ok());
  for (std::vector<FunctionNode*>::iterator it=args().begin();it!=args().end();it++)
    if (*it) delete (*it);
}

/*! There are 2 kinds of mutation:
  - random adjustments to constants 
  - structural mutations (messing with the function tree)
  For structural mutations the obvious things to do are:
  - reordering argsuments
  - dropping argsuments and replacing them with new "stubs".
  - duplicating arguments
  - substituting nodes with other types (can't do this for ourself very easily, but we can do it for children)
  - inserting new nodes between children and ourself

  And of course all children have to be mutated too.
 */
void FunctionNode::mutate(const MutationParameters& parameters,bool mutate_own_parameters)
{
  // First mutate all child nodes.
  for (std::vector<FunctionNode*>::iterator it=args().begin();it!=args().end();it++)
    (*it)->mutate(parameters);
  
  // Perturb any parameters we have
  if (mutate_own_parameters)
    {
      for (std::vector<real>::iterator it=params().begin();it!=params().end();it++)
	{
	  (*it)+=parameters.magnitude()*(parameters.r01()<0.5 ? -parameters.rnegexp() : parameters.rnegexp());
	}
    }

  // Perturb iteration count if any
  if (_iterations)
    {
      if (parameters.r01()<parameters.probability_iterations_change_step())
	{
	  if (parameters.r01()<0.5)
	    {
	      if (_iterations>=2) _iterations--;
	    }
	  else
	    {
	      _iterations++;
	    }
	  if (parameters.r01()<parameters.probability_iterations_change_jump())
	    {
	      if (parameters.r01()<0.5)
		{
		  if (_iterations>1) _iterations=(_iterations+1)/2;
		}
	      else
		{
		  _iterations*=2;
		}
	    }
	  
	  // For safety but shouldn't happen
	  if (_iterations==0) _iterations=1;
	}
    }
      
      
  // Then go to work on the argument structure...
  
  // Think about glitching some nodes.
  for (std::vector<FunctionNode*>::iterator it=args().begin();it!=args().end();it++)
    {
      if (parameters.r01()<parameters.probability_glitch())
	{
	  delete (*it);
	  (*it)=stub(parameters,false);
	}
    }

  // Think about substituting some nodes.
  //! \todo Substitution might make more sense if it was for a node with the same/similar number of arguments.
  for (std::vector<FunctionNode*>::iterator it=args().begin();it!=args().end();it++)
    {
      if (parameters.r01()<parameters.probability_substitute())
	{
	  // Take a copy of the nodes parameters and arguments
	  std::vector<FunctionNode*> a((*it)->deepclone_args());
	  std::vector<real> p((*it)->params());
	  
	  // Replace the node with something interesting (maybe this should depend on how complex the original node was)
	  delete (*it);
	  (*it)=stub(parameters,true);

	  // Do we need some extra arguments ?
	  if (a.size()<(*it)->args().size())
	    {
	      const std::vector<FunctionNode*> xa(stubargs(parameters,(*it)->args().size()-a.size()));
	      a.insert(a.end(),xa.begin(),xa.end());
	    }
	  // Shuffle them
	  std::random_shuffle(a.begin(),a.end());
	  // Have we go too many arguments ?
	  while (a.size()>(*it)->args().size())
	    {
	      delete (a.back());
	      a.pop_back();
	    }
	  
	  // Do we need some extra parameters ?
	  if (p.size()<(*it)->params().size())
	    {
	      const std::vector<real> xp(stubparams(parameters,(*it)->params().size()-p.size()));
	      p.insert(p.end(),xp.begin(),xp.end());
	    }
	  // Shuffle them
	  std::random_shuffle(p.begin(),p.end());
	  // Have we go too many arguments ?
	  while (p.size()>(*it)->params().size())
	    {
	      p.pop_back();
	    }

	  // Impose the new parameters and arguments on the new node (iterations not touched)
	  (*it)->impose(p,a);
	}
    }
  
  // Think about randomising child order
  if (parameters.r01()<parameters.probability_shuffle())
    {
      // This uses rand() (would rather use our own one).
      // This bit of STL seems a bit up in the air (at least in GNU implementation), but it works so who cares.
      std::random_shuffle(args().begin(),args().end());
    }

  // Think about inserting a random stub between us and some children
  for (std::vector<FunctionNode*>::iterator it=args().begin();it!=args().end();it++)
    {
      if (parameters.r01()<parameters.probability_insert())
	{
	  std::vector<real> p;
	  std::vector<FunctionNode*> a;

	  a.push_back((*it));
	  a.push_back(stub(parameters,false));

	  (*it)=new FunctionComposePair(p,a,0);
	}
    }
}

void FunctionNode::simplify_constants() 
{
  for (std::vector<FunctionNode*>::iterator it=args().begin();it!=args().end();it++)
    {
      if ((*it)->is_constant())
	{
	  const XYZ v((*(*it))(XYZ(0.0,0.0,0.0)));
	  std::vector<real> vp;
	  vp.push_back(v.x());
	  vp.push_back(v.y());
	  vp.push_back(v.z());
	  std::vector<FunctionNode*> va; 
	  delete (*it);
	  (*it)=new FunctionConstant(vp,va,0);
	}
      else
	{
	  (*it)->simplify_constants();
	}
    }
}

const std::vector<FunctionNode*> FunctionNode::deepclone_args() const
{
  std::vector<FunctionNode*> ret;
  for (std::vector<FunctionNode*>::const_iterator it=args().begin();it!=args().end();it++)
    ret.push_back((*it)->deepclone());
  return ret;
}

void FunctionNode::impose(std::vector<real>& p,std::vector<FunctionNode*>& a)
{
  for (std::vector<FunctionNode*>::iterator it=args().begin();it!=args().end();it++)
    delete (*it);
  args().clear();

  args()=a;
  params()=p;

  assert(ok());
}


const FunctionPreTransform*const FunctionNode::is_a_FunctionPreTransform() const
{
  return 0;
}

FunctionPreTransform*const FunctionNode::is_a_FunctionPreTransform()
{
  return 0;
}

const FunctionPostTransform*const FunctionNode::is_a_FunctionPostTransform() const
{
  return 0;
}

FunctionPostTransform*const FunctionNode::is_a_FunctionPostTransform()
{
  return 0;
}

const FunctionTop*const FunctionNode::is_a_FunctionTop() const
{
  return 0;
}

FunctionTop*const FunctionNode::is_a_FunctionTop()
{
  return 0;
}

const bool FunctionNode::ok() const
{
  // Args vector should never contain a null or a non-ok argument
  for (std::vector<FunctionNode*>::const_iterator it=args().begin();it!=args().end();it++)
    {
      if ((*it)==0 || !(*it)->ok())
	return false;
    }
  return true;
}

/*! This function only saves the parameters, iteration count if any and child nodes.
  It is intended to be called from save_function of subclasses which will write a function node wrapper.
  The indent number is just the level of recursion, incrementing by 1 each time.
  Outputting multiple spaces per level is handled by the Margin class.
 */
std::ostream& FunctionNode::save_function(std::ostream& out,uint indent) const
{
  if (iterations()!=0)
    out << Margin(indent) << "<i>" << iterations() << "</i>\n";
  
  for (std::vector<real>::const_iterator it=params().begin();it!=params().end();it++)
    {
      out << Margin(indent) << "<p>" << (*it) << "</p>\n";
    }

  for (std::vector<FunctionNode*>::const_iterator it=args().begin();it!=args().end();it++)
    {
      (*it)->save_function(out,indent);
    }
    
  return out;
}
