#pragma once
#include <QMainWindow>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QMap>
#include <QFormLayout> // Added this since you're using it
#include "model/MonsterManager.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private:
    // Only need these two now
    QMap<QString, QLineEdit*> statInputs; 
    QLineEdit *nameInput;
    QLineEdit *hpInput; // You can keep HP separate if you like, or move to map

    void setupUI();
    void applyTheme();
    void refreshMonsterList();
    Monster getMonsterFromUI();

    QVBoxLayout *layout;
    QListWidget *monsterList;
    MonsterManager manager;
};