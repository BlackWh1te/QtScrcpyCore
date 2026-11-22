#ifndef INPUTCONVERTGAME_H
#define INPUTCONVERTGAME_H

#include <QPointF>
#include <QQueue>
#include <QTimer>

#include "inputconvertnormal.h"
#include "keymap.h"

#define MULTI_TOUCH_MAX_NUM 10
class InputConvertGame : public InputConvertNormal
{
    Q_OBJECT
public:
    InputConvertGame(Controller *controller);
    virtual ~InputConvertGame();

    virtual void mouseEvent(const QMouseEvent *from, const QSize &frameSize, const QSize &showSize);
    virtual void wheelEvent(const QWheelEvent *from, const QSize &frameSize, const QSize &showSize);
    virtual void keyEvent(const QKeyEvent *from, const QSize &frameSize, const QSize &showSize);
    virtual bool isCurrentCustomKeymap();

    void loadKeyMap(const QString &json);

protected:
    void updateSize(const QSize &frameSize, const QSize &showSize);
    void sendTouchDownEvent(int id, QPointF pos);
    void sendTouchMoveEvent(int id, QPointF pos);
    void sendTouchUpEvent(int id, QPointF pos);
    void sendTouchEvent(int id, QPointF pos, AndroidMotioneventAction action);
    void sendKeyEvent(AndroidKeyeventAction action, AndroidKeycode keyCode);
    QPointF calcFrameAbsolutePos(QPointF relativePos);
    QPointF calcScreenAbsolutePos(QPointF relativePos);

    // multi touch id
    int attachTouchID(int key);
    void detachTouchID(int key);
    int getTouchID(int key);

    // steer wheel
    void processSteerWheel(const KeyMap::KeyMapNode &node, const QKeyEvent *from);
    QPointF calcSteerWheelOffset(const KeyMap::KeyMapNode &node);
    void retargetSteerWheel();

    // click
    void processKeyClick(const QPointF &clickPos, bool clickTwice, bool switchMap, const QKeyEvent *from);

    // click mutil
    void processKeyClickMulti(const KeyMap::DelayClickNode *nodes, const int count, const QKeyEvent *from);

    // drag
    void processKeyDrag(const QPointF &startPos, QPointF endPos, quint32 startDelay, float dragSpeed, const QKeyEvent *from);

    // android key
    void processAndroidKey(AndroidKeycode androidKey, const QKeyEvent *from);

    // mouse
    bool processMouseClick(const QMouseEvent *from);
    bool processMouseMove(const QMouseEvent *from);
    void moveCursorTo(const QMouseEvent *from, const QPoint &localPosPixel);
    void mouseMoveStartTouch(const QMouseEvent *from);
    void mouseMoveStopTouch();
    void startMouseMoveTimer();
    void stopMouseMoveTimer();
    QPointF mouseMoveAnchor();

    bool switchGameMap();
    // Oxide fork: show/hide the cursor for menus while the keymap stays active
    void setMenuMode(bool on);
    bool checkCursorPos(const QMouseEvent *from);
    void hideMouseCursor(bool hide);

    void getDelayQueue(const QPointF& start, const QPointF& end,
                       const double& distanceStep, const double& posStepconst,
                       quint32 lowestTimer, quint32 highestTimer,
                       QQueue<QPointF>& queuePos, QQueue<quint32>& queueTimer);

protected:
    void timerEvent(QTimerEvent *event);

private slots:
    void onSteerWheelTimer();
    void onDragTimer();
    void onMouseMoveFlush();

private:
    QSize m_frameSize;
    QSize m_showSize;
    bool m_gameMap = false;
    bool m_needBackMouseMove = false;
    int m_multiTouchID[MULTI_TOUCH_MAX_NUM] = { 0 };
    KeyMap m_keyMap;

    bool m_processMouseMove = true;

    // steer wheel
    struct
    {
        // the first key pressed
        int touchKey = Qt::Key_unknown;
        // Oxide fork: sprint pushes the stick further up while held
        bool sprint = false;
        bool hasNode = false;
        KeyMap::KeyMapNode node;
        bool pressedUp = false;
        bool pressedDown = false;
        bool pressedLeft = false;
        bool pressedRight = false;

        // for delay
        struct {
            QPointF currentPos;
            QTimer* timer = nullptr;
            QQueue<QPointF> queuePos;
            QQueue<quint32> queueTimer;
            int pressedNum = 0;
        } delayData;
    } m_ctrlSteerWheel;

    // mouse move
    struct
    {
        QPointF lastConverPos;
        QPointF lastPos = { 0.0, 0.0 };
        bool touching = false;
        int timer = 0;
        bool smallEyes = false;
        int ignoreCount = 0;
        // Oxide fork: mouse deltas are accumulated and flushed to the device at a fixed rate
        QPointF pendingDelta = { 0.0, 0.0 };
        QTimer *flushTimer = nullptr;
        int idleMs = 0;
    } m_ctrlMouseMove;
    // enter custom (game) mode automatically on the first mouse event after a keymap with mouseMoveMap is loaded
    bool m_autoGameMapPending = false;
    bool m_cursorHidden = false;
    // Oxide fork: optional sprint key ("sprintKey") and how much further up the stick goes ("sprintUpScale")
    int m_sprintKey = 0;
    double m_sprintUpScale = 2.0;

    // for drag delay
    struct {
        QPointF currentPos;
        QTimer* timer = nullptr;
        QQueue<QPointF> queuePos;
        QQueue<quint32> queueTimer;
        int pressKey = 0;
    } m_dragDelayData;
};

#endif // INPUTCONVERTGAME_H
