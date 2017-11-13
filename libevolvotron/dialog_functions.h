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
  \brief Interface for class DialogFunctions.
*/

#ifndef _dialog_functions_h_
#define _dialog_functions_h_

#include "common.h"

#include "mutation_parameters_qobject.h"

class EvolvotronMain;

//! Utility class for DialogFunctions.  Expands changed(int) to changed(src,int)
/*! Would ideally live in dialog_functions.cpp, but that causes (moc-related?) problems with linking;
  seems to need wider visibility.  \todo Move to own file and only include in dialog_functions.cpp
 */
class SignalExpanderValueChangedQSlider : public QObject
{
private:
  Q_OBJECT
  
  QSlider*const _src;
public:
  SignalExpanderValueChangedQSlider(QObject* parent,QSlider* src)
    :QObject(parent)
    ,_src(src)
  {}
public slots:
  void valueChanged(int v)
  {
    emit valueChanged(_src,v);
  }

signals:
  void valueChanged(QSlider*,int);
};

//! Similar to SignalExpanderQSlider except attaches an integer argument to the clicked signal
class SignalExpanderClickedUint : public QObject
{
 private:
  Q_OBJECT

  uint _arg;
 public:
  SignalExpanderClickedUint(QObject* parent,uint arg)
    :QObject(parent)
    ,_arg(arg)
    {}
 public slots:
  void clicked()
    {
      emit clicked(_arg);
    }
 signals:
 void clicked(uint);
};

//! Provides a dialog for controlling which functions are available.
class DialogFunctions : public QDialog
{
 private:
  Q_OBJECT

 protected:
  //! Owner of dialog
  EvolvotronMain*const _parent;

  //! Instance of MutationParameters under dialog control.
  /*! \warning Careful of modifying things which might make DialogMutationParameters get out of sync
   */
  MutationParametersQObject*const _mutation_parameters;

  //! Top level holder of all the dialog content.
  QWidget* _dialog_content;

  //! Notification of undiluted branching ratio.
  QLabel* _branching_ratio;

  //! Required branching ratio after dilution
  QSlider* _slider_target_branching_ratio;

  //! Proportion of diluting nodes which are pure constants
  QSlider* _slider_proportion_constant;

  //! Proportion of non-constant nodes which are 12-parameter transforms
  QSlider* _slider_identity_supression;
  
  //! Lookup from each slider in the weighting controls area to corresponding function.
  std::map<QSlider*,const FunctionRegistration*> _slider_to_function;

 public:
  //! Constructor.
  DialogFunctions(EvolvotronMain* parent,MutationParametersQObject* mp);

  //! Destructor.
  ~DialogFunctions();

  //! Reload from _mutation_parameters
  void setup_from_mutation_parameters();

 protected slots:
   
   //@{
   //! Signalled by sliders.
   void changed_target_branching_ratio(int v);
   void changed_proportion_constant(int v);
   void changed_identity_supression(int v);
   void changed_function_weighting(QSlider*,int v);
   //@}

   //! Signalled by randomization methods
   void clicked_button_rand(uint mask);

 public slots:
  //! Signalled by mutation parameters
  void mutation_parameters_changed();
};

#endif
