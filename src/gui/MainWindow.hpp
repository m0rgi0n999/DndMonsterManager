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
#include <QTabWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QGroupBox>
#include "model/MonsterManager.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private:
    // Only need these two now
    QMap<QString, QLineEdit*> statInputs;
    QMap<QString, QLabel*> modLabels; // To display "+2", "-1", etc.
    QMap<QString, QCheckBox*> dmgResistanceCheckBoxes;
    QMap<QString, QCheckBox*> dmgImmunityCheckBoxes;
    QMap<QString, QCheckBox*> condImmunityCheckBoxes;
    QLineEdit *nameInput;
    QLineEdit *acInput;
    QLineEdit *crInput;
    QLineEdit *passiveInput;
    QLineEdit *hpInput;
    QLineEdit *currentHpInput;
    QLineEdit *initiativeInput;
    QListWidget *templateList;
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
	QListWidget *sensesList;
	QLineEdit *sensesInput;
	QListWidget *languagesList;
	QLineEdit *languagesInput;

    void setupUI();
    void applyTheme();
    void refreshMonsterList();
    Monster getMonsterFromUI();
    void populateCheckboxGroup(const std::vector<std::string>& values, QMap<QString, QCheckBox*>& checkboxes);
    std::vector<std::string> collectCheckedValues(const QMap<QString, QCheckBox*>& checkboxes) const;

    QHBoxLayout *layout;
    QTabWidget *tabWidget;
    QListWidget *monsterList;
    MonsterManager manager;
};
