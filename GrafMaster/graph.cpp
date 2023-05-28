#include "graph.h"
#include <QTextStream>
#include <QMessageBox>
#include "priority_queue.h"

bool flag = false;//флаг для алгоритма Дейкстры
QString str;

Graph::Graph(QWidget *parent) : QGraphicsView(parent) {
    scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-450, -450, 800, 900);

    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(800, 900);

    connected = true;
    DFS.clear();
    BFS.clear();
}

void Graph::itemMoved() {//хз
    if(!timerId) {
        timerId = startTimer(1000 / 25);
    }
}

void Graph::timerEvent(QTimerEvent *event) {//хз
    Q_UNUSED(event);
    const QList<QGraphicsItem *> items = scene -> items();
    for(Vertex *temp : qAsConst(vertexList)){
        temp->calculateForces();
    }
    bool itemsMoved = false;
    for(Vertex *temp : qAsConst(vertexList)){
        if(temp->advancePosition()){
            itemsMoved = true;
        }
    }

    if(!itemsMoved){
        killTimer(timerId);
        timerId = 0;
    }
}

void Graph::drawBackground(QPainter *painter, const QRectF &rect){//хз
    Q_UNUSED(rect);

    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect)){
        painter->fillRect(rightShadow, Qt::darkGray);
    }

    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect)){
        painter->fillRect(bottomShadow, Qt::darkGray);
    }

    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersected(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);
    QRectF textRect(sceneRect.left() + 4, sceneRect.top() + sceneRect.height()/9 + 35, sceneRect.width() - 4, 20);
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(14);
    painter->setFont(font);
    painter->setPen(Qt::lightGray);
    createTabWidget(rect);
}

void Graph::createTabWidget(const QRectF &rect) {
    /*QWidget *TextWidget = new QWidget;
    QLabel label("Graph Master", TextWidget);
    label.setGeometry(100, 100, 100, 100);
    scene->addWidget(TextWidget);*/

    Q_UNUSED(rect);

    QRectF sceneRect = this->sceneRect();

    QWidget *CommandsWidget = new QWidget;
    CommandsWidget->setGeometry(sceneRect.left() + 1, sceneRect.top() + 3, sceneRect.width()/6, sceneRect.height() - 2);
    QVBoxLayout *vertexTabLayout = new QVBoxLayout(CommandsWidget);

    QFont textfont = QFont();
    textfont.setPointSize(12);

    QPushButton *addVertexButton = new QPushButton();
    addVertexButton->setText(tr("⚪➕"));
    addVertexButton->setFont(textfont);
    addVertexButton->setMaximumSize(QSize(120, 120));
    addVertexButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                                   "QPushButton:pressed{background-color: lightBlue;} "
                                   "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");

    connect(addVertexButton, &QPushButton::clicked, this, &Graph::insertVertex);

    QPushButton *eraseVertexButton = new QPushButton();
    eraseVertexButton->setText(tr("⚪🗑"));

    eraseVertexButton->setFont(textfont);
    textfont.setPointSize(12);
    eraseVertexButton->setMaximumSize(QSize(120, 120));
    eraseVertexButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                                   "QPushButton:pressed{background-color: lightBlue;} "
                                   "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");

    connect(eraseVertexButton, &QPushButton::clicked, this, &Graph::createEraseVertexWindow);

    QPushButton *setVertexPosButton = new QPushButton();
    setVertexPosButton->setText(tr("Set Position"));
    setVertexPosButton->setFont(textfont);
    setVertexPosButton->setMaximumSize(QSize(120, 120));
    setVertexPosButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                                      "QPushButton:pressed{background-color: lightBlue;} "
                                      "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");

    connect(setVertexPosButton, &QPushButton::clicked, this, &Graph::createSetVertexPosWindow);

    QPushButton *vertexInfoButton = new QPushButton();
    vertexInfoButton->setText(tr("Vertex Info"));
    vertexInfoButton->setFont(textfont);
    vertexInfoButton->setMaximumSize(QSize(120, 120));
    vertexInfoButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                                   "QPushButton:pressed{background-color: lightBlue;} "
                                   "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");

    connect(vertexInfoButton, &QPushButton::clicked, this, &Graph::createVertexInfoWindow);

    vertexTabLayout->addWidget(addVertexButton);
    vertexTabLayout->addWidget(eraseVertexButton);
    //vertexTabLayout->addWidget(setVertexPosButton); erase functional
    //vertexTabLayout->addWidget(vertexInfoButton); erase functional

    QPushButton *addEdgeButton = new QPushButton();
    addEdgeButton->setText(tr("━▶➕"));
    addEdgeButton->setFont(textfont);
    addEdgeButton->setMaximumSize(QSize(120, 120));
    addEdgeButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                                 "QPushButton:pressed{background-color: lightBlue;} "
                                 "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(addEdgeButton, &QPushButton::clicked, this, &Graph::createAddEdgeWindow);

    QPushButton *updateWeightButton = new QPushButton();
    updateWeightButton->setText(tr("Update\nWeight"));
    updateWeightButton->setFont(textfont);
    updateWeightButton->setMaximumSize(QSize(120, 120));
    updateWeightButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                                  "QPushButton:pressed{background-color: lightBlue;} "
                                  "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(updateWeightButton, &QPushButton::clicked, this, &Graph::createUpdateWeightWindow);



    QPushButton *eraseEdgeButton = new QPushButton();
    eraseEdgeButton->setText(tr("━▶🗑"));
    eraseEdgeButton->setFont(textfont);
    eraseEdgeButton->setMaximumSize(QSize(120, 120));
    eraseEdgeButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                                 "QPushButton:pressed{background-color: lightBlue;} "
                                 "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(eraseEdgeButton, &QPushButton::clicked, this, &Graph::createEraseEdgeWindow);

    vertexTabLayout->addWidget(addEdgeButton);
    vertexTabLayout->addWidget(updateWeightButton);
    vertexTabLayout->addWidget(eraseEdgeButton);

    /*QToolButton *functionButton = new QToolButton();
    functionButton->setText(tr("Functions"));
    functionButton->setFont(textfont);
    functionButton->setMaximumSize(QSize(120, 35));
    functionButton->setStyleSheet("QToolButton:selected, QToolButton:hover{color: darkBlue;} "
                             "QToolButton:pressed{background-color: lightBlue;} "
                             "QToolButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    QMenu *menu = new QMenu;
    menu->setFixedWidth(200);

    QAction *dfs = new QAction(tr("DFS"), this);
    dfs->setStatusTip(tr("Depth First Search"));
    connect(dfs, &QAction::triggered, this, &Graph::createDFSWindow);
    menu->addAction(dfs);
    menu->addSeparator();

    QAction *bfs = new QAction(tr("BFS"), this);
    bfs->setStatusTip(tr("Breadth First Search"));
    connect(bfs, &QAction::triggered, this, &Graph::createBFSWindow);
    menu->addAction(bfs);
    menu->addSeparator();

    QAction *dijkstra = new QAction(tr("Dijkstra's algorithm"), this);
    dijkstra->setStatusTip(tr("Dijkstra 's algorithm"));
    connect(dijkstra, &QAction::triggered, this, &Graph::createDijkstraWindow);
    menu->addAction(dijkstra);
    menu->addSeparator();

    functionButton->setPopupMode(QToolButton::InstantPopup);
    functionButton->setMenu(menu);*/

    QPushButton *FuncButton = new QPushButton();
    FuncButton->setText(tr("🔎"));
    FuncButton->setFont(textfont);
    FuncButton->setMaximumSize(QSize(120, 120));
    FuncButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                             "QPushButton:pressed{background-color: lightBlue;} "
                             "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(FuncButton, &QPushButton::clicked, this, &Graph::createFunctionWindow);
    vertexTabLayout->addWidget(FuncButton);

    QPushButton *resetButton = new QPushButton();
    resetButton->setText(tr("Reset"));
    resetButton->setFont(textfont);
    resetButton->setMaximumSize(QSize(120, 120));
    resetButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                             "QPushButton:pressed{background-color: lightBlue;} "
                             "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(resetButton, &QPushButton::clicked, this, &Graph::reset);

    QPushButton *clearButton = new QPushButton();
    clearButton->setText(tr("Clear"));
    clearButton->setFont(textfont);
    clearButton->setMaximumSize(QSize(120, 120));
    clearButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                             "QPushButton:pressed{background-color: lightBlue;} "
                             "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(clearButton, &QPushButton::clicked, this, &Graph::clear);

    QPushButton *graphInfoButton = new QPushButton();
    graphInfoButton->setText(tr("Graph Info"));
    graphInfoButton->setFont(textfont);
    graphInfoButton->setMaximumSize(QSize(120, 120));
    graphInfoButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                             "QPushButton:pressed{background-color: lightBlue;} "
                             "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(graphInfoButton, &QPushButton::clicked, this, &Graph::getGraphInfo);

    //vertexTabLayout->addWidget(resetButton); erase functional
    vertexTabLayout->addWidget(clearButton);
    //vertexTabLayout->addWidget(graphInfoButton); erase functional

    QPushButton *AdjMatrixButton = new QPushButton();
    AdjMatrixButton->setText(tr("Adjacency\nmatrix"));
    AdjMatrixButton->setFont(textfont);
    AdjMatrixButton->setMaximumSize(QSize(120, 120));
    AdjMatrixButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkBlue;} "
                             "QPushButton:pressed{background-color: lightBlue;} "
                             "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(AdjMatrixButton, &QPushButton::clicked, this, &Graph::createAdjMatrixWindow);
    vertexTabLayout->addWidget(AdjMatrixButton);

    scene->addWidget(CommandsWidget);
}

void Graph::createAdjMatrixWindow() {//вывод матрицы смежности
    int rows = vertexList.size() + 1;
    int cols = vertexList.size() + 1;
    QTableWidget *AdjMatrixWindow = new QTableWidget(rows, cols);

    AdjMatrixWindow->setColumnWidth(0, 30);
    AdjMatrixWindow->setRowHeight(0, 30);

    for (int i = 0; i < vertexNum; i++) {
        AdjMatrixWindow->setRowHeight(1+i, 30);
        AdjMatrixWindow->setItem(0, i+1, new QTableWidgetItem(QString::number(i)));
        AdjMatrixWindow->item(0, i+1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    for(int i = 0; i < vertexNum; i++) {
        AdjMatrixWindow->setItem(1+i, 0, new QTableWidgetItem(QString::number(i)));
        AdjMatrixWindow->item(1+i, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        for (Edge *edge : vertexList[i]->getEdges()) {
            int pos = edge->destVertex()->getIndex();
            AdjMatrixWindow->setItem(1+i, pos+1, new QTableWidgetItem(QString::number(edge->getWeight())));
            AdjMatrixWindow->item(1+i, pos+1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
        AdjMatrixWindow->setColumnWidth(i+1, 30);
    }
    AdjMatrixWindow->setWindowTitle("Adjacency matrix");
    AdjMatrixWindow->show();
}

int Graph::getVertexNum() {//получение размера списка вершин
    return vertexList.size();
}

void Graph::scaleView(qreal scaleFactor) {//хз
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if(factor < 0.07 || factor > 100){
        return;
    }
    scale(scaleFactor, scaleFactor);
}

void Graph::insertVertex() {//вставка вершины
    int vertexIndex = vertexList.size();
    vertexList.append(new Vertex(this, vertexIndex));
    vertexList[vertexIndex]->setPos(0, 0);
    scene->addItem(vertexList[vertexIndex]);
    vertexNum++;
}

void Graph::updateIndex() {
    for(int i = 0; i < vertexList.size(); i++) {//исправил
        vertexList[i]->setIndex(i);//Правильнее было бы назвать Name или Number
    }
}

void Graph::setVertexPos(int vertexIndex, double x, double y) {//сеттер для позиции вершины
    Vertex *temp = vertexList[vertexIndex];
    temp->setPos(QPointF(x, y));
}

void Graph::eraseVertex(int vertexIndex) {//удаление вершины
    Vertex *temp = vertexList[vertexIndex];
    for(Edge *edge : temp->getEdges()) {//удаление всех дуг, связанных с вершиной
        eraseEdge(vertexIndex, edge->destVertex()->getIndex());
    }
    temp->clearEdge();
    scene->removeItem(temp);
    edgeNum -= vertexList[vertexIndex]->getEdges().size()*2;//изменяем количество дуг
    vertexList.erase(vertexList.begin() + vertexIndex);
    updateIndex();
    vertexNum--;//изменяем количество вершин
}

void Graph::getVertexInfo(int vertexIndex) {//Вывод информации о выбранной вершине
    QTableWidget *infoWindow = new QTableWidget;
    Vertex *temp = vertexList[vertexIndex];
    int vertexDegree = temp->getDegree();
    //формирование таблички
    infoWindow->setRowCount(vertexDegree + 5);
    infoWindow->setColumnCount(2);
    infoWindow->setItem(0, 0, new QTableWidgetItem("Property"));
    infoWindow->setItem(0, 1, new QTableWidgetItem("Value"));

    infoWindow->setItem(1, 0, new QTableWidgetItem("Vertex Index"));
    infoWindow->setItem(1, 1, new QTableWidgetItem(QString::number(vertexIndex)));

    infoWindow->setItem(2, 0, new QTableWidgetItem("Coordinate"));
    QString x = QString::number(temp->getPos().x());
    QString y = QString::number(temp->getPos().y());
    QString coordinate = "X: " + x + " ; y: " + y;
    infoWindow->setItem(2, 1, new QTableWidgetItem(coordinate));

    infoWindow->setItem(3, 0, new QTableWidgetItem("Visited"));
    bool visited = false;
    if(vertexList[vertexIndex]->getColor() == "black"){
        visited = true;
    }
    infoWindow->setItem(3, 1, new QTableWidgetItem(QString::number(visited)));

    infoWindow->setItem(4, 0, new QTableWidgetItem("Vertex Degree"));
    infoWindow->setItem(4, 1, new QTableWidgetItem(QString::number(vertexDegree)));

    infoWindow->setItem(5, 0, new QTableWidgetItem("Adjacent Vertices"));
    int rowCounter = 5;
    for(Edge *edge : vertexList[vertexIndex]->getEdges()){
        QString adjacentVertex = QString::number(vertexIndex) + " -> " + QString::number(edge->destVertex()->getIndex()) + " ; weight: " + QString::number(edge->getWeight());
        infoWindow->setItem(rowCounter, 1, new QTableWidgetItem(adjacentVertex));
        rowCounter++;
    }
    infoWindow->setWindowTitle(tr("Vertex Information Window"));
    infoWindow->setColumnWidth(0, 140);
    infoWindow->setColumnWidth(1, 220);
    infoWindow->setFixedSize(QSize(380, 400));
    infoWindow->show();
}

void Graph::getGraphInfo() {//Вывод информации о графе
    int rows = vertexList.size() + 8;
    int cols = vertexList.size() + 1;
    QTableWidget *infoWindow = new QTableWidget(rows, cols);
    infoWindow->setItem(0, 0, new QTableWidgetItem("Vertex Number"));
    infoWindow->setItem(0, 1, new QTableWidgetItem(QString::number(vertexNum)));
    infoWindow->setItem(1, 0, new QTableWidgetItem("Edge Number"));
    infoWindow->setItem(1, 1, new QTableWidgetItem(QString::number(edgeNum)));
    infoWindow->setItem(2, 0, new QTableWidgetItem("Connected Graph"));
    infoWindow->setItem(2, 1, new QTableWidgetItem(QString::number(connected)));

    QString dijkstra1 = "Dijkstra 's algorithm at index ";
    if (flag==true && dijkstraSignal()!=-1)
    {
        QVector<int> k = dijkstra(dijkstraSignal());
        dijkstra1+= QString::number(dijkstraSignal());
        for(int i = 0; i < k.size(); i++){
            infoWindow->setItem(3, i+1, new QTableWidgetItem(QString::number(k[i])));
        }
    }

    infoWindow->setItem(3, 0, new QTableWidgetItem(dijkstra1));

    QString dfs = "Depth First Search at index ";
    if (!DFS.empty()){
        dfs += QString::number(DFS[0]);
        for (int i = 0; i < DFS.size(); i++) {
            infoWindow->setItem(4, i+1, new QTableWidgetItem(QString::number(DFS[i])));
        }
    }
    infoWindow->setItem(4, 0, new QTableWidgetItem(dfs));

    QString bfs = "Breadth First Search at index ";
    if (!BFS.empty()){
        bfs += QString::number(BFS[0]);
        for (int i = 0; i < BFS.size(); i++) {
            infoWindow->setItem(5, i+1, new QTableWidgetItem(QString::number(BFS[i])));
        }
    }
    infoWindow->setItem(5, 0, new QTableWidgetItem(bfs));
    //вывод матрицы смежности
    infoWindow->setItem(7, 0, new QTableWidgetItem("Graph Matrix"));
    infoWindow->item(7, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    for (int i = 0; i < vertexNum; i++) {
        infoWindow->setItem(7, i+1, new QTableWidgetItem(QString::number(i)));
        infoWindow->item(7, i+1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    for(int i = 0; i < vertexNum; i++) {
        infoWindow->setItem(8+i, 0, new QTableWidgetItem(QString::number(i)));
        infoWindow->item(8+i, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        for (int j = 0; j < vertexNum; j++){
            infoWindow->setItem(8+i, j+1, new QTableWidgetItem("0"));
            infoWindow->item(8+i, j+1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
        for (Edge *edge : vertexList[i]->getEdges()) {
            int pos = edge->destVertex()->getIndex();
            infoWindow->setItem(8+i, pos+1, new QTableWidgetItem(QString::number(edge->getWeight())));
            infoWindow->item(8+i, pos+1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
        infoWindow->setColumnWidth(i+1, 30);
    }
    infoWindow->setWindowTitle("Graph Information Window");
    infoWindow->setColumnWidth(0, 200);
    infoWindow->setFixedSize(QSize(600, 700));
    infoWindow->show();
}

void Graph::insertEdge(int source, int dest, double weight){//Вставка дуги
    if(checkAdjacent(source, dest)) {//если уже соединены - выход
        return;
    }
    //создаём двунаправленную стрелку
    scene->addItem(new Edge(vertexList[source], vertexList[dest], weight));//убрать вес
    scene->addItem(new Edge(vertexList[dest], vertexList[source], weight));
    edgeNum += 2;
}

void Graph::updateWeight(int source, int dest, double weight){//изменение веса
    if (!checkAdjacent(source, dest)){//если не соединены - выход
        return;
    }
    vertexList[source]->updateWeight(dest, weight);
    vertexList[dest]->updateWeight(source, weight);
}

void Graph::eraseEdge(int source, int dest) {//удаление дуги
    if (!checkAdjacent(source, dest)) {//если вершины не соединены - выход
        return;
    }
    //удаление для начальной вершины
    for (Edge *edge : vertexList[source]->getEdges()) {
        if(edge->destVertex()->getIndex() == dest) {
            vertexList[source]->eraseEdge(dest);
            scene->removeItem(edge);
        }
    }
    //удаление для конечной вершины
    for (Edge *edge : vertexList[dest]->getEdges()){
        if(edge->destVertex()->getIndex() == source){
            vertexList[dest]->eraseEdge(source);
            scene->removeItem(edge);
        }
    }
    edgeNum -= 2;
}

bool Graph::checkAdjacent(int source, int dest) {//Проверка, соединены ли вершины
    return vertexList[source]->pathExist(dest);
}

int Graph::getDegree(int vertexIndex) {//хз
    return vertexList[vertexIndex]->getDegree();
}

void Graph::clear() {//полное удаление графа
    while (!vertexList.empty()) {
        eraseVertex(0);
    }
}

void Graph::checkConnected() {//проверка на посещение всех вершин, для проходов
    connected = true;
    for (Vertex *vertex : vertexList) {
        bool visited = false;
        if (vertex->getColor() == "black") {
            visited = true;
        }
        connected &= visited;//хз
    }
}

void Graph::reset() {//сброс прохода по графу
    if (vertexList.size() == 0) {
        return;
    }
    for(int i = 0; i < vertexList.size(); i++) {
        vertexList[i]->setColor("white");
        for(Edge *edge : vertexList[i]->getEdges()) {
            edge->setColor("white");
        }
    }
}

void Graph::dfsUtil(int vertexIndex, QVector<bool> &visited) {//проход в глубину
    QStack<int> dfsStack;//мб сделать рекурсивным?
    dfsStack.push(vertexIndex);
    while (!dfsStack.empty()) {//пока стек не пуст
        int temp = dfsStack.top();
        Vertex *tempVertex = vertexList[temp];
        dfsStack.pop();
        //проверка посещения текущей вершины
        if (!visited[temp]) {
            visited[temp] = true;
            vertexList[temp]->setColor("black");
            DFS.push_back(temp);
            delay(50);//пауза
        }
        //проход по соседям
        for (Edge *tempEdge : tempVertex->getEdges()) {
            if (!visited[tempEdge->destVertex()->getIndex()]) {
                tempEdge->destVertex()->setColor("gray");
                delay(70);
                dfsStack.push(tempEdge->destVertex()->getIndex());
            }
        }
    }
}

void Graph::dfs(int vertexIndex){
    DFS.clear();//чекнуть где находится спискок? - хз
    QVector<bool> visited(vertexList.size(), false);
    dfsUtil(vertexIndex, visited);
    checkConnected();//по идее, можно убрать
}

void Graph::bfsUtil(int vertexIndex, QVector<bool> &visited) {//проход в ширину
    QQueue<int> bfsQueue;
    bfsQueue.push_back(vertexIndex);
    while(!bfsQueue.empty()) {
        int temp = bfsQueue.front();

        Vertex *tempVertex = vertexList[temp];
        bfsQueue.pop_front();
        if(!visited[temp]){
            visited[temp] = true;
            tempVertex->setColor("black");
            BFS.push_back(temp);
            delay(50);
        }
        for (Edge *tempEdge : tempVertex->getEdges()) {
            if(!visited[tempEdge -> destVertex() -> getIndex()]) {
                tempEdge->destVertex()->setColor("gray");
                bfsQueue.push_back(tempEdge->destVertex()->getIndex());
                delay(70);
            }
        }
    }
}

void Graph::bfs(int vertexIndex){
    BFS.clear();
    QVector<bool> visited(vertexList.size(), false);
    bfsUtil(vertexIndex, visited);
    checkConnected();
}

QVector<int> Graph::dijkstra(int vertexIndex) {//алгоритм Дейкстры
    int Adj_matrix[vertexNum][vertexNum];
    for(int i=0; i<vertexNum;i++) {
        for(int j=0; j<vertexNum;j++) {
            if (i==j) {
                Adj_matrix[i][j] = 0;
            }
            else {
                Adj_matrix[i][j] = 100000000;
            }
        }
    }
    PriorityQueue<int> queue;
    for (int i=0; i < vertexNum; i++) {
        queue.enqueue(Adj_matrix[vertexIndex][i],i);
    }
    while (queue.count() > 0) {
        int Curr_vertex = queue.dequeue();
        int edge_count = vertexList[Curr_vertex]->getEdges().count();
        for (int i=0; i < edge_count; i++) {
            Edge* e = vertexList[Curr_vertex]->getEdges()[i];
            Vertex* dest = e->destVertex();
            if (Adj_matrix[vertexIndex][Curr_vertex]+e->getWeight() < Adj_matrix[vertexIndex][dest->getIndex()]) {
                int old = Adj_matrix[vertexIndex][dest->getIndex()];
                Adj_matrix[vertexIndex][dest->getIndex()] = Adj_matrix[vertexIndex][Curr_vertex]+e->getWeight();
                queue.update(old,dest->getIndex(),Adj_matrix[vertexIndex][dest->getIndex()]);//хз
            }
        }
    }
    QVector<int> new_vector;
    for (int i = 0; i < vertexNum; i++) {
        new_vector.append(Adj_matrix[vertexIndex][i]);
    }
    return new_vector;
}

void Graph::delay(int time) {//пауза
    clock_t now = clock();
    while(clock() - now < time);
}

void Graph::createEraseVertexWindow() {//Окно для удаления выбранной вершины
    window = new QWidget;
    input1 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Erase Vertex Index:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(okButton, 1, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Erase Vertex Window");

    connect(okButton, &QPushButton::clicked, this, &Graph::eraseVertexSignal);
    window->show();
}

void Graph::eraseVertexSignal() {//удаление выбранной вершины
    int srcIndex = input1->text().toInt();
    window->close();
    if (srcIndex >= vertexList.size()){
        return;
    }
    eraseVertex(srcIndex);
}

void Graph::createSetVertexPosWindow(){
    window = new QWidget;
    input1 = new QLineEdit;
    input2 = new QLineEdit;
    input3 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Vertex Index:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(new QLabel(tr("X Coordinate")), 1, 0);
    layout->addWidget(input2, 1, 1);
    layout->addWidget(new QLabel(tr("Y Coordinate:")), 2, 0);
    layout->addWidget(input3, 2, 1);
    layout->addWidget(okButton, 3, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Set Vertex Position Window");
    connect(okButton, &QPushButton::clicked, this, &Graph::setVertexPosSignal);
    window->show();
}

void Graph::setVertexPosSignal(){
    int vertexIndex = input1->text().toInt();
    double xvel = input2->text().toDouble();
    double yvel = input3->text().toDouble();
    window->close();

    if (vertexIndex >= vertexList.size()){
        return;
    }
    setVertexPos(vertexIndex, xvel, yvel);
}

void Graph::createVertexInfoWindow() {//окно информации о выбранной вершине
    window = new QWidget;
    input1 = new QLineEdit;
    QLabel *label = new QLabel;
    label -> setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Vertex Index:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(okButton, 1, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Vertex Info Window");

    connect(okButton, &QPushButton::clicked, this, &Graph::vertexInfoSignal);
    window->show();
}

void Graph::vertexInfoSignal() {//вывод информации о выбранной вершине
    int index = input1->text().toInt();
    window->close();
    if (index >= vertexList.size()){
        return;
    }
    getVertexInfo(index);
}

void Graph::createAddEdgeWindow() {//окно для добавления дуги
    window = new QWidget;
    input1 = new QLineEdit;
    input2 = new QLineEdit;
    input3 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Source Vertex Index:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(new QLabel(tr("Dest Vertex Index:")), 1, 0);
    layout->addWidget(input2, 1, 1);
    layout->addWidget(new QLabel(tr("Edge Weight:")), 2, 0);
    layout->addWidget(input3, 2, 1);
    layout->addWidget(okButton, 3, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Add Edge Window");

    connect(okButton, &QPushButton::clicked, this, &Graph::addEdgeSignal);
    window->show();
}

void Graph::addEdgeSignal() {//добавление дуги
    int srcIndex = input1->text().toInt();
    int destIndex = input2->text().toInt();
    double edgeWeight = input3->text().toDouble();
    window->close();
    if (srcIndex >= vertexList.size() || destIndex >= vertexList.size() || edgeWeight <= 0){
        return;
    }
    insertEdge(srcIndex, destIndex, edgeWeight);
}

void Graph::createEraseEdgeWindow() {//окно для удаления выбранной дуги
    window = new QWidget;
    input1 = new QLineEdit;
    input2 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Source Vertex Index:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(new QLabel(tr("Dest Vertex Index:")), 1, 0);
    layout->addWidget(input2, 1, 1);
    layout->addWidget(okButton, 2, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Erase Edge Window");

    connect(okButton, &QPushButton::clicked, this, &Graph::eraseEdgeSignal);
    window->show();
}

void Graph::eraseEdgeSignal() {//удаление выбранной дуги
    int srcIndex = input1->text().toInt();
    int destIndex = input2->text().toInt();
    window->close();
    if (srcIndex >= vertexList.size() || destIndex >= vertexList.size()){
        return;
    }
    eraseEdge(srcIndex, destIndex);
}

void Graph::createUpdateWeightWindow() {//окно для изменения веса
    window = new QWidget;
    input1 = new QLineEdit;
    input2 = new QLineEdit;
    input3 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Source Vertex Index:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(new QLabel(tr("Dest Vertex Index:")), 1, 0);
    layout->addWidget(input2, 1, 1);
    layout->addWidget(new QLabel(tr("Edge Weight:")), 2, 0);
    layout->addWidget(input3, 2, 1);
    layout->addWidget(okButton, 3, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Update Weight Window");

    connect(okButton, &QPushButton::clicked, this, &Graph::updateWeightSignal);
    window->show();
}

void Graph::updateWeightSignal() {//изменение веса
    int srcIndex = input1->text().toInt();
    int destIndex = input2->text().toInt();
    double edgeWeight = input3->text().toDouble();
    window->close();
    if(srcIndex >= vertexList.size() || destIndex >= vertexList.size() || edgeWeight <= 0){
        return;
    }
    updateWeight(srcIndex, destIndex, edgeWeight);
}

void Graph::createFunctionWindow() {
    window = new QWidget;
    input1 = new QLineEdit;
    input5 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setText("1. DFS\n2. BFS\n3. Dijkstra's algorithm\n4. Traveling salesman problem\n");
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(label, 0, 0);
    layout->addWidget(new QLabel(tr("Num of function:")), 1, 0);
    layout->addWidget(input5, 1, 1);
    layout->addWidget(new QLabel(tr("Starting Vertex Index:")), 2, 0);
    layout->addWidget(input1, 2, 1);
    layout->addWidget(okButton, 3, 0, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Choose function");

    connect(okButton, &QPushButton::clicked, this, &Graph::StartSelectedFunction);
    window->show();
}

void Graph::runDFS(int temp, bool* visited) {
    if (!visited[temp]) {
        visited[temp] = true;
        str += "Вершина " + QString::number(temp) + " посещена\n";
    }

    Vertex *tempVertex = vertexList[temp];
    for (Edge *tempEdge : tempVertex->getEdges()) {//проход по соседям
        if (!visited[tempEdge->destVertex()->getIndex()]) {
            runDFS(tempEdge->destVertex()->getIndex(), visited);
        }
    }
}

void Graph::runBFS(int index, bool* visited, QQueue<int>* bfsQueue) {
    if (visited[index] == false) {
        bfsQueue->push_back(index);
        str += "Вершина " + QString::number(index) + " обработана\n";
        visited[index] = true;
    }
    Vertex *tempVertex = vertexList[index];
    bfsQueue->pop_front();
    for (Edge *tempEdge : tempVertex->getEdges()) {
        if(!visited[tempEdge->destVertex()->getIndex()]) {
            str += "Вершина " + QString::number(tempEdge ->destVertex() ->getIndex()) + " обработана\n";
            visited[tempEdge->destVertex()->getIndex()] = true;
            bfsQueue->push_back(tempEdge->destVertex()->getIndex());
        }
    }
    if (!bfsQueue->empty()) {
        runBFS(bfsQueue->front(), visited, bfsQueue);
    }
}

void Graph::StartSelectedFunction() {
    int index = input1->text().toInt();
    int NumOfFunction = input5->text().toInt();
    if (index >= 0 && index < vertexList.size() && NumOfFunction >= 0 && NumOfFunction < 5) {
        enum Functions {
            DFS = 1, BFS, Dijkstra, Travel
        };
        switch (NumOfFunction) {
        case(Functions::DFS): {
            bool* visited = new bool[vertexList.size()];
            runDFS(index, visited);
            createDFSWindow();
            delete[] visited;
            break;
        }
        case(Functions::BFS): {
            bool* visited = new bool[vertexList.size()];
            runBFS(index, visited, new QQueue<int>);
            createBFSWindow();
            delete[] visited;
            break;
        }
        case(Functions::Dijkstra): {
            //дописать
            break;
        }
        case(Functions::Travel): {
            //дописать
            break;
        }
        }
    }
}

void Graph::createDFSWindow() {
    window = new QWidget;
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(str), 0, 0);
    str = "";
    window->setLayout(layout);
    window->setWindowTitle("DFS");
    window->show();
}

void Graph::createBFSWindow() {
    window = new QWidget;
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(str), 0, 0);
    str = "";
    window->setLayout(layout);
    window->setWindowTitle("BFS");
    window->show();
}

void Graph::dfsSignal(){
    int index = input1->text().toInt();
    window->close();
    if(index >= vertexList.size()){
        return;
    }
    dfs(index);
}

void Graph::createDijkstraWindow() {
    window = new QWidget;
    input1 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Starting Vertex Index:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(okButton, 1, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Dijkstra Window");

    connect(okButton, &QPushButton::clicked, this, &Graph::dijkstraSignal);
    window->show();
}

void Graph::bfsSignal() {
    int index = input1->text().toInt();
    window->close();
    if(index >= vertexList.size()){
        return;
    }
    bfs(index);
}

int Graph::dijkstraSignal() {
    flag = true;
    int index=input1->text().toInt();
    window->close();
    if(index >= vertexList.size()){
        return -1;
    }
    return index;
}
