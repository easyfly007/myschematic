#include "mainwindow.h"
#include "view/schematic_scene.h"
#include "view/schematic_view.h"
#include "tools/tool_manager.h"
#include <myschematic/circuit.h>
#include <myschematic/sheet.h>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QUndoStack>
#include <QLabel>
#include <QKeySequence>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_circuit(new myschematic::Circuit(this))
    , m_scene(new SchematicScene(this))
    , m_view(new SchematicView(m_scene, this))
{
    setWindowTitle(QStringLiteral("MySchematic"));
    resize(1200, 800);

    // Create default sheet
    m_sheet = m_circuit->createSheet(QStringLiteral("top"));
    m_scene->setSheet(m_sheet);

    setCentralWidget(m_view);

    setupMenus();
    setupToolbar();
    setupStatusBar();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupMenus() {
    // File menu
    auto* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&New"), this, []() {}, QKeySequence::New);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("E&xit"), this, &QWidget::close, QKeySequence::Quit);

    // Edit menu
    auto* editMenu = menuBar()->addMenu(tr("&Edit"));
    auto* undoAction = m_scene->undoStack()->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcut(QKeySequence::Undo);
    editMenu->addAction(undoAction);

    auto* redoAction = m_scene->undoStack()->createRedoAction(this, tr("&Redo"));
    redoAction->setShortcut(QKeySequence::Redo);
    editMenu->addAction(redoAction);

    // View menu
    auto* viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(tr("Fit to &Window"), m_view, &SchematicView::fitToWindow,
                        QKeySequence(Qt::CTRL | Qt::Key_0));

    // Tools menu
    auto* toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(tr("&Select"), this, [this]() {
        m_scene->toolManager()->setActiveTool(QStringLiteral("select"));
    }, QKeySequence(Qt::Key_Escape));

    toolsMenu->addAction(tr("&Place Component"), this, [this]() {
        m_scene->toolManager()->setActiveTool(QStringLiteral("place"));
    }, QKeySequence(Qt::Key_I));
}

void MainWindow::setupToolbar() {
    auto* toolbar = addToolBar(tr("Main"));
    toolbar->setMovable(false);

    toolbar->addAction(tr("Select"), [this]() {
        m_scene->toolManager()->setActiveTool(QStringLiteral("select"));
    });

    toolbar->addAction(tr("Place"), [this]() {
        m_scene->toolManager()->setActiveTool(QStringLiteral("place"));
    });

    toolbar->addSeparator();

    auto* undoAction = m_scene->undoStack()->createUndoAction(this, tr("Undo"));
    undoAction->setShortcut(QKeySequence::Undo);
    toolbar->addAction(undoAction);

    auto* redoAction = m_scene->undoStack()->createRedoAction(this, tr("Redo"));
    redoAction->setShortcut(QKeySequence::Redo);
    toolbar->addAction(redoAction);
}

void MainWindow::setupStatusBar() {
    statusBar()->showMessage(tr("Ready"));

    auto* toolLabel = new QLabel(this);
    statusBar()->addPermanentWidget(toolLabel);

    connect(m_scene->toolManager(), &ToolManager::activeToolChanged,
            toolLabel, [toolLabel](const QString& name) {
                toolLabel->setText(QStringLiteral("Tool: ") + name);
            });
}
