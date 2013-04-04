#include <QtWidgets>
#include <QtOpenGL>
#include <QTimer>

#include "flowwidget.h"


FlowWidget::FlowWidget(QWidget *parent, QGLWidget *shareWidget)
    : QGLWidget(parent, shareWidget)
{
    setFocusPolicy(Qt::StrongFocus);

    _updateTimer = new QTimer(this);
    connect(_updateTimer, SIGNAL(timeout()), this, SLOT(delayedUpdate()));

    clearColor = Qt::black;
    xRot = 0;
    yRot = 0;
    zRot = 0;
    program = 0;

    currentStyle = Flow;
    _model = 0;
    _modelColumn = 0;
    _updateOnShow = false;

    /*** Animation Settings ***/
    Animation_step = 0.08f;
    Animation_speedup = 1.5f;
    Animation_step_max = 0.1f;
    Animation_Fade_out_dis = 2.0f;
    Current_Selected = 0;
    pre_rotation = 1.5f;
    View_rotate_light_strenght = 3.0f;
    View_rotate_sub = 0.01f;
    View_rotate_add = 0.02f;
    View_angle = 20.0f;
	X_Initial = 5.0f;

    /*** Position Configuration ***/
    CF_X = 0;
    CF_Y = 0;
    CF_Z = -3;

    CF_RX = 15;
    CF_RY = 0;
    CF_RZ = 0;

    Rotation  = -50.0f;
    X_Distance = 0.5f;
    Center_Distance = 1.0f;
    Z_Distance = 0.0f;

    Center_pos.x = 0.0f;
    Center_pos.y = 0.0f;
    Center_pos.z = 1.0f;
    Center_pos.rot = 0.0f;

    /*** Style ***/
    Shading_top = 1;
    Shading_bottom = 0.02f;
    reflection_up = 0;
    reflection_bottom = 0.45f;

    /*** System variables ***/

    View_rotate = 0;
    View_rotate_active = 0;
    Step_Backup = Animation_step/Animation_speedup;

    texCoords.resize(8);
    texCoords[0] = QVector2D(0, 0);
    texCoords[1] = QVector2D(1, 0);
    texCoords[2] = QVector2D(1, 1);
    texCoords[3] = QVector2D(0, 1);

    texCoords[4] = QVector2D(0, 1);
    texCoords[5] = QVector2D(1, 1);
    texCoords[6] = QVector2D(1, 0);
    texCoords[7] = QVector2D(0, 0);
}

FlowWidget::~FlowWidget()
{
}

QSize FlowWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize FlowWidget::sizeHint() const
{
    return QSize(200, 200);
}

void FlowWidget::setClearColor(const QColor &color)
{
    clearColor = color;
    updateGL();
}

void FlowWidget::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glDisable(GL_DITHER);

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1
#define PROGRAM_COLOR_ATTRIBUTE 2

    QGLShader *vshader = new QGLShader(QGLShader::Vertex, this);
    const char *vsrc =
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec4 texCoord;\n"
        "attribute lowp vec4 colorAttr;\n"
        "varying mediump vec4 texc;\n"
        "varying lowp vec4 col;\n"
        "uniform mediump mat4 matrix;\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = matrix * vertex;\n"
        "    col = colorAttr;\n"
        "    texc = texCoord;\n"
        "}\n";
    vshader->compileSourceCode(vsrc);

    QGLShader *fshader = new QGLShader(QGLShader::Fragment, this);
    const char *fsrc =
        "uniform sampler2D texture;\n"
        "uniform int useTexture;\n"
        "varying mediump vec4 texc;\n"
        "varying lowp vec4 col;\n"
        "void main(void)\n"
        "{\n"
        "    if (useTexture == 1)\n"
        "        gl_FragColor = texture2D(texture, texc.st) * col;\n"
        "    else\n"
        "        gl_FragColor = col;\n"
        "}\n";
    fshader->compileSourceCode(fsrc);

    program = new QGLShaderProgram(this);
    program->addShader(vshader);
    program->addShader(fshader);
    program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    program->bindAttributeLocation("colorAttr", PROGRAM_COLOR_ATTRIBUTE);
    program->link();

    program->bind();
    program->setUniformValue("texture", 0);

    timer.start(12, this);
}

void FlowWidget::paintGL()
{
    drawAll(RenderMode);
}

void FlowWidget::drawAll(FlowWidget::Mode mode)
{
    if (mode == SelectMode)
        qglClearColor(Qt::white);
    else
        qglClearColor(clearColor);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < Current_Selected; i++)
        drawItem(_items[i], mode, indexToColor(i));

    for (int i = _items.count() - 1; i > Current_Selected; i--)
        drawItem(_items[i], mode, indexToColor(i));

	if (_items.count() > 0)
		drawItem(_items[Current_Selected], mode, indexToColor(Current_Selected));
}

void FlowWidget::drawItem(const FlowItem &item, FlowWidget::Mode mode, const QColor &color)
{
    QVector<QVector3D> vertices(8);
    QVector<QVector4D> colors(8);

    float w = item.width;
    float h = item.height;

    vertices[0] = QVector3D(w/2*-1, -0.5, 0);
    vertices[1] = QVector3D(w/2, -0.5, 0);
    vertices[2] = QVector3D(w/2, -0.5+h, 0);
    vertices[3] = QVector3D(w/2*-1, -0.5+h, 0);

    vertices[4] = QVector3D(w/2*-1, -0.5-h, 0);
    vertices[5] = QVector3D(w/2, -0.5-h, 0);
    vertices[6] = QVector3D(w/2, -0.5, 0);
    vertices[7] = QVector3D(w/2*-1, -0.5, 0);

    float opacity = 1-1/(Animation_Fade_out_dis+View_rotate_light_strenght*abs(View_rotate))*abs(0-0);
    //calculate shading
    float LShading = ((Rotation != 0 )?((item.current.rot < 0)?1-1/Rotation*item.current.rot:1):1);
    float RShading = ((Rotation != 0 )?((item.current.rot > 0)?1-1/(Rotation*-1)*item.current.rot:1):1);
    float LUP = Shading_top+(1-Shading_top)*LShading;
    float LDOWN = Shading_bottom+(1-Shading_bottom)*LShading;
    float RUP =  Shading_top+(1-Shading_top)*RShading;
    float RDOWN =  Shading_bottom+(1-Shading_bottom)*RShading;;

    if (mode == SelectMode)
    {
        colors[0] = QVector4D(color.redF(), color.greenF(), color.blueF(), 1);
        colors[1] = QVector4D(color.redF(), color.greenF(), color.blueF(), 1);
        colors[2] = QVector4D(color.redF(), color.greenF(), color.blueF(), 1);
        colors[3] = QVector4D(color.redF(), color.greenF(), color.blueF(), 1);

        colors[4] = QVector4D(1.0f, 1.0f, 1.0f, 1.0f);
        colors[5] = QVector4D(1.0f, 1.0f, 1.0f, 1.0f);
        colors[6] = QVector4D(1.0f, 1.0f, 1.0f, 1.0f);
        colors[7] = QVector4D(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else
    {
        colors[0] = QVector4D(LDOWN*opacity,LDOWN*opacity,LDOWN*opacity,1);
        colors[1] = QVector4D(RDOWN*opacity,RDOWN*opacity,RDOWN*opacity,1);
        colors[2] = QVector4D(RUP*opacity,RUP*opacity,RUP*opacity,1);
        colors[3] = QVector4D(LUP*opacity,LUP*opacity,LUP*opacity,1);

        colors[4] = QVector4D(LUP*opacity*reflection_up,LUP*opacity*reflection_up,LUP*opacity*reflection_up,opacity*reflection_up);
        colors[5] = QVector4D(LUP*opacity*reflection_up,LUP*opacity*reflection_up,LUP*opacity*reflection_up,opacity*reflection_up);
        colors[6] = QVector4D(opacity*reflection_bottom,opacity*reflection_bottom,opacity*reflection_bottom,opacity*reflection_bottom);
        colors[7] = QVector4D(opacity*reflection_bottom,opacity*reflection_bottom,opacity*reflection_bottom,opacity*reflection_bottom);
    }
    
	float aspectRatio = 1.0f;
	if (height() > 0)
		aspectRatio = width() / (float)height();

    QMatrix4x4 m;
    m.perspective(45.0f, aspectRatio, 1.0f, 200.0f);
    m.translate(CF_X, CF_Y, CF_Z);
    m.rotate(CF_RX, 1.0f, 0.0f, 0.0f);
    m.rotate(View_rotate*View_angle+CF_RY, 0.0f, 1.0f, 0.0f);
    m.rotate(CF_RZ, 0.0f, 0.0f, 1.0f);
    m.translate(item.current.x, item.current.y, item.current.z);
    m.rotate(item.current.rot, 0.0f, 1.0f, 0.0f);

    if (mode == SelectMode)
        program->setUniformValue("useTexture", 0);
    else
        program->setUniformValue("useTexture", 1);

    program->setUniformValue("matrix", m);
    program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    program->enableAttributeArray(PROGRAM_COLOR_ATTRIBUTE);
    program->setAttributeArray(PROGRAM_VERTEX_ATTRIBUTE, vertices.constData());
    program->setAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE, texCoords.constData());
    program->setAttributeArray(PROGRAM_COLOR_ATTRIBUTE, colors.constData());

    glBindTexture(GL_TEXTURE_2D, item.img);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
}

void FlowWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
}

void FlowWidget::mousePressEvent(QMouseEvent *event)
{
    QGLWidget::mousePressEvent(event);
    moveTo(hitTest(event->pos()));
}

int FlowWidget::hitTest(const QPoint &point)
{
    GLuint colorRenderBuffer;
    GLuint frameBuffer;

    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glGenRenderbuffers(1, &colorRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);

#ifdef Q_OS_WIN
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8_OES, width(), height());
#else
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width(), height());
#endif
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBuffer);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        qDebug() << "Error: Framebuffer status = " << status;
        return -1;
    }

    drawAll(SelectMode);

    GLubyte pixel[4];
    glReadPixels(point.x(), height()-point.y(), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

    glDeleteRenderbuffers(1, &colorRenderBuffer);
    glDeleteFramebuffers(1, &frameBuffer);

    return colorToIndex(QColor(pixel[0], pixel[1], pixel[2]));
}

int FlowWidget::colorToIndex(const QColor &color) const
{
    return (color.rgb() & 0xffffff);
}

QColor FlowWidget::indexToColor(int index) const
{
    return (0xffu << 24) | (index & 0xffffff);
}

bool FlowWidget::unproject(const QPoint &point, QVector3D &vector)
{
    float aspectRatio = 1.0f;
    if (height() > 0)
        aspectRatio = width() / (float)height();

    QMatrix4x4 m;
    m.perspective(45.0f, aspectRatio, 1.0f, 200.0f);
    m.translate(CF_X, CF_Y, CF_Z);
    m.rotate(CF_RX, 1.0f, 0.0f, 0.0f);
    m.rotate(View_rotate*View_angle+CF_RY, 0.0f, 1.0f, 0.0f);
    m.rotate(CF_RZ, 0.0f, 0.0f, 1.0f);

    bool invertible;
    QMatrix4x4 minv = m.inverted(&invertible);
    if (!invertible)
        return false;

    QVector4D in(point.x(), height() - point.y(), 0.0f, 1.0f);
    in.setX(in.x() / (qreal) width());
    in.setY(in.y() / (qreal) height());

    in.setX(in.x() * 2 - 1);
    in.setY(in.y() * 2 - 1);

    QVector4D out = minv * in;
    if (abs(out.w()) < 1.e-6f)
        return false;

    out.setX(out.x() / out.w());
    out.setY(out.y() / out.w());
    out.setZ(out.z() / out.w());

    vector = out.toVector3D();
    return true;
}

void FlowWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Left:
        moveLeft();
        break;
    case Qt::Key_Right:
        moveRight();
        break;
    case Qt::Key_Return:
        if (_model)
        {
            if (_items.count() > 0)
            {
                QModelIndex index = _model->index(Current_Selected, 0);
                emit activated(index);
            }
        }
        break;
    default:
        QGLWidget::keyPressEvent(event);
    }
}

void FlowWidget::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Left:
        cleanupAnimation();
        break;
    case Qt::Key_Right:
        cleanupAnimation();
        break;
    default:
        QGLWidget::keyReleaseEvent(event);
    }
}

void FlowWidget::showEvent(QShowEvent *event)
{
    QGLWidget::showEvent(event);

    if (_updateOnShow)
    {
        clearItems();
        initItems();
        _updateOnShow = false;
    }
}

void FlowWidget::calculateVector(int index, FlowItemVector &vector)
{
    if (index == 0)
    {
        vector = Center_pos;
    }
    else if (index > 0)
    {
        vector.x = Center_Distance + X_Distance*index;
        vector.y = 0.0f;
        vector.z = -Z_Distance*index;
        vector.rot = Rotation;
    }
    else
    {
        vector.x = -Center_Distance + X_Distance*index;
        vector.y = 0.0f;
        vector.z = Z_Distance*index;
        vector.rot = -Rotation;
    }
}

void FlowWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    update();
    updateGL();
}

void FlowWidget::wheelEvent(QWheelEvent *event)
{
	if (_items.count() == 0)
	{
		event->ignore();
		return;
	}

    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;

    if (numSteps > 0)
    {
        int index = qMin(_items.count() - 1, Current_Selected + numSteps);
        moveTo(index);
    }
    else if (numSteps < 0)
    {
        int index = qMax(0, Current_Selected + numSteps);
        moveTo(index);
    }

    event->accept();
}

void FlowWidget::update()
{
    for (int i = _items.count() - 1; i >= 0; i--)
    {
        calculateVector(i - Current_Selected, _items[i].animEnd);
        animate(_items[i].current, _items[i].animEnd);
    }

    if (!View_rotate_active)
        View_rotate += (0-View_rotate)*View_rotate_sub;
}

void FlowWidget::animate(FlowItemVector &current, FlowItemVector to)
{
    current.x += (to.x - current.x) * Animation_step;
    current.y += (to.y - current.y) * Animation_step;
    current.z += (to.z - current.z) * Animation_step;

    if (abs(to.rot - current.rot) > 0.01f)
    {
        current.rot += (to.rot - current.rot) * Animation_step*pre_rotation;
    }
}

void FlowWidget::moveLeft()
{
    if (Current_Selected > 0)
    {
        Current_Selected--;

        Animation_step *= Animation_speedup;

        if (Animation_step > Animation_step_max)
            Animation_step = Animation_step_max;

        if (View_rotate_active && View_rotate > -1)
            View_rotate -= View_rotate_add;

        View_rotate_active = 1;

    }

    emit selectionChanged(QItemSelection(), QItemSelection());
}

void FlowWidget::moveRight()
{
    if (Current_Selected < _items.count() - 1)
    {
        Current_Selected++;

        Animation_step *= Animation_speedup;

        if (Animation_step > Animation_step_max)
            Animation_step = Animation_step_max;

        if (View_rotate_active && View_rotate < 1)
            View_rotate += View_rotate_add;

        View_rotate_active = 1;
    }

    emit selectionChanged(QItemSelection(), QItemSelection());
}

void FlowWidget::moveTo(int index)
{
    if (index < 0 || index >= _items.count())
        return;

    if (index == Current_Selected)
        return;

    Animation_step *= Animation_speedup;

    if (Animation_step > Animation_step_max)
        Animation_step = Animation_step_max;

    //if (index > Current_Selected)
    //{
    //    if (View_rotate_active && View_rotate < 1)
    //        View_rotate += View_rotate_add;
    //}
    //else if (index < Current_Selected)
    //{
    //    if (View_rotate_active && View_rotate > -1)
    //        View_rotate -= View_rotate_add;
    //}

    Current_Selected = index;
    //View_rotate_active = 1;

    emit selectionChanged(QItemSelection(), QItemSelection());
}

void FlowWidget::cleanupAnimation()
{
    Animation_step = Step_Backup;
    View_rotate_active = 0;
}

FlowWidget::Style FlowWidget::style() const
{
    return currentStyle;
}

void FlowWidget::setStyle(FlowWidget::Style style)
{
    currentStyle = style;

    switch (style)
    {
    case Pyramid:
        Z_Distance = 0.2f;
        X_Distance = 0.3f;
        Rotation = -50.0f;
        View_angle = 20.0f;
        Center_Distance = 1.0f;
        break;
    case Flat:
        Z_Distance = 0.0f;
        X_Distance = 1.1f;
        Rotation = 0.0f;
        View_angle = 20.0f;
        Center_Distance = 0.6f;
        break;
    case FlatAngle:
        Z_Distance = 0.0f;
        X_Distance = 1.1f;
        Rotation = 0.0f;
        View_angle = 70.0f;
        Center_Distance = 0.6f;
        break;
    case Flow:
    default:
        Z_Distance = 0.0f;
        X_Distance = 0.5f;
        Rotation = -50.0f;
        View_angle = 20.0f;
        Center_Distance = 1.0f;
        break;
    }

    update();
}

QAbstractItemModel * FlowWidget::model() const
{
    return _model;
}

void FlowWidget::setModel(QAbstractItemModel *model)
{
    if (_model)
    {
        disconnect(_model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(dataChanged(const QModelIndex&, const QModelIndex&)));
        disconnect(_model, SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(rowsInserted(const QModelIndex&, int, int)));
        disconnect(_model, SIGNAL(rowsRemoved(const QModelIndex&, int, int)), this, SLOT(rowsRemoved(const QModelIndex&, int, int)));
        disconnect(_model, SIGNAL(modelReset()), this, SLOT(modelReset()));
        disconnect(_model, SIGNAL(layoutChanged()), this, SLOT(layoutChanged()));
    }

    _model = model;

    clearItems();

    if (_model)
    {
        connect(_model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(dataChanged(const QModelIndex&, const QModelIndex&)));
        connect(_model, SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(rowsInserted(const QModelIndex&, int, int)));
        connect(_model, SIGNAL(rowsRemoved(const QModelIndex&, int, int)), this, SLOT(rowsRemoved(const QModelIndex&, int, int)));
        connect(_model, SIGNAL(modelReset()), this, SLOT(modelReset()));
        connect(_model, SIGNAL(layoutChanged()), this, SLOT(layoutChanged()));

        initItems();
    }
}

int FlowWidget::modelColumn() const
{
    return _modelColumn;
}

void FlowWidget::setModelColumn(int column)
{
    _modelColumn = column;
    updateItems();
}

void FlowWidget::initItems()
{
    if (!_model)
        return;

    int rows = _model->rowCount();
    for (int i = 0; i < rows; i++)
    {
        QPixmap pixmap = _model->data(_model->index(i, _modelColumn), Qt::DecorationRole).value<QPixmap>();
        //QString name = _model->data(_model->index(i, _modelColumn), Qt::DisplayRole).toString();

        FlowItem newItem;

        if (!pixmap.isNull() && pixmap.height() != 0)
        {
            float aspectRatio = pixmap.width() / (float) pixmap.height();

            newItem.img = bindTexture(pixmap, GL_TEXTURE_2D);
            newItem.width = aspectRatio;
            newItem.height = 1.0f;
        }
        else
        {
            newItem.img = 0;
            newItem.width = 1.3333f;
            newItem.height = 1.0f;
        }

        calculateVector(_items.count(), newItem.current);
        newItem.current.x += X_Initial;
        newItem.current.rot = 0.0f;

        _items.append(newItem);
    }

    if (rows > 0)
        emit selectionChanged(QItemSelection(), QItemSelection());
}

void FlowWidget::clearItems()
{
    for (int i = 0; i < _items.count(); i++)
        deleteTexture(_items[i].img);

    _items.clear();
    Current_Selected = 0;
}

void FlowWidget::updateItems()
{
    if (isVisible())
    {
        if (_updateTimer->isActive())
            _updateTimer->stop();

        _updateTimer->start(1000);
    }
    else
    {
        _updateOnShow = true;
    }
}

void FlowWidget::delayedUpdate()
{
    _updateTimer->stop();
    clearItems();
    initItems();
}

void FlowWidget::dataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight)
{
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);
    updateItems();
}

void FlowWidget::rowsInserted(const QModelIndex & parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);

    updateItems();
}

void FlowWidget::rowsRemoved(const QModelIndex & parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);

    updateItems();
}

void FlowWidget::modelReset()
{
    updateItems();
}

void FlowWidget::layoutChanged()
{
    updateItems();
}

QModelIndex FlowWidget::currentIndex() const
{
    if (_model && _items.count() > 0)
        return _model->index(Current_Selected, 0);

    return QModelIndex();
}

void FlowWidget::setCurrentIndex(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    moveTo(index.row());
}
