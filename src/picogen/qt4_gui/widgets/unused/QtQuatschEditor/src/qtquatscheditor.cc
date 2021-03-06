//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Copyright (C) 2009  Sebastian Mach (*1983)
// * mail: phresnel/at/gmail/dot/com
// * http://phresnel.org
// * http://picogen.org
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "qtquatscheditor.hh"
#include "ui_qtquatscheditor.h"

#include <iostream>
#include <algorithm>

#include <QComboBox>


EditorScene::EditorScene (QtQuatschEditor *editor)
: QGraphicsScene (editor), editor(editor), currentNodeItem (0) {
}

void EditorScene::dropEvent (QGraphicsSceneDragDropEvent*) {
        if (0 == currentNodeItem) {
                return;
        }

        NodeItem *child;
        switch (dropType) {
        case SetType:
                child = currentNodeItem;
                break;
        case AddChild:
                child = currentNodeItem->addChild();
                break;
        case InsertLeftSibling:
                child = currentNodeItem->insertLeftSibling();
                break;
        case InsertRightSibling:
                child = currentNodeItem->insertRightSibling();
                break;
        };

        // TODO that's plain ugly
        child->setType (
                (QuatschNodeType)editor
                 ->ui
                 ->nodeTypesTreeWidget
                 ->currentItem()
                 ->data(0, Qt::UserRole).toInt()
        );

        currentNodeItem->select();
        currentNodeItem->clearHighlight();
        currentNodeItem = 0;
}

void EditorScene::dragEnterEvent(QGraphicsSceneDragDropEvent*) {
        QList<QGraphicsItem*> selected =
                        editor->ui->graphicsView->scene()->selectedItems();
        for (QList<QGraphicsItem*>::iterator it = selected.begin();
             it != selected.end();
             ++it
        ) {
                if (*it != (QGraphicsItem*)this)
                        (*it)->setSelected (false);
        }
}

void EditorScene::dragMoveEvent(QGraphicsSceneDragDropEvent* e) {
        using redshift::tuple;
        using redshift::get;

        // TODO smells on the hardcode
        const QPointF C = e->scenePos();
        if (e->source()->objectName().toStdString()=="nodeTypesTreeWidget"){
                tuple<NodeItem*, DropType>
                        nearest = editor->findDropNode(C,0);
                if (0 != get<0>(nearest)) {
                        dropType = get<1>(nearest);
                        currentNodeItem = get<0>(nearest);

                        switch (get<1>(nearest)) {
                        case AddChild:
                                currentNodeItem->highlight(NodeItem::Right);
                                break;
                        case InsertLeftSibling:
                                currentNodeItem->highlight(NodeItem::Top);
                                break;
                        case InsertRightSibling:
                                currentNodeItem->highlight(NodeItem::Bottom);
                                break;
                        case SetType:
                                currentNodeItem->highlight(NodeItem::Complete);
                                break;
                        };
                }
        } else {
                currentNodeItem = 0;
        }
}

void EditorScene::dragLeaveEvent(QGraphicsSceneDragDropEvent*) {
        if (0 != currentNodeItem)
                currentNodeItem->clearHighlight();
        currentNodeItem = 0;
}



GLGraphicsScene::GLGraphicsScene (QWidget *parent)
: QGraphicsScene (parent), currentNode(0) {
}

void GLGraphicsScene::drawBackground (QPainter* painter, const QRectF & rect) {
        Q_UNUSED(painter)
        Q_UNUSED(rect)

        struct : public instant_preview::HeightFunction {
                float operator () (float u, float v) const {
                        const float argv [] = {u, v};
                        return (*f) (argv);
                }
                Compiler::FunctionPtr f;
        } fun;

        struct : public instant_preview::HeightFunction {
                float operator () (float, float) const {
                        return 0.0f;
                }
        } fun0;

        if (0 != currentNode) {
                fun.f = currentNode->genQuatsch();
                instant_preview::draw(fun);
        } else {
                instant_preview::draw(fun0);
        }
}

QRectF GLGraphicsScene::itemsBoundingRect () const {
        return QRectF (-1.0,-1.0,2.0,2.0);
}

void GLGraphicsScene::setCurrentNode (NodeItem *node) {
        currentNode = node;
        this->invalidate();
}


namespace {
struct node_t {
        QString text;
        QString icon;
        QuatschNodeType type;

        node_t (QString text, QString icon, QuatschNodeType type)
        : text(text), icon(icon), type(type)
        {}
};
void addCategory (QTreeWidget *tree, QString text, QString icon, std::vector<node_t> items) {

        QTreeWidgetItem *cat = new QTreeWidgetItem ();

        cat->setText(0, text);
        cat->setIcon (0, QIcon (icon));
        cat->setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        tree->addTopLevelItem (cat);

        for (std::vector<node_t>::const_iterator it=items.begin();
             it!=items.end();
             ++it
        ) {
                QTreeWidgetItem *item = new QTreeWidgetItem();
                item->setText (0, it->text);
                item->setIcon (0, QIcon(it->icon));
                item->setData (0, Qt::UserRole, QVariant(it->type));
                item->setFlags (Qt::ItemIsSelectable
                              | Qt::ItemIsEnabled
                              | Qt::ItemIsDragEnabled);
                cat->addChild(item);
        }
}
}



QtQuatschEditor::QtQuatschEditor(QWidget *parent)
    : QWidget(parent), ui(new Ui::QtQuatschEditor)
    , currentPropertyWidget (0)
{
        ui->setupUi(this);

        /*{
                descriptors[NodeItem::Undefined] = NodeItemDescriptor (
                                false, -1, -1);
                descriptors[NodeItem::Parameter] = NodeItemDescriptor (
                                false, 0, 0);
                descriptors[NodeItem::UserConstant] = NodeItemDescriptor (
                                false, 0, 0);
                descriptors[NodeItem::PredefinedConstant] = NodeItemDescriptor (
                                false, 0, 0);
                descriptors[NodeItem::Addition] = NodeItemDescriptor (
                                false, -1, -1);
                descriptors[NodeItem::Subtraction] = NodeItemDescriptor (
                                false, -1, -1);
                descriptors[NodeItem::Multiplication] = NodeItemDescriptor (
                                false, -1, -1);
                descriptors[NodeItem::Division] = NodeItemDescriptor (
                                false, -1, -1);
        }*/


        // Graph Editor
        {
                //QGraphicsScene *scene = new QGraphicsScene (this);
                editorScene = new EditorScene (this);
                editorScene->setItemIndexMethod(QGraphicsScene::NoIndex);
                ui->graphicsView->setScene(editorScene);
                ui->graphicsView->setRenderHint(QPainter::Antialiasing);

                QObject::connect(editorScene, SIGNAL(selectionChanged()),
                  this, SLOT(on_graphicsScene_selectionChanged()));

                /*QObject::connect(ui->graphicsView, SIGNAL(dropEvent(QGraphicsSceneDragDropEvent*)),
                  this, SLOT(on_graphicsScene_dropEvent(QDropEvent*)));*/

                this->rootNode = new NodeItem (editorScene, this, this, 0, 0);

                rootNode->doLayout();

                QRectF rect = editorScene->itemsBoundingRect();
                ui->graphicsView->setSceneRect(rect);
                ui->graphicsView->show();
        }

        // Heightmap 3d Preview
        {
                glScene = new GLGraphicsScene (this);
                glScene->setSceneRect(0.0,0.0,1.0,1.0);
                ui->heightmapView->setScene(glScene);
                ui->heightmapView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
                ui->heightmapView->setRenderHint(QPainter::Antialiasing);
                ui->heightmapView->setSceneRect(glScene->itemsBoundingRect());
        }

        // Heightmap 2d Preview
        {
                heightmap = QPixmap (256,256);
                ui->heightmapLabel->setPixmap(heightmap);
        }

        // Node Types Tree Widget
        {
                QTreeWidget *t = ui->nodeTypesTreeWidget;

                {
                        std::vector<node_t> items;
                        items.push_back(node_t ("Parameter", ":/aggregate/parameter", Parameter));
                        items.push_back(node_t ("User Constant", ":/aggregate/userconstant", UserConstant));
                        items.push_back(node_t ("Predefined Constant", ":/aggregate/predefinedconstant", PredefinedConstant));
                        addCategory(t, "Terminal", ":/aggregate/terminal", items);
                }

                {
                        std::vector<node_t> items;
                        items.push_back(node_t ("Addition", ":/aggregate/addition", Addition ));
                        items.push_back(node_t ("Subtraction", ":/aggregate/subtraction", Subtraction ));
                        items.push_back(node_t ("Multiplication", ":/aggregate/multiplication", Multiplication ));
                        items.push_back(node_t ("Division", ":/aggregate/division", Division ));
                        items.push_back(node_t ("Absolute", ":/aggregate/absolute", Absolute));
                        items.push_back(node_t ("Minimize", ":/aggregate/minimize", Minimize ));
                        items.push_back(node_t ("Maximize", ":/aggregate/maximize", Maximize ));
                        items.push_back(node_t ("Negate", ":/aggregate/negate", Negate ));
                        items.push_back(node_t ("Lerp", ":/aggregate/lerp", Lerp));

                        addCategory(t, "Basic", ":/aggregate/multiplication", items);
                }

                {
                        std::vector<node_t> items;
                        items.push_back(node_t ("Sine", ":/aggregate/sine", Sine));
                        items.push_back(node_t ("Cosine", ":/aggregate/cosine", Cosine));

                        items.push_back(node_t ("Exponentiate (x^y)", ":/aggregate/exponentiate", Exponentiate ));
                        items.push_back(node_t ("Exponentiate (exp(x))", ":/aggregate/exp", Exp));
                        items.push_back(node_t ("SquareRoot", ":/aggregate/sqrt", Sqrt));

                        items.push_back(node_t ("Inverse", ":/aggregate/inverse", Inverse));

                        items.push_back(node_t ("Logarithm", ":/aggregate/log", Log));
                        items.push_back(node_t ("Logarithm_10", ":/aggregate/log10", Log10));

                        addCategory(t, "Non-Linear", ":/aggregate/cosine", items);
                }

                {
                        std::vector<node_t> items;
                        items.push_back(node_t ("Floor", ":/aggregate/floor", Floor));
                        items.push_back(node_t ("Truncate", ":/aggregate/truncate", Truncate));
                        items.push_back(node_t ("Fractional", ":/aggregate/fractional", Fractional));

                        addCategory(t, "Fractions & Integers", ":/aggregate/cosine", items);
                }

                {
                        std::vector<node_t> items;
                        items.push_back(node_t ("And", ":/aggregate/and", And));
                        items.push_back(node_t ("Or", ":/aggregate/or", Or));
                        items.push_back(node_t ("Not", ":/aggregate/not", Not));

                        items.push_back(node_t ("Less-Than", ":/aggregate/lessthan", LessThan));
                        items.push_back(node_t ("Less-Than-Or-Equal", ":/aggregate/lessthanorequal", LessThanOrEqual));
                        items.push_back(node_t ("Greater-Than", ":/aggregate/greaterthan", GreaterThan));
                        items.push_back(node_t ("Greater-Than-Or-Equal", ":/aggregate/greaterthanorequal", GreaterThanOrEqual));
                        addCategory(t, "Relation & Logical", ":/aggregate/relation", items);
                }

                {
                        std::vector<node_t> items;
                        items.push_back(node_t ("If-Then-Else", ":/aggregate/ifthenelse", IfThenElse));
                        addCategory(t, "Control Flow", ":/aggregate/controlflow", items);
                }

                {
                        std::vector<node_t> items;
                        items.push_back(node_t ("Noise", ":/aggregate/noise", Noise2d));
                        items.push_back(node_t ("Layer-Noise", ":/aggregate/layernoise", LayeredNoise2d));
                        addCategory(t, "Complex", ":/aggregate/complex", items);
                }
        }

        on_graphicsScene_selectionChanged();
}



QtQuatschEditor::~QtQuatschEditor() {
    delete ui;
}



redshift::tuple<NodeItem *, DropType>
 QtQuatschEditor::findDropNode (QPointF const &C, NodeItem *exclude)
 const {
        using redshift::make_tuple;

        QGraphicsItem *graphicsItem = editorScene->itemAt (C);
        NodeItem* nearest = dynamic_cast<NodeItem*>(graphicsItem);
        if (0 != nearest && nearest != exclude) {
                return make_tuple (nearest, SetType);
        }

        qreal nearestDist = -1;
        typedef QList<QGraphicsItem*> ItemList;
        ItemList items = editorScene->items();

        for (ItemList::iterator it = items.begin();
             it != items.end();
             ++it
        ) {
                NodeItem* item = dynamic_cast<NodeItem*>(*it);
                if (0 != item
                    && item != exclude
                    && C.x() >= item->sceneBoundingRect().left()
                ) {
                        const QRectF bb = item->sceneBoundingRect();
                        const QPointF diff = bb.center() - C;
                        const qreal dist = sqrtf (
                                diff.x()*diff.x()+
                                diff.y()*diff.y()
                        );

                        if (nearestDist < 0.0f
                          || dist < nearestDist) {
                                nearestDist = dist;
                                nearest = item;
                        }
                }
        }
        if (nearest == 0)
                return make_tuple ((NodeItem*)0, AddChild);

        if (nearest == rootNode)
                return make_tuple (nearest, AddChild);

        const QRectF bb = nearest->sceneBoundingRect();
        if (C.x() >= bb.left() && C.x() <= bb.right()) {
                if (C.y() <= nearest->pos().y())
                        return make_tuple (nearest, InsertLeftSibling);
                else
                        return make_tuple (nearest, InsertRightSibling);
        }

        return make_tuple (nearest, AddChild);
}



void QtQuatschEditor::mousePressEvent(QGraphicsSceneMouseEvent *event) {
        Q_UNUSED(event)
}



void QtQuatschEditor::keyPressEvent(QKeyEvent* event) {
        if (event->modifiers().testFlag(Qt::ControlModifier)) {
                ui->graphicsView->verticalScrollBar()->setEnabled(false);
                ui->graphicsView->horizontalScrollBar()->setEnabled(false);
        }
}



void QtQuatschEditor::keyReleaseEvent(QKeyEvent* event) {
        Q_UNUSED(event)
        ui->graphicsView->verticalScrollBar()->setEnabled(true);
        ui->graphicsView->horizontalScrollBar()->setEnabled(true);
}



void QtQuatschEditor::wheelEvent(QWheelEvent* wheel) {
        if (wheel->modifiers().testFlag(Qt::ControlModifier)) {
                wheel->accept();
                const float
                        degrees =  static_cast<float>(wheel->delta()) / 8.f,
                        scale_ = 1.0f + degrees * 0.01f,
                        scale = scale_ < 0.125f ? 0.125f : scale_
                ;
                ui->graphicsView->scale(scale, scale);
        }
}



void QtQuatschEditor::on_swapWith3dPreview_clicked() {
        /*QWidget *ob = ui->graphicsView->parentWidget();
        ui->graphicsView->setParent(ui->heightmapView->parentWidget());
        ui->heightmapView->setParent(ob);*/
        /*QGraphicsScene *tmp = ui->graphicsView->scene();
        ui->graphicsView->setScene(ui->heightmapView->scene());
        ui->heightmapView->setScene(tmp);*/
        /*QLayout *tmp = ui->graphicsView->layout();
        ui->graphicsView->setLayout(ui->heightmapView->layout());
        ui->heightmapView->setLayout(tmp);*/
        /*QLayoutItem *tmp = ui->previewLayout->itemAt(0);
        ui->previewLayout->rem*/
        /*
        QLayoutItem *a = ui->previewFrame->layout()->itemAt(0);
        QLayoutItem *b = ui->editorFrame->layout()->itemAt(0);

        ui->previewFrame->layout()->removeItem(a);
        ui->editorFrame->layout()->removeItem(b);
        ui->previewFrame->layout()->addItem(b);
        ui->editorFrame->layout()->addItem(a);*/
        //ui->previewFrame->set
}



void QtQuatschEditor::on_addNodeButton_clicked() {
        QList<QGraphicsItem*> selected = ui->graphicsView->scene()->selectedItems();

        if (0 == selected.size()) {
                return;
        } else if (1 == selected.size()) {
                NodeItem* node = dynamic_cast<NodeItem*> (selected [0]);
                if (0 != node) {
                        node->addChild();
                }
        }
}



void QtQuatschEditor::on_insertLeftSiblingButton_clicked() {
        QList<QGraphicsItem*> selected = ui->graphicsView->scene()->selectedItems();

        if (0 == selected.size()) {
                return;
        } else if (1 == selected.size()) {
                NodeItem* node = dynamic_cast<NodeItem*> (selected [0]);
                if (0 != node) {
                        node->insertLeftSibling();
                }
        }
}



void QtQuatschEditor::on_insertRightSiblingButton_clicked() {
        QList<QGraphicsItem*> selected = ui->graphicsView->scene()->selectedItems();

        if (0 == selected.size()) {
                return;
        } else if (1 == selected.size()) {
                NodeItem* node = dynamic_cast<NodeItem*> (selected [0]);
                if (0 != node) {
                        node->insertRightSibling();
                }
        }
}



void QtQuatschEditor::on_graphicsScene_selectionChanged() {
        QList<QGraphicsItem*> selected = ui->graphicsView->scene()->selectedItems();
        if (1 == selected.size()) {
                NodeItem* node = dynamic_cast<NodeItem*> (selected [0]);
                if (0 == node)
                        return;

                const QuatschNodeType type = node->getType();
                ui->typeCombo->setCurrentIndex (type);

                const QuatschNodeValue value = node->getValue();

                ui->scale->setValue(value.getScale());
                ui->offset->setValue(value.getOffset());
                switch (value.getScaleOffsetMode()) {
                case QuatschNodeValue::disable:
                        ui->scaleOffsetMode->setCurrentIndex(0);
                        break;
                case QuatschNodeValue::offset_scale:
                        ui->scaleOffsetMode->setCurrentIndex(1);
                        break;
                case QuatschNodeValue::scale_offset:
                        ui->scaleOffsetMode->setCurrentIndex(2);
                        break;
                };
                // CurrentIndexChanged() seems to not be called
                // by Qt upon setCurrentIndex().
                on_scaleOffsetMode_currentIndexChanged(
                                ui->scaleOffsetMode->currentIndex());

                if (node->isRootItem()) {
                        ui->insertLeftSiblingButton->setEnabled (false);
                        ui->insertRightSiblingButton->setEnabled (false);
                        ui->addNodeButton->setEnabled (true);
                        ui->deleteNodeButton->setEnabled (false);
                        ui->typeCombo->setEnabled (true);
                } else {
                        ui->insertLeftSiblingButton->setEnabled (true);
                        ui->insertRightSiblingButton->setEnabled (true);
                        ui->addNodeButton->setEnabled (true);
                        ui->typeCombo->setEnabled (true);
                        ui->deleteNodeButton->setEnabled (true);
                }
                ui->centerOnSelectedButton->setEnabled (true);
                ui->nodePropertiesGroupBox->setEnabled(true);
                updateHeightmap (node);

        } else {
                ui->insertLeftSiblingButton->setEnabled (false);
                ui->insertRightSiblingButton->setEnabled (false);
                ui->addNodeButton->setEnabled (false);
                ui->typeCombo->setEnabled (false);
                ui->centerOnSelectedButton->setEnabled (false);
                ui->deleteNodeButton->setEnabled (false);
                ui->typeCombo->setCurrentIndex(0);
                ui->nodePropertiesGroupBox->setEnabled(false);

                updateHeightmap (0);
        }

        displayPropertyWindow();
}



void QtQuatschEditor::updateHeightmap () {
        QList<QGraphicsItem*> selected = ui->graphicsView->scene()->selectedItems();
        if (1 == selected.size()) {
                updateHeightmap (dynamic_cast<NodeItem*> (selected [0]));
        } else {
                updateHeightmap (0);
        }
}



void QtQuatschEditor::updateHeightmap (NodeItem *node) {
        if (0 == node) {
                node = rootNode;
        }

        if (QuatschNode(*node).isCompilable()) {
                QImage q = node->genHeightmap(256,256);
                heightmap = QPixmap::fromImage(q);
                ui->heightmapLabel->setPixmap (heightmap);
                drawHeightmap3d (q);

                glScene->setCurrentNode(node);
        } else {
                QImage q(256,256,QImage::Format_RGB32);
                q.fill(QColor(200,130,130).rgb());
                heightmap = QPixmap::fromImage(q);
                ui->heightmapLabel->setPixmap (heightmap);

                glScene->setCurrentNode(0);
        }

        repaint();
}



void QtQuatschEditor::on_typeCombo_currentIndexChanged(int index) {
        QList<QGraphicsItem*> selected = ui->graphicsView->scene()->selectedItems();
        if (1 == selected.size()) {
                NodeItem* node = dynamic_cast<NodeItem*> (selected [0]);
                if (0 == node)
                        return;

                node->setType(static_cast<QuatschNodeType>(index));
                updateHeightmap();
        }
        displayPropertyWindow();
}



void QtQuatschEditor::on_scaleOffsetMode_currentIndexChanged(int index) {
        QList<QGraphicsItem*> selected = ui->graphicsView->scene()->selectedItems();
        if (1 == selected.size()) {
                NodeItem* node = dynamic_cast<NodeItem*> (selected [0]);
                if (0 == node)
                        return;

                QuatschNodeValue val = node->getValue();
                switch (index) {
                case 0:
                        val.setScaleOffsetMode(QuatschNodeValue::disable);
                        ui->scale->setEnabled(false);
                        ui->offset->setEnabled(false);
                        break;
                case 1:
                        val.setScaleOffsetMode(QuatschNodeValue::offset_scale);
                        ui->scale->setEnabled(true);
                        ui->offset->setEnabled(true);
                        break;
                case 2:
                        val.setScaleOffsetMode(QuatschNodeValue::scale_offset);
                        ui->scale->setEnabled(true);
                        ui->offset->setEnabled(true);
                        break;
                };

                node->setValue(val);
                updateHeightmap();
        }
        displayPropertyWindow();
}



void QtQuatschEditor::on_offset_valueChanged(double offset) {
        QList<QGraphicsItem*> selected = ui->graphicsView->scene()->selectedItems();
        if (1 == selected.size()) {
                NodeItem* node = dynamic_cast<NodeItem*> (selected [0]);
                if (0 == node)
                        return;

                QuatschNodeValue val = node->getValue();
                val.setOffset(offset);
                node->setValue(val);
                updateHeightmap();
        }
        displayPropertyWindow();
}



void QtQuatschEditor::on_scale_valueChanged(double scale) {
        QList<QGraphicsItem*> selected = ui->graphicsView->scene()->selectedItems();
        if (1 == selected.size()) {
                NodeItem* node = dynamic_cast<NodeItem*> (selected [0]);
                if (0 == node)
                        return;

                QuatschNodeValue val = node->getValue();
                val.setScale(scale);
                node->setValue(val);
                updateHeightmap();
        }
        displayPropertyWindow();
}



void QtQuatschEditor::on_completeViewButton_clicked() {
        QRectF rect = ui->graphicsView->scene()->itemsBoundingRect();
        ui->graphicsView->setSceneRect(rect);
        rect.adjust(-30,-30,30,30);
        ui->graphicsView->fitInView (rect, Qt::KeepAspectRatio);
        ui->graphicsView->show();
}



void QtQuatschEditor::on_centerOnSelectedButton_clicked() {
        QList<QGraphicsItem*> selected = ui->graphicsView->scene()->selectedItems();
        if (1 == selected.size()) {
                ui->graphicsView->resetTransform();
                ui->graphicsView->centerOn(selected[0]);
        }
}



void QtQuatschEditor::on_deleteNodeButton_clicked() {
        QList<QGraphicsItem*> selected = ui->graphicsView->scene()->selectedItems();
        if (1 == selected.size()) {
                NodeItem* node = dynamic_cast<NodeItem*> (selected [0]);
                if (0 == node)
                        return;

                QMessageBox box;
                box.setText("This will delete ALL children, too!");
                box.setIcon(QMessageBox::Warning);
                box.setStandardButtons(QMessageBox::Abort | QMessageBox::Apply);
                box.setDefaultButton(QMessageBox::Abort);
                if (box.exec() == QMessageBox::Apply) {
                        node->die();
                }
        }
}



void QtQuatschEditor::displayPropertyWindow() {
        if (0 != currentPropertyWidget) {
                ui->nodePropertiesLayout->removeWidget (currentPropertyWidget);
                delete currentPropertyWidget;
                currentPropertyWidget = 0;
        }


        ui->helpText->setText("");

        QList<QGraphicsItem*> selected = ui->graphicsView->scene()->selectedItems();
        if (1 == selected.size()) {
                NodeItem* node = dynamic_cast<NodeItem*> (selected [0]);
                if (0 == node)
                        return;

                switch (node->getType()) {
                case Undefined:
                        break;
                case PredefinedConstant:
                        currentPropertyWidget = new PredefinedConstantUi(node);
                        ui->nodePropertiesLayout->addWidget(currentPropertyWidget);
                        ui->helpText->setText("Use one of the predefined constants.");
                        break;
                case Parameter:
                        currentPropertyWidget = new ParameterUi(node);
                        ui->nodePropertiesLayout->addWidget(currentPropertyWidget);
                        break;
                case UserConstant:
                        currentPropertyWidget = new UserConstantUi(node);
                        ui->nodePropertiesLayout->addWidget(currentPropertyWidget);
                        ui->helpText->setText("Enter a custom value.");
                        break;
                case Addition:
                        ui->helpText->setText("Sums up the values of its child nodes.");
                        break;
                case Subtraction:
                        ui->helpText->setText("Like Addition, but subtracts all values instead, beginning at the top most child.");
                        break;
                case Multiplication:
                        break;
                case Division:
                        break;
                case Exponentiate:
                        ui->helpText->setText(
                                "Raises the first operand to the power of the second: <big>x<sup>y</sup></big> <br>"
                                "<strong>Note:</strong> This function accepts arbitrarily many "
                                "operands, raising the 1st operand to the power of the 2nd to the power of the 3rd, etc."
                        );
                        break;
                case Minimize:
                        break;
                case Maximize:
                        break;
                case Negate:
                        break;
                case Lerp:
                        /*ui->helpText->setText(
                                "Performs a linear interpolation between the 2nd, 3rd, n'th child. "
                                "The <strong>1st</strong> child defines which of the other children "
                                "are used / contribute at most."
                                "<p>E.g., if the 1st child is 0, then the second child is the only "
                                "child that is used. If it is 1, then the last child is used. "
                                "And, assuming there are 3 children (apart from the 1st), a value "
                                "of 0.5 would mean the 2nd child is used. <br>"
                                "<p>Mathematically speaking, the formula for two child nodes would be "
                                "<center><dfn>h = (1-x<sub>1</sub>)*x<sub>2</sub> + x<sub>1</sub>*x<sub>3</sub></dfn></center><br>"
                                "with x<sub>n</sub> being the children. "
                                "<p>As an extension for your convenience, lerp also works with only two operands, "
                                "in which case lerp will always select the 2nd operand."
                        );*/
                        break;
                case And:
                        break;
                case Or:
                        break;
                case Not:
                        break;
                case LessThan:
                        break;
                case LessThanOrEqual:
                        break;
                case GreaterThan:
                        break;
                case GreaterThanOrEqual:
                        break;
                case Inverse:
                        break;
                case Sine:
                        break;
                case Cosine:
                        break;

                case Floor:
                        break;
                case Absolute:
                        break;
                case Truncate:
                        break;
                case Fractional:
                        break;

                case Sqrt:
                        break;
                case Log:
                        break;
                case Log10:
                        break;
                case Exp:
                        break;

                case IfThenElse:
                        break;

                case Noise2d:
                        currentPropertyWidget = new Noise2dUi(node);
                        ui->nodePropertiesLayout->addWidget(currentPropertyWidget);
                        break;
                case LayeredNoise2d:
                        currentPropertyWidget = new LayeredNoise2dUi(node);
                        ui->nodePropertiesLayout->addWidget(currentPropertyWidget);
                        break;

                case MultiplyWithPi:
                        break;
                };
                this->repaint();
        }
}



void QtQuatschEditor::on_pushButton_5_clicked() {
        QMessageBox box;
        box.setWindowTitle("code");
        box.setText(rootNode->genJuxCode());
        box.exec();
}



void QtQuatschEditor::on_moveNodeUpButton_clicked() {
        QList<QGraphicsItem*> selected = ui->graphicsView->scene()->selectedItems();
        if (1 == selected.size()) {
                NodeItem* node = dynamic_cast<NodeItem*> (selected [0]);
                if (0 == node)
                        return;
                node->moveUp();
        }
}



void QtQuatschEditor::on_moveNodeDownButton_clicked() {
        QList<QGraphicsItem*> selected = ui->graphicsView->scene()->selectedItems();
        if (1 == selected.size()) {
                NodeItem* node = dynamic_cast<NodeItem*> (selected [0]);
                if (0 == node)
                        return;
                node->moveDown();
        }
}



void QtQuatschEditor::on_asParentsSiblingButton_clicked() {
        QList<QGraphicsItem*> selected = ui->graphicsView->scene()->selectedItems();
        if (1 == selected.size()) {
                NodeItem* node = dynamic_cast<NodeItem*> (selected [0]);
                if (0 == node)
                        return;
                node->asParentsSibling();
        }
}



void QtQuatschEditor::on_asLeftSiblingsChildButton_clicked() {
        QList<QGraphicsItem*> selected = ui->graphicsView->scene()->selectedItems();
        if (1 == selected.size()) {
                NodeItem* node = dynamic_cast<NodeItem*> (selected [0]);
                if (0 == node)
                        return;
                node->asLeftSiblingsChild();
        }
}



void QtQuatschEditor::on_asRightSiblingsChildButton_clicked() {
        QList<QGraphicsItem*> selected = ui->graphicsView->scene()->selectedItems();
        if (1 == selected.size()) {
                NodeItem* node = dynamic_cast<NodeItem*> (selected [0]);
                if (0 == node)
                        return;
                node->asRightSiblingsChild();
        }
}



void QtQuatschEditor::drawHeightmap3d (QImage const &/*heightmap*/) {
        ui->heightmapView->repaint();
}



void QtQuatschEditor::on_pushButton_clicked() {
        /*using namespace actuarius;
        std::ofstream os ("~/Desktop/qtquatschser.txt");
        OArchive(os) & pack ("heightmap", *this->rootNode);*/
        std::ofstream os ("/home/smach/Desktop/qtquatschser.txt");
        if (0 != rootNode)
                rootNode->serialize ("heightmap", os);
}



void QtQuatschEditor::on_pushButton_2_clicked() {
        /*using namespace actuarius;
        std::ofstream os ("~/Desktop/qtquatschser.txt");
        OArchive(os) & pack ("heightmap", *this->rootNode);*/
        std::ifstream os ("/home/smach/Desktop/qtquatschser.txt");
        if (0 != rootNode)
                rootNode->deserialize ("heightmap", os);
}
