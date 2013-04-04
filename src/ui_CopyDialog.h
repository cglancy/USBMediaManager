/********************************************************************************
** Form generated from reading UI file 'CopyDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.0.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COPYDIALOG_H
#define UI_COPYDIALOG_H

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
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_CopyDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *pathLabel;
    QLineEdit *pathLineEdit;
    QPushButton *browseButton;
    QLabel *label_2;
    QLineEdit *totalLineEdit;
    QLabel *label;
    QLineEdit *availableLineEdit;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *CopyDialog)
    {
        if (CopyDialog->objectName().isEmpty())
            CopyDialog->setObjectName(QStringLiteral("CopyDialog"));
        CopyDialog->resize(482, 165);
        verticalLayout = new QVBoxLayout(CopyDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        pathLabel = new QLabel(CopyDialog);
        pathLabel->setObjectName(QStringLiteral("pathLabel"));
        pathLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(pathLabel, 0, 0, 1, 1);

        pathLineEdit = new QLineEdit(CopyDialog);
        pathLineEdit->setObjectName(QStringLiteral("pathLineEdit"));

        gridLayout->addWidget(pathLineEdit, 0, 1, 1, 1);

        browseButton = new QPushButton(CopyDialog);
        browseButton->setObjectName(QStringLiteral("browseButton"));

        gridLayout->addWidget(browseButton, 0, 2, 1, 1);

        label_2 = new QLabel(CopyDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        totalLineEdit = new QLineEdit(CopyDialog);
        totalLineEdit->setObjectName(QStringLiteral("totalLineEdit"));
        totalLineEdit->setReadOnly(true);

        gridLayout->addWidget(totalLineEdit, 1, 1, 1, 1);

        label = new QLabel(CopyDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label, 2, 0, 1, 1);

        availableLineEdit = new QLineEdit(CopyDialog);
        availableLineEdit->setObjectName(QStringLiteral("availableLineEdit"));
        availableLineEdit->setReadOnly(true);

        gridLayout->addWidget(availableLineEdit, 2, 1, 1, 1);


        verticalLayout->addLayout(gridLayout);

        verticalSpacer = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        okButton = new QPushButton(CopyDialog);
        okButton->setObjectName(QStringLiteral("okButton"));
        okButton->setDefault(true);

        hboxLayout->addWidget(okButton);

        cancelButton = new QPushButton(CopyDialog);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        hboxLayout->addWidget(cancelButton);


        verticalLayout->addLayout(hboxLayout);


        retranslateUi(CopyDialog);
        QObject::connect(okButton, SIGNAL(clicked()), CopyDialog, SLOT(accept()));
        QObject::connect(cancelButton, SIGNAL(clicked()), CopyDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(CopyDialog);
    } // setupUi

    void retranslateUi(QDialog *CopyDialog)
    {
        CopyDialog->setWindowTitle(QApplication::translate("CopyDialog", "Export Videos for USB Media Player", 0));
        pathLabel->setText(QApplication::translate("CopyDialog", "Destination folder:", 0));
        browseButton->setText(QApplication::translate("CopyDialog", "Browse...", 0));
        label_2->setText(QApplication::translate("CopyDialog", "Required space:", 0));
        label->setText(QApplication::translate("CopyDialog", "Available space:", 0));
        okButton->setText(QApplication::translate("CopyDialog", "Export", 0));
        cancelButton->setText(QApplication::translate("CopyDialog", "Cancel", 0));
    } // retranslateUi

};

namespace Ui {
    class CopyDialog: public Ui_CopyDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COPYDIALOG_H
