#ifndef H_GL_WIDGET_H
#define H_GL_WIDGET_H

#include "hgl.h"
#include "hframe.h"
#include "hgui.h"
#include <qopenglwidget.h>
#include <atomic>

void bindTexture(GLTexture* tex, QImage* img);

class HGLWidget : public QOpenGLWidget
{
public:
    HGLWidget(QWidget* parent = NULL);

    void setVertices(double ratio);
    void setVertices(QRect rc);

    void drawFrame(HFrame *pFrame);
    void drawTexture(HRect rc, GLTexture *tex);
    void drawRect(HRect rc, HColor clr, int line_width = 1, bool bFill = false);
    void drawText(QPoint lb, const char* text, int fontsize, QColor clr);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

    static void loadYUVShader();
    void initVAO();
    void initYUV();

    void drawYUV(HFrame* pFrame);

protected:
    static std::atomic_flag s_init_flag;
    static GLuint prog_yuv;
    static GLuint texUniformY;
    static GLuint texUniformU;
    static GLuint texUniformV;
    GLuint  tex_yuv[3];

    GLfloat vertices[8];
    GLfloat textures[8];

    enum VER_ATTR{
        VER_ATTR_VER,
        VER_ATTR_TEX,
        VER_ATTR_NUM
    };
};

#endif // H_GL_WIDGET_H
