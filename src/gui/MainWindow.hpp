// MainWindow.hpp
#pragma once
#include <QMainWindow>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QMap>
#include <QLabel>
#include <QFormLayout> // Added this since you're using it
#include <QTableWidget>
#include "model/MonsterManager.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private:
    // Only need these two now
    QMap<QString, QLineEdit*> statInputs; 
    QMap<QString, QLabel*> modLabels; // To display "+2", "-1", etc.
    QLineEdit *nameInput;
    QLineEdit *acInput;
    QLineEdit *crInput;
    QLineEdit *passiveInput;
    QLineEdit *hpInput; // You can keep HP separate if you like, or move to map
    QListWidget *abilityList;
    QLineEdit *abilityInput;
	QListWidget *actionList;
	QLineEdit *actionInput;
	QTableWidget *skillTable;
	QTableWidget *savingThrowTable;
	QListWidget *legendaryActionList;
	QLineEdit *legendaryActionInput;
	QListWidget *reactionList;
	QLineEdit *reactionInput;

    void setupUI();
    void applyTheme();
    void refreshMonsterList();
    Monster getMonsterFromUI();

    QVBoxLayout *layout;
    QListWidget *monsterList;
    MonsterManager manager;
};
