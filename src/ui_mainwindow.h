/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.0.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionExportFiles;
    QAction *actionExit;
    QAction *actionAbout;
    QAction *actionDownloadAll;
    QAction *actionPlayVideo;
    QAction *actionSmallIconView;
    QAction *actionLargeIconView;
    QAction *actionDetailView;
    QAction *actionDownloadVideo;
    QAction *actionMediumIconView;
    QAction *actionOpen;
    QAction *actionFlowView;
    QAction *actionRecentFiles;
    QAction *actionPreferences;
    QAction *actionDownloadCategory;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QMenu *menu_Help;
    QMenu *menu_View;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(826, 506);
        actionExportFiles = new QAction(MainWindow);
        actionExportFiles->setObjectName(QStringLiteral("actionExportFiles"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionExit->setMenuRole(QAction::QuitRole);
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionAbout->setMenuRole(QAction::AboutRole);
        actionDownloadAll = new QAction(MainWindow);
        actionDownloadAll->setObjectName(QStringLiteral("actionDownloadAll"));
        actionPlayVideo = new QAction(MainWindow);
        actionPlayVideo->setObjectName(QStringLiteral("actionPlayVideo"));
        actionSmallIconView = new QAction(MainWindow);
        actionSmallIconView->setObjectName(QStringLiteral("actionSmallIconView"));
        actionSmallIconView->setCheckable(true);
        actionSmallIconView->setChecked(false);
        actionLargeIconView = new QAction(MainWindow);
        actionLargeIconView->setObjectName(QStringLiteral("actionLargeIconView"));
        actionLargeIconView->setCheckable(true);
        actionDetailView = new QAction(MainWindow);
        actionDetailView->setObjectName(QStringLiteral("actionDetailView"));
        actionDetailView->setCheckable(true);
        actionDetailView->setChecked(true);
        actionDownloadVideo = new QAction(MainWindow);
        actionDownloadVideo->setObjectName(QStringLiteral("actionDownloadVideo"));
        actionMediumIconView = new QAction(MainWindow);
        actionMediumIconView->setObjectName(QStringLiteral("actionMediumIconView"));
        actionMediumIconView->setCheckable(true);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionFlowView = new QAction(MainWindow);
        actionFlowView->setObjectName(QStringLiteral("actionFlowView"));
        actionFlowView->setCheckable(true);
        actionRecentFiles = new QAction(MainWindow);
        actionRecentFiles->setObjectName(QStringLiteral("actionRecentFiles"));
        actionPreferences = new QAction(MainWindow);
        actionPreferences->setObjectName(QStringLiteral("actionPreferences"));
        actionPreferences->setMenuRole(QAction::PreferencesRole);
        actionDownloadCategory = new QAction(MainWindow);
        actionDownloadCategory->setObjectName(QStringLiteral("actionDownloadCategory"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 826, 26));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QStringLiteral("menu_File"));
        menu_Help = new QMenu(menuBar);
        menu_Help->setObjectName(QStringLiteral("menu_Help"));
        menu_View = new QMenu(menuBar);
        menu_View->setObjectName(QStringLiteral("menu_View"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menu_File->menuAction());
        menuBar->addAction(menu_View->menuAction());
        menuBar->addAction(menu_Help->menuAction());
        menu_File->addAction(actionOpen);
        menu_File->addAction(actionRecentFiles);
        menu_File->addSeparator();
        menu_File->addAction(actionDownloadAll);
        menu_File->addAction(actionExportFiles);
        menu_File->addSeparator();
        menu_File->addAction(actionPreferences);
        menu_File->addSeparator();
        menu_File->addAction(actionExit);
        menu_Help->addAction(actionAbout);
        menu_View->addAction(actionDetailView);
        menu_View->addAction(actionSmallIconView);
        menu_View->addAction(actionLargeIconView);
        menu_View->addAction(actionFlowView);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "USB Video Manager", 0));
        actionExportFiles->setText(QApplication::translate("MainWindow", "&Export for USB Media Player...", 0));
        actionExit->setText(QApplication::translate("MainWindow", "E&xit", 0));
        actionAbout->setText(QApplication::translate("MainWindow", "&About USB Video Manager...", 0));
        actionDownloadAll->setText(QApplication::translate("MainWindow", "&Download All", 0));
        actionPlayVideo->setText(QApplication::translate("MainWindow", "&Play", 0));
#ifndef QT_NO_TOOLTIP
        actionPlayVideo->setToolTip(QApplication::translate("MainWindow", "Play Video", 0));
#endif // QT_NO_TOOLTIP
        actionSmallIconView->setText(QApplication::translate("MainWindow", "&Small Icons", 0));
        actionLargeIconView->setText(QApplication::translate("MainWindow", "&Large Icons", 0));
        actionDetailView->setText(QApplication::translate("MainWindow", "&Details", 0));
        actionDownloadVideo->setText(QApplication::translate("MainWindow", "&Download", 0));
        actionMediumIconView->setText(QApplication::translate("MainWindow", "&Medium Icons", 0));
        actionOpen->setText(QApplication::translate("MainWindow", "&Open Library...", 0));
        actionFlowView->setText(QApplication::translate("MainWindow", "Gallery", 0));
        actionRecentFiles->setText(QApplication::translate("MainWindow", "&Recent Libraries", 0));
        actionPreferences->setText(QApplication::translate("MainWindow", "&Preferences...", 0));
        actionDownloadCategory->setText(QApplication::translate("MainWindow", "&Download", 0));
        menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0));
        menu_Help->setTitle(QApplication::translate("MainWindow", "&Help", 0));
        menu_View->setTitle(QApplication::translate("MainWindow", "&View", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
