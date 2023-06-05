#include "vertex.h"
#include "edge.h"
#include "graph.h"

#include <QVector>

Vertex::Vertex(Graph *graph, int index) : graph(graph) {
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
    vertexIndex = index;
    degree = 0;
    color = "white";
}

Vertex::~Vertex(){
    clearEdge();
    degree = 0;
}

QString Vertex::getColor(){
    return this->color;
}

void Vertex::setColor(QString color){
    this->color = color;
    qApp->processEvents();
    update();
}

void Vertex::addEdge(Edge *edge){
    edgeList << edge;
    edge->adjust();
    degree++;
}

QVector<Edge *> Vertex::getEdges() const{
    return edgeList;
}

void Vertex::calculateForces(){
    if(!scene() || scene()->mouseGrabberItem() == this) {
        newPos = pos();
        return;
    }

    qreal xvel = 0;
    qreal yvel = 0;
    const QList<QGraphicsItem *> items = scene()->items();
    for(QGraphicsItem *item : items) {
        Vertex *temp = qgraphicsitem_cast<Vertex *>(item);
        if(!temp){
            continue;
        }
        QPointF vec = mapToItem(temp, 0, 0);
        qreal dx = vec.x();
        qreal dy = vec.y();
        double l = 2.0 * (dx * dx + dy * dy);
        if(l > 0 && l < 600){
            xvel += (dx * 150.0) / l;
            yvel += (dy * 150.0) / l;
        }
    }
    if(qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1){
        xvel = yvel = 0;
    }

    QRectF sceneRect = scene()->sceneRect();
    newPos = pos() + QPointF(xvel, yvel);
    newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 10), sceneRect.right() - 10));
    newPos.setY(qMin(qMax(newPos.y(), sceneRect.top() + 145), sceneRect.bottom() - 10));
}


bool Vertex::advancePosition() {
    if(newPos == pos()){
        return false;
    }
    setPos(newPos);
    return true;
}

int Vertex::getDegree(){
    return degree;
}

int Vertex::getIndex() {
    return vertexIndex;
}

void Vertex::setIndex(int index) {
    vertexIndex = index;
    qApp->processEvents();
    update();
}

bool Vertex::pathExist(int dest) {//проверка, является ли вершина конечной точкой
    for (Edge *temp : edgeList) {
        if (temp->destVertex()->getIndex() == dest) {
            return true;
        }
    }
    return false;
}

void Vertex::setPosition(double xvel, double yvel) {
    QRectF sceneRect = scene()->sceneRect();
    newPos = pos() + QPointF(xvel, yvel);
    newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 10), sceneRect.right() - 10));
    newPos.setY(qMin(qMax(newPos.y(), sceneRect.top() + 145), sceneRect.bottom() - 10));
}

void Vertex::insertEdge(int dest, double weight) {//добавление дуги
    const QList<QGraphicsItem *> items = scene()->items();
    for(QGraphicsItem *item : items){
        Vertex *temp = qgraphicsitem_cast<Vertex *>(item);
        if(!temp){
            continue;
        }
        if(temp->getIndex() == dest) {
            Edge *edge = new Edge(this, temp, weight);
            edgeList.append(edge);
            return;
        }
    }
}

void Vertex::updateWeight(int dest, double weight) {//изменение веса
    for(Edge *temp : edgeList){
        if(temp->destVertex()->getIndex() == dest) {
            temp->setWeight(weight);
        }
    }
}

void Vertex::eraseEdge(int dest) {//удаление конечной точки для дуг
    if (!pathExist(dest)) {//вершина не является конечной точкой - выход
        return;
    }

    for (int i = 0; i < edgeList.size(); i++) {//исправил
        if (edgeList[i]->destVertex()->getIndex() == dest) {
            edgeList.erase(edgeList.begin() + i);
        }
    }
    degree--;
}


void Vertex::clearEdge() {
    edgeList.clear();
    degree = 0;
}

void Vertex::changeEdgeColor(int dest) {//изменение цвета дуги
    for (Edge *edge : edgeList){
        if (edge->destVertex()->getIndex() == dest) {
            edge->setColor("black");
        }
    }
}

QPointF Vertex::getPos() {
    return newPos;
}

QRectF Vertex::boundingRect() const {
    qreal adjust = 2;
    return QRectF(-10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust);
}

QPainterPath Vertex::shape() const {
    QPainterPath path;
    path.addEllipse(-10, -10, 20, 20);
    return path;
}

void Vertex::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) {

    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);
    painter->drawEllipse(-7, -7, 20, 20);

    QRadialGradient gradient(-3, -3, 10);
    prepareGeometryChange();
    painter->setBrush(QBrush(Qt::white, Qt::SolidPattern));
    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-10, -10, 20, 20);//рисование круга
    prepareGeometryChange();
    QString index = QString::number(vertexIndex);
    painter->drawText(QRect(-10, -10, 20, 20), Qt::AlignCenter, index);//рисование номера вершины
}


QVariant Vertex::itemChange(GraphicsItemChange change, const QVariant &value) {
    switch(change){
        case ItemPositionHasChanged:
            for(Edge *edge : qAsConst(edgeList)){
                graph->itemMoved();
                edge->adjust();
                for (Edge *reverseEdge : edge->destVertex()->getEdges()) {
                    reverseEdge->adjust();
                }
            }
            graph->itemMoved();
            break;
        default:
            break;
    };
    return QGraphicsItem::itemChange(change, value);
}


void Vertex::mousePressEvent(QGraphicsSceneMouseEvent *event){
    update();
    QGraphicsItem::mousePressEvent(event);
}

void Vertex::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
