// Source file for evolvotron
// Copyright (C) 2005 Tim Day
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

#include <qgroupbox.h>

#include "dialog_favourite.h"
#include "function_registry.h"
#include "evolvotron_main.h"

DialogFavourite::DialogFavourite(EvolvotronMain* parent)
  :QDialog(parent)
  ,_parent(parent)
{
  setCaption("Favourite");
  
  _dialog_content=new QVBox(this);
  
  QGroupBox* group0=new QGroupBox(1,Qt::Horizontal,"Favourite",_dialog_content);

  new QLabel("The function type used as the root node of new image functions:",group0,0,Qt::WordBreak);
  _favourite=new QComboBox(false,group0);
  _favourite_fn_to_index[""]=_favourite->count();
  _index_to_favourite_fn[_favourite->count()]="";
  _favourite->insertItem("- No preference -");

  for (std::vector<const FunctionRegistration*>::const_iterator it=FunctionRegistry::get()->registrations().begin();
       it!=FunctionRegistry::get()->registrations().end();
       it++
       )
    {
      const FunctionRegistration*const fn=(*it);
      _favourite_fn_to_index[fn->name()]=_favourite->count();
      _index_to_favourite_fn[_favourite->count()]=fn->name();
      _favourite->insertItem(fn->name());      
    }

  std::cerr << _parent->favourite_function() << "\n";

  QGroupBox* group1=new QGroupBox(1,Qt::Horizontal,"Wrapping",_dialog_content);
  
  _unwrapped=new QCheckBox("Root node is UNWRAPPED",group1);

  update();
}

DialogFavourite::~DialogFavourite()
{}

void DialogFavourite::update()
{
  const std::map<std::string,unsigned int>::const_iterator it=_favourite_fn_to_index.find(_parent->favourite_function());
  if (it!=_favourite_fn_to_index.end())
    _favourite->setCurrentItem((*it).second);

  _unwrapped->setChecked(_parent->favourite_function_unwrapped());
}

void DialogFavourite::resizeEvent(QResizeEvent* e)
{
  Superclass::resizeEvent(e);
  _dialog_content->resize(size());
}
