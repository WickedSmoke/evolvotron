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

MutatableImage::MutatableImage(const FunctionNodeInfo* info,bool& ok,std::string& report)
  :_root(0/*new FunctionNode(info,ok)*/)
{
  ok=false;
  report="Initialisation from FunctionNodeInfo not yet implemented";
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
	_report="No root function node found";
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

    std::clog << "StartElement : " /*<< element*/ << "\n";

    if (_expect_characters)
      {
	_report="Expected character data but got start element \""+element+"\"";
	return false;
      }

    if (_expect_top_level_element)
      {
	if (element=="evolvotron-image-function")
	  {
	    std::clog << "Element: evolvotron-image-function\n";
	    _expect_top_level_element=false;
	    //! \todo Check version number attribute
	  }
	else
	  {
	    _report="Expected <evolvotron-image-function> but got \""+element+"\"";
	    return false;
	  }
      }
    else
      {
	if (element=="f")
	  {
	    std::clog << "Element: f\n";
	    FunctionNodeInfo*const f=new FunctionNodeInfo;
	    if (_stack.empty())
	      {
		if (*_root==0)
		  {
		    *_root=f;
		  }
		else
		  {
		    _report="Multiple top level <f> elements encountered";
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
	    std::clog << "Element: type\n";
	    _expect_characters=true;
	    _expect_characters_type=true;
	  }
	else if (element=="i")
	  {
	    std::clog << "Element: i\n";
	    _expect_characters=true;
	    _expect_characters_iterations=true;
	  }
	else if (element=="p")
	  {
	    std::clog << "Element: p\n";
	    _expect_characters=true;
	    _expect_characters_parameter=true;
	  }
	else 
	  {
	    _report="Expected <f>, <type>, <i> or <p> but got \""+element+"\"";
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
	_report="Expected character data but got end element \""+element+"\"";
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
	    _report="Unexpected character data : \""+s+"\"";
	    return false;
	  }
      }

    std::clog << "Non-blank expected character data\n";

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
	    _report="Couldn't parse \""+s+"\" as an integer";
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
	    _report="Couldn't parse \""+s+"\" as a float";
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
  FunctionNodeInfo* root=0;

  LoadHandler load_handler(&root);

  QXmlSimpleReader xml_reader;
  xml_reader.setContentHandler(&load_handler);

  const bool ok=xml_reader.parse(&xml_source,false);

  if (ok)
    {
      report="Function load not yet implemented\n";

      bool iok;
      std::string ireport;
      MutatableImage*const ret=new MutatableImage(root,iok,ireport);
      delete root;

      if (iok)
	{
	  return ret;
	}
      else
	{
	  report="Parsed file OK, but encountered problem using it:\n"+ireport;
	  delete ret;
	  return 0;
	}
    }
  else
    {
      report="Parse error: "+load_handler.errorString()+"\n";
      delete root;
      return 0;
    }
}

