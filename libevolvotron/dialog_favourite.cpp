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
  \brief Implementation of class DialogFavourite.
*/



#include "dialog_favourite.h"

#include "function_registry.h"
#include "evolvotron_main.h"

DialogFavourite::DialogFavourite(EvolvotronMain* parent)
  :QDialog(parent)
  ,_parent(parent)
  ,_favourite_function_unwrapped(false)
{
  setWindowTitle("Favourite");
  setSizeGripEnabled(true);

  setLayout(new QVBoxLayout);
  
  QGroupBox*const group0=new QGroupBox("Function");
  group0->setLayout(new QVBoxLayout);
  layout()->addWidget(group0);

  group0->layout()->addWidget(new QLabel("Root node for new image functions:"));
  _favourite=new QComboBox;
  group0->layout()->addWidget(_favourite);
  _favourite_fn_to_index[""]=_favourite->count();
  _index_to_favourite_fn[_favourite->count()]="";
  _favourite->addItem("- No preference -");

  for (FunctionRegistry::Registrations::const_iterator it=_parent->mutation_parameters().function_registry().registrations().begin();
       it!=_parent->mutation_parameters().function_registry().registrations().end();
       it++
       )
    {
      const FunctionRegistration& fn=
#if BOOST_VERSION >= 103400
      *(it->second);
#else
      *it;
#endif
      _favourite_fn_to_index[fn.name()]=_favourite->count();
      _index_to_favourite_fn[_favourite->count()]=fn.name();
      _favourite->addItem(fn.name().c_str());
    }

  QGroupBox*const group1=new QGroupBox("Wrapping");
  group1->setLayout(new QVBoxLayout);
  layout()->addWidget(group1);
  
  _unwrapped=new QCheckBox("Disable additional space/colour transforms");
  group1->layout()->addWidget(_unwrapped);

  //! \todo: Add OK & reset/restart versions ?
  QPushButton*const ok=new QPushButton("OK");
  layout()->addWidget(ok);
  ok->setDefault(true);

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
    _favourite->setCurrentIndex((*it).second);
  else
    _favourite->setCurrentIndex(0);

  _unwrapped->setChecked(_favourite_function_unwrapped);

  _unwrapped->setEnabled(!_favourite_function.empty());
}

bool DialogFavourite::favourite_function(const std::string& f)
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
