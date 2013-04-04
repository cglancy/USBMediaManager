#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtWidgets>
#include <QGLWidget>
#include <QList>
#include <QBasicTimer>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QModelIndex>

class QAbstractItemModel;
class QItemSelection;
class QTimer;

QT_FORWARD_DECLARE_CLASS(QGLShaderProgram);

struct FlowItemVector
{
    float x;
    float y;
    float z;
    float rot;
};

struct FlowItem
{
    GLuint img;
    float width;
    float height;
    FlowItemVector current;
    FlowItemVector animEnd;
};

class FlowWidget : public QGLWidget
{
    Q_OBJECT
public:
    enum Style
    {
        Flow,
        Pyramid,
        Flat,
        FlatAngle
    };

public:
    explicit FlowWidget(QWidget *parent = 0, QGLWidget *shareWidget = 0);
    ~FlowWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void setClearColor(const QColor &color);

    QAbstractItemModel * model() const;
    void setModel(QAbstractItemModel *model);

    int modelColumn() const;
    void setModelColumn(int column);

    Style style() const;
    void setStyle(Style style);

    QModelIndex currentIndex() const;

signals:
    void selectionChanged(const QItemSelection& current, const QItemSelection& previous);
    void activated(const QModelIndex &index);

public slots:
    void setCurrentIndex(const QModelIndex &index);
    void moveLeft();
    void moveRight();

private slots:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void rowsInserted(const QModelIndex &parent, int start, int end);
    void rowsRemoved(const QModelIndex &parent, int start, int end);
    void layoutChanged();
    void modelReset();
    void delayedUpdate();

protected:
    enum Mode
    {
        RenderMode,
        SelectMode
    };

    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void timerEvent(QTimerEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void showEvent(QShowEvent *event);
    void drawAll(Mode mode);
    void drawItem(const FlowItem &item, Mode mode, const QColor &color);
    void initItems();
    void updateItems();
    void clearItems();
    bool unproject(const QPoint &point, QVector3D &vector);

private:
    void calculateVector(int index, FlowItemVector &vector);
    void animate(FlowItemVector &current, FlowItemVector to);
    void cleanupAnimation();
    void update();
    int colorToIndex(const QColor &color) const;
    QColor indexToColor(int index) const;
    void moveTo(int index);
    int hitTest(const QPoint &point);

    QColor clearColor;
    QPoint lastPos;
    int xRot;
    int yRot;
    int zRot;
    GLuint textures[6];
    QBasicTimer timer;
    Style currentStyle;
    QVector<QVector2D> texCoords;
    QAbstractItemModel *_model;
    int _modelColumn;
    QGLShaderProgram *program;
    QTimer *_updateTimer;
    QList<FlowItem> _items;
    bool _updateOnShow;

    /*** Animation Settings ***/
    //sets the speed of the animation
    float Animation_step;
    //sets the acceleration of the animation
    float Animation_speedup;
    //sets the maximum speed of the animation
    float Animation_step_max;
    //sets the distance of view 
    float Animation_Fade_out_dis;
    //sets/returns the curent selected cover 
    int Current_Selected;
    //sets the rotation increasion
    float pre_rotation;
    //sets the light strenght on rotation 
    float View_rotate_light_strenght;
    //sets the speed of the rotation 
    float View_rotate_add;
    //sets the speed of reversing the rotation
    float View_rotate_sub;
    //sets the maximum view angle
    float View_angle;

    /*** Position Configuration ***/
    //the X Position of the Coverflow 
    float CF_X;
    //the Y Position of the Coverflow 
    float CF_Y;
    //the Z Position of the Coverflow 
    float CF_Z;
    //the X Rotation of the Coverflow 
    float CF_RX;
    //the Y Rotation of the Coverflow 
    float CF_RY;
    //the Z Rotation of the Coverflow 
    float CF_RZ;
    //sets the rotation of each cover
    float Rotation;
    //sets the distance between the covers
    float X_Distance;
    //sets the distance between the centered and the non centered covers
    float Center_Distance;
    //sets the pushback amount 
    float Z_Distance;
    //defines the position of the centered cover 
    FlowItemVector Center_pos;
	float X_Initial;

    /*** Style ***/
    //sets the amount of shading of the covers in the back (0-1)
    float Shading_top;
    float Shading_bottom;

    //sets the reflection strenght (0-1)
    float reflection_up;
    float reflection_bottom;

    /*** System info ***/
    float View_rotate;
    int View_rotate_active;
    float Step_Backup;
};

#endif
