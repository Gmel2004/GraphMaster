#ifndef GRAPH_H
#define GRAPH_H

#include <vertex.h>
#include <edge.h>

#include <ctime>
#include <climits>
#include <iomanip>

#include <QMainWindow>
#include <QGraphicsItem>
#include <QVector>
#include <QStack>
#include <QQueue>
#include <QPointF>
#include <QGraphicsView>
#include <QWidget>
#include <QStyleOptionGraphicsItem>
#include <QKeyEvent>
#include <QPushButton>
#include <QHBoxLayout>
#include <QStyle>
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QWidget>
#include <QTableWidget>
#include <QApplication>
#include <QGroupBox>
#include <QToolButton>
#include <QMenu>
#include <QDir>
#include <QSaveFile>
#include <QFileDialog>


#include <windows.h>
#include <conio.h>
#include <process.h>
#include "Psapi.h"
#include <TlHelp32.h>
#include <assert.h>
#include <stdio.h>
#include <sstream>
#include <functional>
using namespace std;

class vertex;
class Edge;
class Graph;

class Graph : public QGraphicsView {
    Q_OBJECT

public:
    Graph(QWidget *parent = nullptr);
    void itemMoved();
    int getVertexNum();
    void runDFS(int temp, bool* visited);
    void runBFS(int index, bool* visited, QQueue<int>* bfsQueue);
    void runDijkstra(int vertexIndex);
    void createDFSWindow();
    int ** GetAdjMatrix();
    void RemoveAdjMatrix(int ** AdjMatrix);
    void runTSP();

protected:
    void timerEvent(QTimerEvent *event) override;

    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void createTabWidget(const QRectF &rect);
    void createMenuBar();
    void scaleView(qreal scaleFactor);

    void updateIndex();
    void insertVertex();
    void setVertexPos(int vertexIndex, double x, double y);
    void eraseVertex(int vertexIndex);
    void insertEdge(int source, int dest, double weight);
    void updateWeight(int source, int dest, double weight);
    void eraseEdge(int source, int dest);
    bool checkAdjacent(int source, int dest);
    int getDegree(int vertexIndex);
    void clear();

    void createEraseVertexWindow();
    void createAddEdgeWindow();
    void createEraseEdgeWindow();
    void createUpdateWeightWindow();
    void createBFSWindow();
    void createDijkstraWindow();
    void createAdjMatrixWindow();
    void createFunctionWindow();
    void createTSPWindow();

private slots:
    void eraseVertexSlot();
    void addEdgeSlot();
    void eraseEdgeSlot();
    void updateWeightSlot();
    void StartSelectedFunction();

private:
    unsigned int edgeNum=0;
    int vertexNum;
    bool connected;

    QVector<Vertex *> vertexList;
    QGraphicsScene *scene;
    int timerId = 0;

    QWidget *window;
    QLineEdit *input1;
    QLineEdit *input2;
    QLineEdit *input3;
    QLineEdit *input5;

    QString curFile;
};

#endif // GRAPH_H
