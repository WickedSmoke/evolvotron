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
#include <qxml.h>

#include "mutatable_image.h"

std::ostream& MutatableImage::save_function(std::ostream& out) const
{
  out << "<?xml version=\"1.0\"?>\n";
  out << "<evolvotron-image version=\"" << EVOLVOTRON_VERSION << "\">\n";
  
  root()->save_function(out,1);

  out << "</evolvotron-image>\n";

  return out;
}


class LoadHandler : public QXmlDefaultHandler
{
public:
  bool startElement(const QString& namespaceURI,const QString& localName,const QString &qName,const QXmlAttributes& atts)
  {
    //std::clog << "Element\n";
    std::clog << "Element " << (const char*)localName << "\n";
    //std::clog << "Start element: " << (const char*)namespaceURI << "/" << (const char*)localName << "/" << (const char*)qName << "\n";
    return true;
  }

  bool characters(const QString& s)
  {
    //std::clog << "Char\n";
    std::clog << "Characters: " << (const char*)s << "\n";
    return true;
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

  LoadHandler load_handler;

  QXmlSimpleReader xml_reader;
  xml_reader.setContentHandler(&load_handler);

  const bool ok=xml_reader.parse(&xml_source,false);

  if (ok)
    report="Function load not yet implemented\n";
  else
    report="Parse error\n";
  return 0;
}

