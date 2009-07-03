#include <stdexcept>

#include <QApplication>
#include <QGLWidget>
#include <QSvgRenderer>

#include <QtOpenGL/qgl.h>

#include <QtSvg>

// Follow file:///usr/share/doc/qt4-doc-html/html/opengl-overpainting.html
// for combining OpenGL with 2D

class View : public QGLWidget
{
public:
  View()
    :_renderer(this)
  {
    setAutoFillBackground(false);

    if (!_renderer.load(QString("drawing.svg")))
      throw std::runtime_error(__PRETTY_FUNCTION__+std::string(": loading SVG file failed\n"));
  }
  void initializeGL()
  {}
  /*
  void paintGL()
  {
    QPainter painter;
    painter.begin(this);
    painter.end();
  }
  */
  void paintEvent(QPaintEvent*) 
  {
    glViewport(0,0,static_cast<GLint>(width()),static_cast<GLint>(height()));

    glClearColor(0.25f,0.25f,.25f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    QPainter painter;
    painter.begin(this);
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

