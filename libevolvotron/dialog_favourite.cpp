// Source file for evolvotron
// Copyright (C) 2007 Tim Day
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
  \brief Implementation of class DialogFavourite.
*/

#include "libevolvotron_precompiled.h"

#include "dialog_favourite.h"

#include <qgroupbox.h>

#include "function_registry.h"
#include "evolvotron_main.h"

DialogFavourite::DialogFavourite(EvolvotronMain* parent)
  :QDialog(parent)
  ,_parent(parent)
  ,_favourite_function_unwrapped(false)
{
  setCaption("Favourite");
  
  _dialog_content=new QVBox(this);
  
  QGroupBox* group0=new QGroupBox(1,Qt::Horizontal,"Function",_dialog_content);

  new QLabel("Root node for new image functions:",group0,0,Qt::WordBreak);
  _favourite=new QComboBox(false,group0);
  _favourite_fn_to_index[""]=_favourite->count();
  _index_to_favourite_fn[_favourite->count()]="";
  _favourite->insertItem("- No preference -");

  for (FunctionRegistry::Registrations::const_iterator it=_parent->mutation_parameters().function_registry().registrations().begin();
       it!=_parent->mutation_parameters().function_registry().registrations().end();
       it++
       )
    {
      const FunctionRegistration& fn=
#if BOOST_VERSION >= 013400
      **(it->second);
#else
      **it;
#endif
      _favourite_fn_to_index[fn.name()]=_favourite->count();
      _index_to_favourite_fn[_favourite->count()]=fn.name();
      _favourite->insertItem(fn.name());
    }

  QGroupBox* group1=new QGroupBox(1,Qt::Horizontal,"Wrapping",_dialog_content);
  
  _unwrapped=new QCheckBox("Disable additional space/colour transforms\n",group1);

  //! \todo: Add OK & reset/restart versions ?
  QPushButton* ok=new QPushButton("OK",_dialog_content);

  update_gui_from_state();

  connect(_favourite,SIGNAL(activated(int)),this,SLOT(changed_favourite(int)));
  
  connect(_unwrapped,SIGNAL(toggled(bool)),this,SLOT(changed_unwrapped(bool)));

  connect(ok,SIGNAL(clicked()),this,SLOT(hide()));
}

DialogFavourite::~DialogFavourite()
{}

void DialogFavourite::changed_favourite(int i)
{
  const std::map<unsigned int,std::string>::const_iterator it=_index_to_favourite_fn.find(i);
  if (it!=_index_to_favourite_fn.end())
    {
      _favourite_function=(*it).second;
    }

  update_gui_from_state();
}

void DialogFavourite::changed_unwrapped(bool b)
{
  _favourite_function_unwrapped=b;

  update_gui_from_state();
}


void DialogFavourite::update_gui_from_state()
{
  const std::map<std::string,unsigned int>::const_iterator it=_favourite_fn_to_index.find(_favourite_function);
  if (it!=_favourite_fn_to_index.end())
    _favourite->setCurrentItem((*it).second);
  else
    _favourite->setCurrentItem(0);

  _unwrapped->setChecked(_favourite_function_unwrapped);

  _unwrapped->setEnabled(!_favourite_function.empty());
}

void DialogFavourite::resizeEvent(QResizeEvent* e)
{
  Superclass::resizeEvent(e);
  _dialog_content->resize(size());
}

const bool DialogFavourite::favourite_function(const std::string& f)
{
  if (_parent->mutation_parameters().function_registry().lookup(f))
    {
      _favourite_function=f;
      update_gui_from_state();
      return true;
    }
  else
    return false;
}

void DialogFavourite::favourite_function_unwrapped(bool v)
{
  _favourite_function_unwrapped=v;
  update_gui_from_state();
}
