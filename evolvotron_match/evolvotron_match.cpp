// Source file for evolvotron
// Copyright (C) 2004 Tim Day
/*! \page License License

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
  \brief Mutate functions searching for a match to a given image.
*/

#include "evolvotron_match_precompiled.h"

#include "args.h"
#include "mutatable_image.h"
#include "mutation_parameters.h"

//! A QImage which deletes it's data on destruction
class QImageWithData
{
public:
  QImageWithData(uchar* data,int w,int h,int depth,QRgb* colortable,int numColors,QImage::Endian bitOrder)
    :_image(new QImage(data,w,h,depth,colortable,numColors,bitOrder))
    ,_data(data)
  {}

  virtual ~QImageWithData()
  {}

  QImage*const image() 
  {
    return _image.get();
  }
  const QImage*const image() const
  {
    return _image.get();
  }

protected:
  const std::auto_ptr<QImage> _image;
  const std::auto_ptr<uchar> _data;
};

//! Render a new QImage of the given mutatable image
QImageWithData*const render_image(const MutatableImage*const imagefn,int width,int height)
{
  uint*const image_data=new uint[width*height];
  uint* pixel=image_data;
  
  for (int row=0;row<height;row++)
    for (int col=0;col<width;col++,pixel++)
      {
	// xy co-ords vary over -1.0 to 1.0.  In the one frame case z will be 0
	const XYZ p(
		    -1.0+2.0*(col+0.5)/width,
		    1.0-2.0*(row+0.5)/height,
		    0.0
		    );

	const XYZ c(imagefn->get_rgb(p));

	const uint col0=lrint(clamped(c.x(),0.0,255.0));
	const uint col1=lrint(clamped(c.y(),0.0,255.0));
	const uint col2=lrint(clamped(c.z(),0.0,255.0));
	
	*pixel=((col0<<16)|(col1<<8)|(col2));
      }

  return new QImageWithData
    (
     reinterpret_cast<uchar*>(image_data),
     width,
     height,
     32,
     0,
     0,
     QImage::LittleEndian
     );
}

const double compare_images(const QImage* src0,const QImage* src1)
{
  double t=0.0;

  assert(src0->width()==src1->width() && src0->height()==src1->height());

  for (int y=0;y<src0->height();y++)
    {
      for (int x=0;x<src0->width();x++)
	{
	  //! \todo Optimise pixel accesses.
	  const QRgb col0(src0->pixel(x,y));
	  const QRgb col1(src1->pixel(x,y));

	  const real r0=qRed(col0);
	  const real r1=qRed(col1);
	  const real g0=qGreen(col0);
	  const real g1=qGreen(col1);
	  const real b0=qBlue(col0);
	  const real b1=qBlue(col1);
	  t+=sqr(r0-r1)+sqr(g0-g1)+sqr(b0-b1);
	}
    }

  return t/(255.0*src0->width()*src0->height());
}

//! Application code
int main(int argc,char* argv[])
{
  {
    Args args(argc,argv);

    if (args.option("-v")) 
      std::clog.rdbuf(std::cerr.rdbuf());
    else
      std::clog.rdbuf(sink_ostream.rdbuf());
  
    // Normally would use time(0) to seed random number generator
    // but can imagine several of these starting up virtually simultaneously
    // so need something with higher resolution.
    // Adding the process id too to keep things unique.
  
    QTime t(QTime::currentTime());
    uint seed=getpid()+t.msec()+1000*t.second()+60000*t.minute()+3600000*t.hour();   
    std::clog << "Random seed is " << seed << "\n";
   
    MutationParameters mutation_parameters(seed,false,false);
 
    if (argc<2)
      {
	std::cerr << "Must supply a filename\n";
	exit(1);
      }

    QImage target_image(args.last(1).c_str());
    if (target_image.isNull())
      {
	std::cerr << "Couldn't load target image\n";
	exit(1);
      }

    const uint fresh=10;
    const uint reset=100;
    const real cooling=0.9;
    const uint iterations=10000000;

    std::clog << "Cumulative cooling effect is " << pow(cooling,reset) << " over cycle\n";
  
    double best_score=0.0;
    boost::shared_ptr<const QImageWithData> best_image;
    boost::shared_ptr<const MutatableImage> best_imagefn;
  
    for (uint i=0,output_frame=0;i<iterations;i++)
      {
	if (i%reset==0)
	  {
	    mutation_parameters.reset();
	  }
	else
	  {
	    mutation_parameters.general_cool(cooling);
	  }

	boost::shared_ptr<const MutatableImage> new_imagefn;

	if (best_imagefn==0 || i%fresh==0)
	  {
	    do 
	      {
		new_imagefn=boost::shared_ptr<const MutatableImage>(new MutatableImage(mutation_parameters,true,true,false));
	      }
	    while (new_imagefn->is_constant());
	  }
	else
	  {
	    new_imagefn=boost::shared_ptr<const MutatableImage>(best_imagefn->mutated(mutation_parameters));
	  }

      
	boost::shared_ptr<const QImageWithData> new_image
	  =boost::shared_ptr<const QImageWithData>(render_image(new_imagefn.get(),target_image.width(),target_image.height()));

	const double score=compare_images(new_image->image(),&target_image);

	if (best_image.get()==0 || score<best_score)
	  {
	    best_score=score;
	    best_image=new_image;
	    best_imagefn=new_imagefn;
	    std::stringstream filename;
	    filename << "f" << std::setw(6) << std::setfill('0') << output_frame++ << ".png";
	    best_image->image()->save(filename.str().c_str(),"PNG");
	    std::clog << "[" << best_score << "]";
	  }
	else
	  {
	    std::clog << ".";
	  }
      }

    best_image->image()->save("match.png","PNG");
  }

#ifndef NDEBUG
  assert(InstanceCounted::is_clear());
#endif
	 
	 exit(0);
}
