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
  \brief Implementation of class MutatableImage.
*/

#include <iostream>
#include <vector>
#include <stack>

#include <qxml.h>

#include "mutatable_image.h"
#include "function_node_info.h"
#include "function_top.h"
#include "mutatable_image_display_big.h"

MutatableImage::MutatableImage(FunctionTop* r,bool sinz,bool sm)
  :_top(r)
  ,_sinusoidal_z(sinz)
  ,_spheremap(sm)
  ,_locked(false)
{
  assert(r!=0);
}

MutatableImage::MutatableImage(const MutationParameters& parameters,bool exciting,bool sinz,bool sm)
  :_top(0)
  ,_sinusoidal_z(true)
  ,_spheremap(sm)
  ,_locked(false)
{
  std::vector<real> pv(FunctionNode::stubparams(parameters,12));
  std::vector<FunctionNode*> av;
  av.push_back(FunctionNode::stub(parameters,exciting));
  _top=new FunctionTop(pv,av,0);
  //! \todo _sinusoidal_z should be obtained from AnimationParameters when it exists
}

MutatableImage::~MutatableImage()
{
  delete _top;
}

//! Accessor.
const FunctionTop*const MutatableImage::top() const
{
  return _top;
}

MutatableImage*const MutatableImage::deepclone() const
{
  return new MutatableImage(top()->typed_deepclone(),sinusoidal_z(),spheremap()); 
}

const XYZ MutatableImage::operator()(const XYZ& p) const
{
  assert(top()!=0);
  return (*top())(p);
}

const bool MutatableImage::is_constant() const
{
  return top()->is_constant();
}

const bool MutatableImage::ok() const
{
  return top()->ok();
}  

const XYZ MutatableImage::sampling_coordinate(uint x,uint y,uint z,uint sx,uint sy,uint sz) const
{
  if (spheremap())
    {
      const real longitude=-M_PI+2.0*M_PI*(x+0.5)/sx;
      const real latitude=0.5*M_PI-M_PI*(y+0.5)/sy;
      const real r=(
		     sinusoidal_z()
		     ?
		     0.5+cos(M_PI*(z+0.5)/sz)
		     :
		     0.5+(z+0.5)/sz
		     );
		     
      return XYZ
	(
	 r*sin(longitude)*cos(latitude),
	 r*cos(longitude)*cos(latitude),
	 r*sin(latitude)
	 );
    }
  else
    {
      return XYZ
	(
	 -1.0+2.0*(x+0.5)/sx,
	  1.0-2.0*(y+0.5)/sy,
	 (
	  sinusoidal_z()
	  ?
	  cos(M_PI*(z+0.5)/sz)
	  :
	  -1.0+2.0*(z+0.5)/sz
	  )
	 );
    }
}

MutatableImage*const MutatableImage::mutated(const MutationParameters& p) const
{
  FunctionTop*const c=top()->typed_deepclone();  
  c->mutate(p);
  return new MutatableImage(c,sinusoidal_z(),spheremap());
}

MutatableImage*const MutatableImage::simplified() const
{
  FunctionTop*const c=top()->typed_deepclone();  
  c->simplify_constants();
  return new MutatableImage(c,sinusoidal_z(),spheremap());  
}

void MutatableImage::get_rgb(const XYZ& p,uint c[3]) const
{
  // Actually calculate a pixel value from the image.
  // negexp distribution on colour-space parameters probably means the nominal range is something like -4.0 to 4.0
  XYZ pv((*top())(p));

  // Scale nominal -2.0 to 2.0 range to 0-255
  XYZ v(127.5*(0.5*pv+XYZ(1.0,1.0,1.0)));
  
  // Clamp out of range values 
  v.x(clamped(v.x(),0.0,255.0));
  v.y(clamped(v.y(),0.0,255.0));
  v.z(clamped(v.z(),0.0,255.0));

  c[0]=(uint)floorf(v.x());
  c[1]=(uint)floorf(v.y());
  c[2]=(uint)floorf(v.z());
}

void MutatableImage::get_stats(uint& total_nodes,uint& total_parameters,uint& depth,uint& width,real& proportion_constant) const
{
  top()->get_stats(total_nodes,total_parameters,depth,width,proportion_constant);
}

std::ostream& MutatableImage::save_function(std::ostream& out) const
{
  out 
    << "<?xml version=\"1.0\"?>\n"
    << "<evolvotron-image-function version=\"" 
    << EVOLVOTRON_VERSION 
    << "\""
    << " zsweep=\""
    << (_sinusoidal_z ? "sinusoidal" : "linear")
    << "\""
    << " projection=\""
    << (_spheremap ? "spheremap" : "planar")
    << "\""
    << ">\n";
  
  top()->save_function(out,1);

  out << "</evolvotron-image-function>\n";

  return out;
}

//! LoadHandler class overrides default handler's null methods.
/*! Expect to see an <evolvotron-image> followed by nested
  <f>...</f> wrapping <type>...</type>, <i>...</i>, <p>...</p> and more <f> declarations.
 */
class LoadHandler : public QXmlDefaultHandler
{
protected:

  std::string _report;

  FunctionNodeInfo** _root;

  bool* _ret_sinusoidal_z;
  bool* _ret_spheremap;

  std::stack<FunctionNodeInfo*> _stack;

  bool _expect_top_level_element;
  bool _expect_characters;
  bool _expect_characters_type;
  bool _expect_characters_iterations;
  bool _expect_characters_parameter;

public:

  LoadHandler(FunctionNodeInfo** root,bool* sinz,bool* smap)
    :_root(root)
     ,_ret_sinusoidal_z(sinz)
     ,_ret_spheremap(smap)
     ,_expect_top_level_element(true)
     ,_expect_characters(false)
     ,_expect_characters_type(false)
     ,_expect_characters_iterations(false)
     ,_expect_characters_parameter(false)
  {
    *_root=0;
  }

  bool startDocument()
  {
    std::clog << "Reading document...\n";
    return true;
  }

  bool endDocument()
  {
    std::clog << "...completed document\n";
    if (*_root==0)
      {
	_report+="Error: No root function node found\n";
	return false;
      }
    return true;
  }
  
  //! Called for start elements.
  /*! Don't know anything about namespaces - parameters ignored.
    Stick with latin1() conversion because we shouldn't have put anything fancy in the file
   */
  bool startElement(const QString&,const QString& localName,const QString&,const QXmlAttributes& atts)
  {
    const std::string element(localName.latin1());

    if (_expect_characters)
      {
	_report+="Error: Expected character data but got start element \""+element+"\"\n";
	return false;
      }

    if (_expect_top_level_element)
      {
	if (element=="evolvotron-image-function")
	  {
	    _expect_top_level_element=false;

	    int idx;
	    if ((idx=atts.index("version"))==-1)
	      {
		_report+="Warning: File does not include evolvotron version\n";
	      }
	    else
	      {
		const QString version=atts.value(idx);
		if (version!=QString(EVOLVOTRON_VERSION))
		  {
		    QString tmp;
		    tmp="Warning: File saved from a different evolvotron version: "+version+"\n(This is version "+QString(EVOLVOTRON_VERSION)+")\n";
		    _report+=tmp.latin1();
		  }
	      }

	    if ((idx=atts.index("zsweep"))==-1)
	      {
		_report+="Warning: zsweep attribute not found\nDefaulting to sinusoidal\n";
		*_ret_sinusoidal_z=true;
	      }
	    else
	      {
		const QString zsweep=atts.value(idx);
		if (zsweep==QString("sinusoidal"))
		  *_ret_sinusoidal_z=true;
		else if (zsweep==QString("linear"))
		  *_ret_sinusoidal_z=false;
		else
		  {
		    QString tmp;
		    tmp="Error: zsweep attribute expected \"sinusoidal\" or \"linear\", but got \""+zsweep+"\"\n";
		    _report+=tmp.latin1();
		    return false;
		  }
	      }

	    if ((idx=atts.index("projection"))==-1)
	      {
		_report+="Warning: projection attribute not found\nDefaulting to planar\n";
		*_ret_spheremap=false;
	      }
	    else
	      {
		const QString projection=atts.value(idx);
		if (projection==QString("spheremap"))
		  *_ret_spheremap=true;
		else if (projection==QString("planar"))
		  *_ret_spheremap=false;
		else
		  {
		    QString tmp;
		    tmp="Error: projection attribute expected \"spheremap\" or \"planar\", but got \""+projection+"\"\n";
		    _report+=tmp.latin1();
		    return false;
		  }
	      }

	  }
	else
	  {
	    _report+="Error: Expected <evolvotron-image-function> but got \""+element+"\"\n";
	    return false;
	  }
      }
    else
      {
	if (element=="f")
	  {
	    FunctionNodeInfo*const f=new FunctionNodeInfo;
	    if (_stack.empty())
	      {
		if (*_root==0)
		  {
		    *_root=f;
		  }
		else
		  {
		    _report+="Error: Multiple top level <f> elements encountered\n";
		    return false;
		  }
	      }
	    else
	      {
		_stack.top()->args().push_back(f);
	      }
	    _stack.push(f);
	  }
	else if (element=="type")
	  {
	    _expect_characters=true;
	    _expect_characters_type=true;
	  }
	else if (element=="i")
	  {
	    _expect_characters=true;
	    _expect_characters_iterations=true;
	  }
	else if (element=="p")
	  {
	    _expect_characters=true;
	    _expect_characters_parameter=true;
	  }
	else 
	  {
	    _report+="Error: Expected <f>, <type>, <i> or <p> but got \""+element+"\"\n";
	    return false;
	  }
      }
	        
    return true;
  }

  //! We don't need to check this matches startElement
  /*! Don't know anything about namespaces - parameter ignored.
    Stick with latin1() conversion because we shouldn't have put anything fancy in the file
  */
  bool endElement(const QString&, const QString& localName, const QString&)
  {
    const std::string element(localName.latin1());

    if (_expect_characters)
      {
	_report+="Error: Expected character data but got end element \""+element+"\"\n";
	return false;
      }

    if (element=="f")
      {
	_stack.pop();
      }

    return true;
  }
  
  bool characters(const QString& s)
  {
    QString stripped=s.stripWhiteSpace();
    
    if (stripped.isEmpty())
      {
	// Just keep going and hope something good turns up (maybe next line ?)
	return true;
      }
    else
      {
	if (!_expect_characters)
	  {
          QString tmp;
          tmp = "Error: Unexpected character data : \""+s+"\"\n";
	    _report += tmp.latin1();
	    return false;
	  }
      }

    //std::clog << "Non-blank expected character data\n";

    if (_expect_characters_type)
      {
	_stack.top()->type(s.latin1());
	_expect_characters_type=false;
      }
    else if (_expect_characters_iterations)
      {
	bool ok;
	_stack.top()->iterations(s.toUInt(&ok));
	_expect_characters_iterations=false;
	if (!ok)
	  {
          QString tmp;
          tmp = "Error: Couldn't parse \""+s+"\" as an integer\n";
	    _report += tmp.latin1();
	    return false;
	  }	
      }
    else if (_expect_characters_parameter)
      {
	bool ok;
	_stack.top()->params().push_back(s.toFloat(&ok));
	_expect_characters_parameter=false;
	if (!ok)
	  {
          QString tmp;
          tmp = "Error: Couldn't parse \""+s+"\" as a real\n";
	    _report+=tmp.latin1();
	    return false;
	  }
	
      }
    
    _expect_characters=false;
    
    return true;
  }

  QString errorString()
  {
    return QString(_report.c_str());
  }
};

/*! If NULL is returned, then the import failed: error message in report.
  If an image is returned then report contains warning messages (probably version mismatch).
*/
MutatableImage*const MutatableImage::load_function(std::istream& in,std::string& report)
{
  // Don't want to faff with Qt's file classes so just read everything into a string.

  std::string in_data;
  char c;
  while (in.get(c)) in_data+=c;

  QXmlInputSource xml_source;
  xml_source.setData(QString(in_data.c_str()));

  // The LoadHandler will set this to point at the root node.  We're responsible for deleting it.
  FunctionNodeInfo* info=0;

  bool sinusoidal_z;
  bool spheremap;
  LoadHandler load_handler(&info,&sinusoidal_z,&spheremap);

  QXmlSimpleReader xml_reader;
  xml_reader.setContentHandler(&load_handler);

  const bool ok=xml_reader.parse(&xml_source,false);

  if (ok)
    {
      // Might be a warning message in there.
      report=load_handler.errorString().latin1();

      FunctionNode*const root=FunctionNode::create(info,report);
      delete info;
      
      if (root)
	{
	  FunctionTop*const fn_top=root->is_a_FunctionTop();
	  if (fn_top)
	    {
	      return new MutatableImage(fn_top,sinusoidal_z,spheremap);
	    }
	  else
	    {
	      delete root;
	      report="Loaded functions must have FunctionTop for their root node";
	      return 0;
	    }
	}
      else
	{
	  return 0;
	}
    }
  else
    {
      QString tmp;
      tmp = "Parse error: "+load_handler.errorString()+"\n";
      report=tmp.latin1();
      delete info;
      return 0;
    }
}


