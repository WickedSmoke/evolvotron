#include <stdexcept>

#include <QApplication>
#include <QGLWidget>
#include <QSvgRenderer>

#include <QtOpenGL/qgl.h>

#include <QtSvg>

// Following file:///usr/share/doc/qt4-doc-html/html/opengl-overpainting.html
// for combining OpenGL with 2D

// Looks nicer run with e.g __GL_FSAA_MODE=13 ./svg

// TODO: Benchmark.  Try and capture SVG rendering in a display list.

class View : public QGLWidget
{
public:
  View()
    :QGLWidget()
    ,_renderer(this)
  {
    setAutoFillBackground(false);

    if (!_renderer.load(QString("drawing.svg")))
      throw std::runtime_error(__PRETTY_FUNCTION__+std::string(": loading SVG file failed\n"));
  }

  void initializeGL()
  {}

  //Don't use paintGL when combining 2D/3D
  //void paintGL()
  //{}

  void paintEvent(QPaintEvent*) 
  {
    glViewport(0,0,static_cast<GLint>(width()),static_cast<GLint>(height()));

    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //glEnable(GL_MULTISAMPLE);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    _renderer.render(&painter);
    painter.end();
  }
protected:
  void resizeGL(int width,int height)
  {
    glViewport(0,0,static_cast<GLint>(width),static_cast<GLint>(height));
  }
private:
  QSvgRenderer _renderer;
};

int main(int argc,char** argv)
{
  QApplication app(argc,argv);

  QWidget*const w=new View();

  w->show();

  return app.exec();
}

