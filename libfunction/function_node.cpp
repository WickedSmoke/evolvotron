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
  \brief Implementation of class FunctionNode and derived classes.
*/


#include "function_node.h"

#include "function_compose_pair.h"
#include "function_constant.h"
#include "function_node_info.h"
#include "function_registry.h"
#include "margin.h"
#include "mutation_parameters.h"

std::unique_ptr<boost::ptr_vector<FunctionNode> > FunctionNode::cloneargs() const
{
  std::unique_ptr<boost::ptr_vector<FunctionNode> > ret(new boost::ptr_vector<FunctionNode>());
  for (boost::ptr_vector<FunctionNode>::const_iterator it=args().begin();it!=args().end();it++)
    {
      ret->push_back(it->deepclone().release());
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
  for (boost::ptr_vector<FunctionNode>::const_iterator it=args().begin();it!=args().end();it++)
    {
      uint sub_nodes;
      uint sub_parameters;
      uint sub_depth;
      uint sub_width;
      real sub_proportion_constant;

      it->get_stats(sub_nodes,sub_parameters,sub_depth,sub_width,sub_proportion_constant);

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

bool FunctionNode::verify_info(const FunctionNodeInfo& info,unsigned int np,unsigned int na,bool it,std::string& report)
{
  if (info.params().size()!=np)
    {
      std::stringstream msg;
      msg << "Error: For function " << info.type() << ": expected " << np << " parameters, but found " << info.params().size() << "\n";
      report+=msg.str();
      return false;
    }
  if (info.args().size()!=na)
    {
      std::stringstream msg;
      msg << "Error: For function " << info.type() << ": expected " << na << " arguments, but found " << info.args().size() << "\n";
      report+=msg.str();
      return false;
    }
  if (info.iterations()!=0 && !it)
    {
      std::stringstream msg;
      msg << "Error: For function " << info.type() << ": unexpected iteration count\n";
      report+=msg.str();
      return false;
    }
  if (info.iterations()==0 && it)
    {
      std::stringstream msg;
      msg << "Error: For function " << info.type() << ": expected iteration count but none found\n";
      report+=msg.str();
      return false;
    }
  return true;
}

bool FunctionNode::is_constant() const
{
  if (args().empty()) return false;
  for (unsigned int i=0;i<args().size();i++)
    {
      if (!arg(i).is_constant()) return false;
    }
  return true;
}

bool FunctionNode::ok() const
{
  bool good=true;
  for (boost::ptr_vector<FunctionNode>::const_iterator it=args().begin();good && it!=args().end();it++)
    {
      good=(*it).ok();
    }
  
  return good;
}

bool FunctionNode::create_args(const FunctionRegistry& function_registry,const FunctionNodeInfo& info,boost::ptr_vector<FunctionNode>& args,std::string& report)
{
  for (boost::ptr_vector<FunctionNodeInfo>::const_iterator it=info.args().begin();it!=info.args().end();it++)
    {
      std::unique_ptr<FunctionNode> fn(FunctionNode::create(function_registry,*it,report));
      // Check whether something has gone wrong.  If it has, delete everything allocated so far and return false.
      if (!fn.get())
	{
	  args.clear();
	  return false;
	}
      args.push_back(fn.release());
    }
  return true;
}

std::unique_ptr<FunctionNode> FunctionNode::stub(const MutationParameters& parameters,bool exciting)
{
  return parameters.random_function_stub(exciting);
}

/*! This setus up a vector of random bits of stub, used for initialiing nodes with children. 
 */
void FunctionNode::stubargs(boost::ptr_vector<FunctionNode>& v,const MutationParameters& parameters,uint n,bool exciting)
{
  assert(v.empty());
  for (uint i=0;i<n;i++)
    v.push_back(stub(parameters,exciting).release());
}

void FunctionNode::stubparams(std::vector<real>& v,const MutationParameters& parameters,uint n)
{
  assert(v.empty());
  for (uint i=0;i<n;i++)
    v.push_back((parameters.r01() < 0.5f ? -1.0f : 1.0f)*parameters.rnegexp());
}

uint FunctionNode::stubiterations(const MutationParameters& parameters)
{
  return 1+static_cast<uint>(floor(parameters.r01()*parameters.max_initial_iterations()));
}

FunctionNode::FunctionNode(const std::vector<real>& p,boost::ptr_vector<FunctionNode>& a,uint iter)
  :_args(a.release())
   ,_params(p)
   ,_iterations(iter)
{}

/*! Returns null ptr if there's a problem, in which case there will be an explanation in report.
 */
std::unique_ptr<FunctionNode> FunctionNode::create(const FunctionRegistry& function_registry,const FunctionNodeInfo& info,std::string& report)
{
  const FunctionRegistration*const reg=function_registry.lookup(info.type());
  if (reg)
    {
      return std::unique_ptr<FunctionNode>((*(reg->create_fn()))(function_registry,info,report));
    }
  else
    {
      report+="Error: Unrecognised function name: "+info.type()+"\n";
      return std::unique_ptr<FunctionNode>();
    }
}

/*! Deletes all arguments.  No one else should be referencing nodes except the root node of an image.
 */
FunctionNode::~FunctionNode()
{}

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
  for (boost::ptr_vector<FunctionNode>::iterator it=args().begin();it!=args().end();it++)
    it->mutate(parameters);
  
  // Perturb any parameters we have
  if (mutate_own_parameters)
    {
      if (parameters.r01()<parameters.effective_probability_parameter_reset())
	{
	std::vector<real> p;
	stubparams(p,parameters,params().size());
	params(p);
	}
      else
	{
	  for (std::vector<real>::iterator it=params().begin();it!=params().end();it++)
	    {
	      (*it)+=parameters.effective_magnitude_parameter_variation()*(parameters.r01()<0.5 ? -parameters.rnegexp() : parameters.rnegexp());
	    }
	}
    }

  // Perturb iteration count if any
  if (_iterations)
    {
      if (parameters.r01()<parameters.effective_probability_iterations_change_step())
	{
	  if (parameters.r01()<0.5)
	    {
	      if (_iterations>=2) _iterations--;
	    }
	  else
	    {
	      _iterations++;
	    }
	  if (parameters.r01()<parameters.effective_probability_iterations_change_jump())
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
  for (uint i=0;i<args().size();i++)
    {
      if (parameters.r01()<parameters.effective_probability_glitch())
	{
	  args().replace(i,stub(parameters,false).release());
	}
    }

  // Think about substituting some nodes.
  //! \todo Substitution might make more sense if it was for a node with the same/similar number of arguments.
  for (uint i=0;i<args().size();i++)
    {
      if (parameters.r01()<parameters.effective_probability_substitute())
	{
	  // Take a copy of the nodes parameters and arguments
	  std::unique_ptr<boost::ptr_vector<FunctionNode> > a(args()[i].deepclone_args());
	  std::vector<real> p(args()[i].params());
	  
	  // Replace the node with something interesting (maybe this should depend on how complex the original node was)
	  args().replace(i,stub(parameters,false).release());

	  FunctionNode& it=args()[i];
	  // Do we need some extra arguments ?
	  if (a->size()<it.args().size())
	    {
	      boost::ptr_vector<FunctionNode> xa;
	      stubargs(xa,parameters,it.args().size()-a->size());
	      a->transfer(a->end(),xa.begin(),xa.end(),xa);
	    }
	  // Shuffle them
	  random_shuffle(*a,parameters.rng01());
	  // Have we got too many arguments ?
	  while (a->size()>it.args().size())
	    {
	      a->pop_back();
	    }
	  
	  // Do we need some extra parameters ?
	  if (p.size()<it.params().size())
	    {
	      std::vector<real> xp;
	      stubparams(xp,parameters,it.params().size()-p.size());
	      p.insert(p.end(),xp.begin(),xp.end());
	    }
	  // Shuffle them
	  random_shuffle(p,parameters.rng01());
	  // Have we go too many arguments ?
	  while (p.size()>it.params().size())
	    {
	      p.pop_back();
	    }

	  // Impose the new parameters and arguments on the new node (iterations not touched)
	  it.args(*a);
	  it.params()=p;
	}
    }
  
  // Think about randomising child order
  if (parameters.r01()<parameters.effective_probability_shuffle())
    {
      random_shuffle(args(),parameters.rng01());
    }

  // Think about inserting a random stub between us and some subnodes
  for (uint i=0;i<args().size();i++)
    {
      if (parameters.r01()<parameters.effective_probability_insert())
	{
	  boost::ptr_vector<FunctionNode> a;
	  a.transfer(a.begin(),args().begin()+i,args());
	  a.push_back(stub(parameters,false).release());
	  
	  std::vector<real> p;
	  args().insert(args().begin()+i,new FunctionComposePair(p,a,0));
	}
    }
}

void FunctionNode::simplify_constants() 
{
  for (uint i=0;i<args().size();i++)
    {
      if (args()[i].is_constant())
	{
	  const XYZ v(args()[i](XYZ(0.0,0.0,0.0)));
	  std::vector<real> vp;
	  vp.push_back(v.x());
	  vp.push_back(v.y());
	  vp.push_back(v.z());
	  boost::ptr_vector<FunctionNode> va;
          std::unique_ptr<FunctionConstant> replacement(new FunctionConstant(vp,va,0));
	  args().replace(i,replacement.release());
	}
      else
	{
	  args()[i].simplify_constants();
	}
    }
}

std::unique_ptr<boost::ptr_vector<FunctionNode> > FunctionNode::deepclone_args() const
{
  std::unique_ptr<boost::ptr_vector<FunctionNode> > ret(new boost::ptr_vector<FunctionNode>());
  for (boost::ptr_vector<FunctionNode>::const_iterator it=args().begin();it!=args().end();it++)
    ret->push_back(it->deepclone().release());
  return ret;
}

const FunctionTop* FunctionNode::is_a_FunctionTop() const
{
  return 0;
}

FunctionTop* FunctionNode::is_a_FunctionTop()
{
  return 0;
}

/*! This function only saves the parameters, iteration count if any and child nodes.
  It is intended to be called from save_function of subclasses which will write a function node wrapper.
  The indent number is just the level of recursion, incrementing by 1 each time.
  Outputting multiple spaces per level is handled by the Margin class.
 */
std::ostream& FunctionNode::save_function(std::ostream& out,uint indent,const std::string& function_name) const
{
  out << Margin(indent) << "<f>\n";
  out << Margin(indent+1) << "<type>" << function_name << "</type>\n";
  
  if (iterations()!=0) out << Margin(indent+1) << "<i>" << iterations() << "</i>\n";
  
  for (std::vector<real>::const_iterator it=params().begin();it!=params().end();it++)
    {
      out << Margin(indent+1) << "<p>" << (*it) << "</p>\n";
    }

  for (boost::ptr_vector<FunctionNode>::const_iterator it=args().begin();it!=args().end();it++)
    {
      (*it).save_function(out,indent+1);
    }

  out << Margin(indent) << "</f>\n";  
  return out;
}
