/********************************************************************************
** Form generated from reading UI file 'OpenDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.0.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPENDIALOG_H
#define UI_OPENDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_OpenDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *sourceLineEdit;
    QLabel *label_2;
    QLineEdit *descriptionLineEdit;
    QLabel *label_3;
    QLineEdit *copyrightLineEdit;
    QTreeView *treeView;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *OpenDialog)
    {
        if (OpenDialog->objectName().isEmpty())
            OpenDialog->setObjectName(QStringLiteral("OpenDialog"));
        OpenDialog->resize(526, 384);
        verticalLayout = new QVBoxLayout(OpenDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(OpenDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        sourceLineEdit = new QLineEdit(OpenDialog);
        sourceLineEdit->setObjectName(QStringLiteral("sourceLineEdit"));
        sourceLineEdit->setReadOnly(true);

        gridLayout->addWidget(sourceLineEdit, 0, 1, 1, 1);

        label_2 = new QLabel(OpenDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        descriptionLineEdit = new QLineEdit(OpenDialog);
        descriptionLineEdit->setObjectName(QStringLiteral("descriptionLineEdit"));
        descriptionLineEdit->setReadOnly(true);

        gridLayout->addWidget(descriptionLineEdit, 1, 1, 1, 1);

        label_3 = new QLabel(OpenDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        copyrightLineEdit = new QLineEdit(OpenDialog);
        copyrightLineEdit->setObjectName(QStringLiteral("copyrightLineEdit"));
        copyrightLineEdit->setReadOnly(true);

        gridLayout->addWidget(copyrightLineEdit, 2, 1, 1, 1);


        horizontalLayout_2->addLayout(gridLayout);


        verticalLayout->addLayout(horizontalLayout_2);

        treeView = new QTreeView(OpenDialog);
        treeView->setObjectName(QStringLiteral("treeView"));

        verticalLayout->addWidget(treeView);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        spacerItem = new QSpacerItem(48, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(spacerItem);

        okButton = new QPushButton(OpenDialog);
        okButton->setObjectName(QStringLiteral("okButton"));
        okButton->setDefault(true);

        horizontalLayout->addWidget(okButton);

        cancelButton = new QPushButton(OpenDialog);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        horizontalLayout->addWidget(cancelButton);


        verticalLayout->addLayout(horizontalLayout);

        QWidget::setTabOrder(sourceLineEdit, descriptionLineEdit);
        QWidget::setTabOrder(descriptionLineEdit, copyrightLineEdit);
        QWidget::setTabOrder(copyrightLineEdit, treeView);
        QWidget::setTabOrder(treeView, okButton);
        QWidget::setTabOrder(okButton, cancelButton);

        retranslateUi(OpenDialog);
        QObject::connect(okButton, SIGNAL(clicked()), OpenDialog, SLOT(accept()));
        QObject::connect(cancelButton, SIGNAL(clicked()), OpenDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(OpenDialog);
    } // setupUi

    void retranslateUi(QDialog *OpenDialog)
    {
        OpenDialog->setWindowTitle(QApplication::translate("OpenDialog", "Open Library", 0));
        label->setText(QApplication::translate("OpenDialog", "Library Source:", 0));
        label_2->setText(QApplication::translate("OpenDialog", "Description:", 0));
        label_3->setText(QApplication::translate("OpenDialog", "Copyright:", 0));
        okButton->setText(QApplication::translate("OpenDialog", "Open", 0));
        cancelButton->setText(QApplication::translate("OpenDialog", "Cancel", 0));
    } // retranslateUi

};

namespace Ui {
    class OpenDialog: public Ui_OpenDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPENDIALOG_H
