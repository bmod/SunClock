#include "renderimage.h"

#include <QImage>
#include <QOffscreenSurface>
#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QSurfaceFormat>
#include <suncalc.h>

void __sAssert(const bool condition, const QString& message) {
    if (!condition) {
        qFatal("ERRORR: %s", message.toStdString().c_str());
    }
}

QImage sRenderImage(const QSize& size, const std::chrono::system_clock::time_point& time, float lat, float lng) {



    //   =======CONTEXT SETUP======

    //   Set OpenGL version to use
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setMajorVersion(4);
    surfaceFormat.setMinorVersion(3);

    QOpenGLContext openGLContext;
    openGLContext.setFormat(surfaceFormat);
    openGLContext.create();
    if (!openGLContext.isValid())
        qFatal("Unable to create context");

    QOffscreenSurface surface;
    surface.setFormat(surfaceFormat);
    surface.create();
    if (!surface.isValid())
        qFatal("Unable to create the Offscreen surface");

    if (!openGLContext.makeCurrent(&surface))
        qFatal("Unable to make OpenGL context current");

    //   Viewport size
    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    QOpenGLFramebufferObject fbo(size, fboFormat);
    openGLContext.functions()->glViewport(0, 0, size.width(), size.height());
    fbo.bind();

    //    ========GEOMEETRY SETUP========
    // clang-format off
    static const float vertexPositions[] = {
        -1.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, -1.0f,
    };

    static const float vertexColors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
    };
    // clang-format on
    QOpenGLBuffer vertexPositionBuffer(QOpenGLBuffer::VertexBuffer);
    vertexPositionBuffer.create();
    vertexPositionBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vertexPositionBuffer.bind();
    vertexPositionBuffer.allocate(vertexPositions, 8 * sizeof(float));

    QOpenGLBuffer vertexColorBuffer(QOpenGLBuffer::VertexBuffer);
    vertexColorBuffer.create();
    vertexColorBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vertexColorBuffer.bind();
    vertexColorBuffer.allocate(vertexColors, 12 * sizeof(float));

    bool re;
    QOpenGLShaderProgram program;
    re = program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/sky.vert");
    __sAssert(re, "Failed to load vertex shader");

    re = program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/sky.frag");
    __sAssert(re, "Failed to load fragment shader");

    // program.addShaderFromSourceCode(QOpenGLShader::Vertex, "#version 330\r\n"
    //                                                        "in vec2 position;\n"
    //                                                        "in vec3 color;\n"
    //                                                        "out vec3 fragColor;\n"
    //                                                        "void main() {\n"
    //                                                        "    fragColor = color;\n"
    //                                                        "    gl_Position = vec4(position, 0.0, 1.0);\n"
    //                                                        "}\n");
    // program.addShaderFromSourceCode(QOpenGLShader::Fragment, "#version 330\r\n"
    //                                                          "in vec3 fragColor;\n"
    //                                                          "out vec4 color;\n"
    //                                                          "void main() {\n"
    //                                                          "    color = vec4(fragColor, 1.0);\n"
    //                                                          "}\n");

    re = program.link();
    __sAssert(re, "Failed to link program");
    re = program.bind();
    __sAssert(re, "Failed to bind program");

    QOpenGLVertexArrayObject vao;
    re = vao.create();
    __sAssert(re, "Failed to create vertex array");
    vao.bind();

    vertexPositionBuffer.bind();
    program.enableAttributeArray("pos");
    program.setAttributeBuffer("pos", GL_FLOAT, 0, 2);

    vertexColorBuffer.bind();
    program.enableAttributeArray("color");
    program.setAttributeBuffer("color", GL_FLOAT, 0, 3);


    program.enableAttributeArray("sunDir");
    program.setAttributeBuffer("sunDir", GL_FLOAT, 0, 3);

    const suncalc::SunCoords coords = suncalc::sunCoords(time, lat, lng, 1);
    const QVector3D sunVector = suncalc::sunVector(coords);
    qDebug() << lat << lng;
    program.setUniformValue("sunDir", sunVector);
    program.setUniformValue("skyRangeX", QVector2D{0.4, 0.6}); // was: mConfig.skyRangeX()
    program.setUniformValue("skyRangeY", QVector2D{0.25, 0.5});

    //    ==============DRAWING TO THE FBO============

    openGLContext.functions()->glClearColor(0.3f, 0.0f, 0.7f, 1.0f);
    openGLContext.functions()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    openGLContext.functions()->glDrawArrays(GL_QUADS, 0, 4);

    //   =============CLEANUP================== maybe not necessary
    program.disableAttributeArray("position");
    program.disableAttributeArray("color");
    program.release();
    fbo.release();

    //    ========SAVE IMAGE===========

    return fbo.toImage();
}