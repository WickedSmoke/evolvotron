// Source file for evolvotron
// Copyright (C) 2002,2003,2004 Tim Day
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
  \brief Implementation of class DialogFunctions.
*/

#include <iostream>

#include <qtooltip.h>
#include <qgroupbox.h>
#include <qtabwidget.h>

#include "vbox_scrollview.h"

#include "dialog_functions.h"
#include "function_registry.h"

/*! About dialog displays author info, web addresses and license info.
 */
DialogFunctions::DialogFunctions(QMainWindow* parent,MutationParameters* mp)
  :QDialog(parent)
  ,_parent(parent)
  ,_mutation_parameters(mp)
{
  assert(_parent!=0);

  setCaption("Functions");

  _dialog_content=new QVBox(this);

  QTabWidget* tabs=new QTabWidget(_dialog_content);
  _ok=new QPushButton("OK",_dialog_content);
    
  for (int c=-1;c<FnClassifications;c++)
    {
      VBoxScrollView* scrollview=new VBoxScrollView(this);
      tabs->addTab(scrollview,(c==-1 ? "All" : function_classification_name[c]));

      //! \todo Add buttons to affect all items in group

      for (std::vector<const FunctionRegistration*>::const_iterator it=FunctionRegistry::get()->registrations().begin();
	   it!=FunctionRegistry::get()->registrations().end();
	   it++)
	{
	  const FunctionRegistration*const fn=(*it);
	  if (c==-1 || fn->classification()&(1<<c))
	    {
	      QGroupBox* g=new QGroupBox(3,Qt::Horizontal,fn->name(),scrollview->contentParent());
	      
	      QSizePolicy spx(QSizePolicy::Expanding,QSizePolicy::Preferred);
	      g->setSizePolicy(spx);
	      
	      new QLabel("2^-10",g);
	      QSlider* s=new QSlider(-10,0,1,0,Qt::Horizontal,g);
	      s->setSizePolicy(spx);
	      new QLabel("1",g);
	      
	      _slider_to_function.insert(std::make_pair(s,fn));

	      SignalExpanderQSlider*const sx=new SignalExpanderQSlider(this,s);
	      connect(
		      s,SIGNAL(valueChanged(int)),
		      sx,SLOT(valueChanged(int))
		      );
	      connect(
		      sx,SIGNAL(valueChanged(QSlider*,int)),
		      this,SLOT(changed_function_weighting(QSlider*,int))
		      );
	    }
	}
    }

  // And add another tab for all the branching-ratio/dilution controls
  {
    QVBox* vbox=new QVBox(this);
    tabs->addTab(vbox,"Dilution");
    
    _branching_ratio=new QLabel(vbox);
    
    QGroupBox* c0=new QGroupBox(3,Qt::Horizontal,"Required branching ratio after dilution",vbox);
    new QLabel("0.1",c0);
    _slider_target_branching_ratio=new QSlider(10,90,1,50,Qt::Horizontal,c0);
    QToolTip::add(_slider_target_branching_ratio,"The branching ratio must be diluted to <1.0 to prevent formation of infinitely large function-trees.\nWarning: setting a high value results in complex function trees taking a long time to compute.\nSetting a low value results in very simple images.");
    new QLabel("0.9",c0);

    QGroupBox* c1=new QGroupBox(3,Qt::Horizontal,"Of diluting nodes, proportion constant:",vbox);
    new QLabel("0.0",c1);
    _slider_proportion_constant=new QSlider(0,100,1,50,Qt::Horizontal,c1);
    new QLabel("1.0",c1);
    QToolTip::add(_slider_proportion_constant,"This specifies the proportion of diluting nodes which will be constant.");
    
    QGroupBox* c2=new QGroupBox(3,Qt::Horizontal,"Of non-constant diluting nodes, proportion transforms",vbox);
    new QLabel("0.0",c2);
    _slider_identity_supression=new QSlider(0,100,1,50,Qt::Horizontal,c2);
    QToolTip::add(_slider_identity_supression,"This specifies the proportion of non-constant diluting nodes which will be transforms (c.f identity nodes).");
    new QLabel("1.0",c2);
  }

  connect(
	  _slider_target_branching_ratio,SIGNAL(valueChanged(int)),
	  this,SLOT(changed_target_branching_ratio(int))
	  );

  connect(
          _ok,SIGNAL(clicked()),
          this,SLOT(hide())
          );

  connect(
	  _slider_proportion_constant,SIGNAL(valueChanged(int)),
	  this,SLOT(changed_proportion_constant(int))
	  );

  connect(
	  _slider_identity_supression,SIGNAL(valueChanged(int)),
	  this,SLOT(changed_identity_supression(int))
	  );

  setup_from_mutation_parameters();

  connect(
	  _mutation_parameters,SIGNAL(changed()),
	  this,SLOT(mutation_parameters_changed())
	  );
}

DialogFunctions::~DialogFunctions()
{}

void DialogFunctions::resizeEvent(QResizeEvent* e)
{
  Superclass::resizeEvent(e);
  _dialog_content->resize(size());
}

void DialogFunctions::setup_from_mutation_parameters()
{
  const float b=_mutation_parameters->random_function_branching_ratio();
  const float s=_mutation_parameters->proportion_basic();
  const float br=(1.0f-s)*b;

  std::stringstream msg;
  msg 
    << "Undiluted random function branching ratio is " << b << "\n"
    << "Diliuting in proportion " << s << " to obtain required branching ratio.";
  _branching_ratio->setText(msg.str().c_str());

  _slider_target_branching_ratio->setValue(static_cast<int>(100.0f*br+0.5f));
  _slider_proportion_constant->setValue(static_cast<int>(0.5f+100.0f*_mutation_parameters->proportion_constant()));
  _slider_identity_supression->setValue(static_cast<int>(0.5f+100.0f*_mutation_parameters->identity_supression()));

  for (std::map<QSlider*,const FunctionRegistration*>::const_iterator it=_slider_to_function.begin();
       it!=_slider_to_function.end();
       it++
       )
    {
      const float w=_mutation_parameters->get_weighting((*it).second);
      const int iw=static_cast<int>(floor(0.5f+(log(w)*(1.0/M_LN2))));
      if (iw!=(*it).first->value())
	{
	  (*it).first->setValue(iw);
	}
    }
}

void DialogFunctions::changed_target_branching_ratio(int v)
{
  const float target_branching_ratio=v/100.0f;
  
  // Want to solve tgt=proportion_basic*0.0+(1.0-proportion_basic)*random_function_branching_ratio
  const float proportion_basic=1.0f-target_branching_ratio/_mutation_parameters->random_function_branching_ratio();

  std::clog 
    << "Basic-node dilution proportion set to " 
    << proportion_basic 
    << " giving a branching ratio of "
    << (1.0-proportion_basic)*_mutation_parameters->random_function_branching_ratio()
    << "\n";

  _mutation_parameters->proportion_basic(proportion_basic);

  setup_from_mutation_parameters();
}

void DialogFunctions::changed_proportion_constant(int v)
{
  _mutation_parameters->proportion_constant(v/100.0f);
}

void DialogFunctions::changed_identity_supression(int v)
{
  _mutation_parameters->identity_supression(v/100.0f);
}

void DialogFunctions::changed_function_weighting(QSlider* s,int v)
{
  std::map<QSlider*,const FunctionRegistration*>::const_iterator it=_slider_to_function.find(s);
  if (it==_slider_to_function.end())
    {
      std::clog << "DialogFunctions::changed_function_weighting : unknown source slider, ignoring\n";
    }
  else
    {
      const FunctionRegistration* fn=(*it).second;
      
      const float w=pow(2,v);
      std::clog << fn->name() << " weighting changed to " << w << "\n";
      _mutation_parameters->change_function_weighting(fn,w);
    }
}

void DialogFunctions::mutation_parameters_changed()
{
  std::clog << "[DialogFunctions::mutation_parameters_changed()]\n";
  setup_from_mutation_parameters();  
}
