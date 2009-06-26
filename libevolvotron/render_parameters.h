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
  \brief Interface for class RenderParameters
*/

#ifndef _render_parameters_h_
#define _render_parameters_h_

template <typename T> bool change(T& dst,const T& src)
{
  const T previous=dst;
  dst=src;
  return (dst!=previous);
}

//! Class encapsulating things affecting rendering
class RenderParameters : public QObject
{
  Q_OBJECT;

 public:
  RenderParameters(bool jitter,uint multisample,QObject* parent);
  ~RenderParameters();

  //! Accessor.
  bool jittered_samples() const
    {
      return _jittered_samples;
    }

  //! Accessor.
  void jittered_samples(bool v)
    {
      if (change(_jittered_samples,v)) report_change();
    }

  //! Accessor.
  uint multisample_grid() const
    {
      assert(_multisample_grid>=1);
      return _multisample_grid;
    }

  //! Accessor.
  void multisample_grid(uint v)
    {
      assert(v>=1);
      if (change(_multisample_grid,v)) report_change();
    }

signals:
  void changed();

 protected:
  void report_change();

  private:

  //! Whether sample points should be randomized.
  bool _jittered_samples;

  //! Grid for multisampling.
  /*! Default is 1.  4 would be 16 samples in a 4x4 grid.
   */
  uint _multisample_grid;
};


#endif
