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
  \brief Implementation of class MutatableImage.
*/

#include "mutatable_image.h"

#include "function_node_info.h"
#include "function_top.h"
#include "mutatable_image_display_big.h"
#include "random.h"
#include "transform.h"

unsigned long long MutatableImage::_count=0;

MutatableImage::MutatableImage(std::unique_ptr<FunctionTop>& r,bool sinz,bool sm,bool lock)
  :_top(r.release())
  ,_sinusoidal_z(sinz)
  ,_spheremap(sm)
  ,_locked(lock)
  ,_serial(_count++)
{
  assert(_top.get()!=0);
}

MutatableImage::MutatableImage(const MutationParameters& parameters,bool exciting,bool sinz,bool sm)
  :_sinusoidal_z(sinz)
  ,_spheremap(sm)
  ,_locked(false)
  ,_serial(_count++)
{
  std::vector<real> pv;
  FunctionNode::stubparams(pv,parameters,12);
  boost::ptr_vector<FunctionNode> av;
  av.push_back(FunctionNode::stub(parameters,exciting).release());
  _top=std::unique_ptr<FunctionTop>(new FunctionTop(pv,av,0));
  //! \todo _sinusoidal_z should be obtained from AnimationParameters when it exists
}

MutatableImage::~MutatableImage()
{}

//! Accessor.
const FunctionTop& MutatableImage::top() const
{
  return *_top;
}

boost::shared_ptr<const MutatableImage> MutatableImage::deepclone() const
{
  return deepclone(false);
}

boost::shared_ptr<const MutatableImage> MutatableImage::deepclone(bool lock) const
{
  std::unique_ptr<FunctionTop> root(top().typed_deepclone());
  return boost::shared_ptr<const MutatableImage>(new MutatableImage(root,sinusoidal_z(),spheremap(),lock)); 
}

bool MutatableImage::is_constant() const
{
  return top().is_constant();
}

bool MutatableImage::ok() const
{
  return top().ok();
}  

const XYZ MutatableImage::sampling_coordinate(real x,real y,uint z,uint sx,uint sy,uint sz) const
{
  if (spheremap())
    {
      const real longitude=-M_PI+2.0*M_PI*x/sx;
      const real latitude=0.5*M_PI-M_PI*y/sy;
      const real r=(
		    sinusoidal_z()
		    ?
		    0.5+cos(M_PI*z/sz)
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
	 -1.0+2.0*x/sx,
	  1.0-2.0*y/sy,
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

boost::shared_ptr<const MutatableImage> MutatableImage::mutated(const MutationParameters& p) const
{
  std::unique_ptr<FunctionTop> c(top().typed_deepclone());  
  c->mutate(p);
  return boost::shared_ptr<const MutatableImage>(new MutatableImage(c,sinusoidal_z(),spheremap(),false));
}

boost::shared_ptr<const MutatableImage> MutatableImage::simplified() const
{
  std::unique_ptr<FunctionTop> c(top().typed_deepclone());  
  c->simplify_constants();
  return boost::shared_ptr<const MutatableImage>(new MutatableImage(c,sinusoidal_z(),spheremap(),false));
}

const XYZ MutatableImage::get_rgb(const XYZ& p) const
{
  // Actually calculate a pixel value from the image.
  // negexp distribution on colour-space parameters probably means the nominal range is something like -4.0 to 4.0
  const XYZ pv(top()(p));

  // Scale a nominal -2.0 to 2.0 range to 0-255
  return 127.5*(0.5*pv+XYZ(1.0,1.0,1.0));
}

const XYZ MutatableImage::get_rgb(uint x,uint y,uint f,uint width,uint height,uint frames,Random01* r01,uint multisample) const
{
  XYZ accumulated_colour(0.0,0.0,0.0);
  for (uint sy=0;sy<multisample;sy++)
    for (uint sx=0;sx<multisample;sx++)
      {
	//! \todo: Multisampling in z would be a motion blur/exposure length sort of effect (but not implemented).
	// xyz co-ords vary over -1.0 to 1.0
	// In the one frame case z will be 0
	const real jx=(r01 ? (*r01)() : 0.5);
	const real jy=(r01 ? (*r01)() : 0.5);
	const XYZ p
	  (
	   sampling_coordinate
	   (
	    x+(sx+jx)/multisample,
	    y+(sy+jy)/multisample,
	    f,
	    width,
	    height,
	    frames
	    )
	   );
	
	accumulated_colour+=get_rgb(p);
      }

  accumulated_colour/=(multisample*multisample);
		  
  // Clamp out of range values
  accumulated_colour.x(clamped(accumulated_colour.x(),0.0,255.0));
  accumulated_colour.y(clamped(accumulated_colour.y(),0.0,255.0));
  accumulated_colour.z(clamped(accumulated_colour.z(),0.0,255.0));

  return accumulated_colour;
}

void MutatableImage::get_stats(uint& total_nodes,uint& total_parameters,uint& depth,uint& width,real& proportion_constant) const
{
  top().get_stats(total_nodes,total_parameters,depth,width,proportion_constant);
}

std::ostream& MutatableImage::save_function(std::ostream& out) const
{
  out 
    << "<?xml version=\"1.0\"?>\n"
    << "<evolvotron-image-function version="
    << "\""
    << stringify(EVOLVOTRON_VERSION)
    << "\""
    << " zsweep=\""
    << (_sinusoidal_z ? "sinusoidal" : "linear")
    << "\""
    << " projection=\""
    << (_spheremap ? "spheremap" : "planar")
    << "\""
    << ">\n";
  
  top().save_function(out,1);

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

  std::unique_ptr<FunctionNodeInfo>& _root;

  bool* _ret_sinusoidal_z;
  bool* _ret_spheremap;

  //! Stack just used to track our progress through the nested <f>...</f> declarations.
  /*! Ownership is entirely under _root and the pointr-container in FunctionNodeInfo.
   */
  std::stack<FunctionNodeInfo*> _stack;

  bool _expect_top_level_element;
  bool _expect_characters;
  bool _expect_characters_type;
  bool _expect_characters_iterations;
  bool _expect_characters_parameter;

public:

  LoadHandler(std::unique_ptr<FunctionNodeInfo>& root,bool* sinz,bool* smap)
    :_root(root)
     ,_ret_sinusoidal_z(sinz)
     ,_ret_spheremap(smap)
     ,_expect_top_level_element(true)
     ,_expect_characters(false)
     ,_expect_characters_type(false)
     ,_expect_characters_iterations(false)
     ,_expect_characters_parameter(false)
  {
    _root.reset();
  }

  bool startDocument()
  {
    std::clog << "Reading document...\n";
    return true;
  }

  bool endDocument()
  {
    std::clog << "...completed document\n";
    if (_root.get()==0)
      {
	_report+="Error: No root function node found\n";
	return false;
      }
    return true;
  }
  
  //! Called for start elements.
  /*! Don't know anything about namespaces - parameters ignored.
   */
  bool startElement(const QString&,const QString& localName,const QString&,const QXmlAttributes& atts)
  {
    const std::string element(localName.toLocal8Bit().data());

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
		if (version!=QString(stringify(EVOLVOTRON_VERSION)))
		  {
		    QString tmp;
		    tmp="Warning: File saved from a different evolvotron version: "+version+"\n(This is version "+QString(stringify(EVOLVOTRON_VERSION))+")\n";
		    _report+=tmp.toLocal8Bit().data();
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
		    _report+=tmp.toLocal8Bit().data();
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
		    _report+=tmp.toLocal8Bit().data();
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
	    std::unique_ptr<FunctionNodeInfo> f(new FunctionNodeInfo());
	    if (_stack.empty())
	      {
		if (_root.get()==0)
		  {
		    _stack.push(f.get());
		    _root.reset(f.release());
		  }
		else
		  {
		    _report+="Error: Multiple top level <f> elements encountered\n";
		    return false;
		  }
	      }
	    else
	      {
		FunctionNodeInfo*const it=f.get();
		_stack.top()->args().push_back(f.release());
		_stack.push(it);
	      }
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
  */
  bool endElement(const QString&, const QString& localName, const QString&)
  {
    const std::string element(localName.toLocal8Bit().data());

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
    QString stripped=s.simplified();
    
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
	    _report += tmp.toLocal8Bit().data();
	    return false;
	  }
      }

    //std::clog << "Non-blank expected character data\n";

    if (_expect_characters_type)
      {
	_stack.top()->type(s.toLocal8Bit().data());
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
	    _report += tmp.toLocal8Bit().data();
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
	    _report+=tmp.toLocal8Bit().data();
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
boost::shared_ptr<const MutatableImage> MutatableImage::load_function(const FunctionRegistry& function_registry,std::istream& in,std::string& report)
{
  // Don't want to faff with Qt's file classes so just read everything into a string.

  std::string in_data;
  char c;
  while (in.get(c)) in_data+=c;

  QXmlInputSource xml_source;
  xml_source.setData(QString(in_data.c_str()));

  // The LoadHandler will set this to point at the root node.
  std::unique_ptr<FunctionNodeInfo> info;

  bool sinusoidal_z;
  bool spheremap;
  LoadHandler load_handler(info,&sinusoidal_z,&spheremap);

  QXmlSimpleReader xml_reader;
  xml_reader.setContentHandler(&load_handler);

  const bool ok=xml_reader.parse(&xml_source,false);

  if (ok)
    {
      // Might be a warning message in there.
      report=load_handler.errorString().toLocal8Bit().data();

      assert(info.get());
      std::unique_ptr<FunctionNode> root(FunctionNode::create(function_registry,*info,report));
      info.reset();
      
      if (root.get())
	{
	  if (!root->is_a_FunctionTop())
	    {
	      // Build a FunctionTop wrapper for compataibility with old .xml files

	      boost::ptr_vector<FunctionNode> a;
	      a.push_back(root.release());

	      const TransformIdentity ti;
	      std::vector<real> tiv=ti.get_columns();
	      std::vector<real> p;
	      p.insert(p.end(),tiv.begin(),tiv.end());
	      p.insert(p.end(),tiv.begin(),tiv.end());
	      
	      root=std::unique_ptr<FunctionTop>(new FunctionTop(p,a,0));
	    }
	  assert(root->is_a_FunctionTop());
	  std::unique_ptr<FunctionTop> root_as_top(root.release()->is_a_FunctionTop());  // Interestingly, if is_a_FunctionTop threw, the root would be leaked.
	  return boost::shared_ptr<const MutatableImage>(new MutatableImage(root_as_top,sinusoidal_z,spheremap,false));
	}
      else
	{
	  return boost::shared_ptr<const MutatableImage>();
	}
    }
  else
    {
      QString tmp;
      tmp = "Parse error: "+load_handler.errorString()+"\n";
      report=tmp.toLocal8Bit().data();
      return boost::shared_ptr<const MutatableImage>();
    }
}
