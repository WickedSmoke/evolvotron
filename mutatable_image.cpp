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


void MutatableImage::get_rgb(const XYZ& p,uint c[3]) const
{
  // Actually calculate a pixel value from the image.
  // The nominal range is -1.0 to 1.0
  XYZ pv((*root())(p));

  // Use smooth tanh to avoid hard clamping.
  pv.x(tanh(pv.x()));
  pv.y(tanh(pv.y()));
  pv.z(tanh(pv.z()));

  // Scale nominal -1.0 to 1.0 range to 0-255
  XYZ v(127.5*(pv+XYZ(1.0,1.0,1.0)));
  
  // Clamp out of range values just in case
  v.x(clamped(v.x(),0.0f,255.0f));
  v.y(clamped(v.y(),0.0f,255.0f));
  v.z(clamped(v.z(),0.0f,255.0f));
		  
  c[0]=(uint)floorf(v.x());
  c[1]=(uint)floorf(v.y());	  
  c[2]=(uint)floorf(v.z());
}

std::ostream& MutatableImage::save_function(std::ostream& out) const
{
  out << "<?xml version=\"1.0\"?>\n";
  out << "<evolvotron-image-function version=\"" << EVOLVOTRON_VERSION << "\">\n";
  
  root()->save_function(out,1);

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
  std::stack<FunctionNodeInfo*> _stack;

  bool _expect_top_level_element;
  bool _expect_characters;
  bool _expect_characters_type;
  bool _expect_characters_iterations;
  bool _expect_characters_parameter;
public:
  LoadHandler(FunctionNodeInfo** root)
    :_root(root)
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
   */
  bool startElement(const QString&,const QString& localName,const QString&,const QXmlAttributes& atts)
  {
    const std::string element(localName);

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
		    _report+="Warning: File saved from a different evolvotron version: "+version+"\n(This is version "+QString(EVOLVOTRON_VERSION)+")\n";
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
   */
  bool endElement(const QString&, const QString& localName, const QString&)
  {
    const std::string element(localName);

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
	    _report+="Error: Unexpected character data : \""+s+"\"\n";
	    return false;
	  }
      }

    //std::clog << "Non-blank expected character data\n";

    if (_expect_characters_type)
      {
	_stack.top()->type((const char*)s);
	_expect_characters_type=false;
      }
    else if (_expect_characters_iterations)
      {
	bool ok;
	_stack.top()->iterations(s.toUInt(&ok));
	_expect_characters_iterations=false;
	if (!ok)
	  {
	    _report+="Error: Couldn't parse \""+s+"\" as an integer\n";
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
	    _report+="Error: Couldn't parse \""+s+"\" as a float\n";
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

  LoadHandler load_handler(&info);

  QXmlSimpleReader xml_reader;
  xml_reader.setContentHandler(&load_handler);

  const bool ok=xml_reader.parse(&xml_source,false);

  if (ok)
    {
      // Might be a warning message in there.
      report=load_handler.errorString();

      FunctionNode*const root=FunctionNode::create(info,report);
      delete info;
      
      if (root)
	{
	  return new MutatableImage(root);
	}
      else
	{
	  return 0;
	}
    }
  else
    {
      report="Parse error: "+load_handler.errorString()+"\n";
      delete info;
      return 0;
    }
}


