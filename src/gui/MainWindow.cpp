// MainWindow.cpp
#include "model/MonsterManager.hpp"
#include "model/Templates.hpp"
#include <QListWidget>
#include <QFormLayout>
#include <QTableWidget>
#include <QSpinBox>
#include <QHeaderView>
#include "gui/MainWindow.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  auto *central = new QWidget(this);
  layout = new QVBoxLayout(central);
  setCentralWidget(central);

  applyTheme();
	setupUI();

	refreshMonsterList();
}

void MainWindow::applyTheme() {
	QString bg = "#2B2B2B";
	QString text = "#ffffff";
  // Variable trick for Dark Mode readability
  QString style = QString(R"(
    QWidget { background-color: %1; color: %2; }
	  QLineEdit { background-color: #3c3c3c; color: %2 }
	  QListWidget { background-color: %1; color: %2; border: 1px solid #555; }
  )").arg(bg, text);

	this->setStyleSheet(style);
}

void MainWindow::setupUI() {
  // 1. Initialize the layout first
  auto *formLayout = new QFormLayout();

  // 2. Define your inputs
  nameInput = new QLineEdit();
  formLayout->addRow("Name:", nameInput);

  QStringList stats = {"Strength", "Dexterity", "Constitution", "Intelligence", "Wisdom", "Charisma"};
  for (const QString &stat : stats) {
    auto *input = new QLineEdit("10");
    auto *modLabel = new QLabel("0");

    input->setMaximumWidth(50);

    auto *rowLayout = new QHBoxLayout();
    rowLayout->addWidget(input);
    rowLayout->addWidget(modLabel);
    rowLayout->addStretch();

    statInputs[stat] = input; // Add to your map
    modLabels[stat] = modLabel;
    formLayout->addRow(stat + ":", rowLayout);

    connect(input, &QLineEdit::textChanged, [input, modLabel]() {
      int val = input->text().toInt();
      int mod = (val - 10) / 2;
      QString modStr = (mod >= 0) ? "+" + QString::number(mod) : QString::number(mod);
      modLabel->setText("(" + modStr + ")");
    });
  }

  abilityInput = new QLineEdit();
  abilityInput->setPlaceholderText("Enter ability (e.g. Fly, Darkvision)...");
  auto *addAbilityBtn = new QPushButton("Add Ability");

  abilityList = new QListWidget();
  abilityList->setMaximumHeight(100);

  formLayout->addRow("New Ability:", abilityInput);
  formLayout->addWidget(addAbilityBtn);
  formLayout->addRow("Abilities:", abilityList);

  connect(addAbilityBtn, &QPushButton::clicked, [this]() {
    if (!abilityInput->text().isEmpty()) {
      abilityList->addItem(abilityInput->text());
      abilityInput->clear();
    }
  });

  actionInput = new QLineEdit();
  actionInput->setPlaceholderText("Enter action (e.g. Multiattack, Bite)...");
  auto *addActionBtn = new QPushButton("Add Action");

  actionList = new QListWidget();
  actionList->setMaximumHeight(100);

  formLayout->addRow("New Action:", actionInput);
  formLayout->addWidget(addActionBtn);
  formLayout->addRow("Actions:", actionList);

  connect(addActionBtn, &QPushButton::clicked, [this]() {
    if (!actionInput->text().isEmpty()) {
      actionList->addItem(actionInput->text());
      actionInput->clear();
    }
  });

  legendaryActionInput = new QLineEdit();
  legendaryActionInput->setPlaceholderText("Enter action (e.g. Multiattack, Bite)...");
  auto *addLegendaryActionBtn = new QPushButton("Add Legendary Action");

  legendaryActionList = new QListWidget();
  legendaryActionList->setMaximumHeight(100);

  formLayout->addRow("New Legendary Action:", legendaryActionInput);
  formLayout->addWidget(addLegendaryActionBtn);
  formLayout->addRow("Legendary Actions:", legendaryActionList);

  connect(addLegendaryActionBtn, &QPushButton::clicked, [this]() {
    if (!legendaryActionInput->text().isEmpty()) {
      legendaryActionList->addItem(legendaryActionInput->text());
      legendaryActionInput->clear();
    }
  });

  reactionInput = new QLineEdit();
  reactionInput->setPlaceholderText("Enter reaction (e.g. Multiattack, Bite)...");
  auto *addReactionBtn = new QPushButton("Add Reaction");

  reactionList = new QListWidget();
  reactionList->setMaximumHeight(100);

  formLayout->addRow("New Reaction:", reactionInput);
  formLayout->addWidget(addReactionBtn);
  formLayout->addRow("Reactions:", reactionList);

  connect(addReactionBtn, &QPushButton::clicked, [this]() {
    if (!reactionInput->text().isEmpty()) {
      reactionList->addItem(reactionInput->text());
      reactionInput->clear();
    }
  });

	skillTable = new QTableWidget(0, 2);
	skillTable->setHorizontalHeaderLabels({"Skill", "Bonus"});
	skillTable->horizontalHeader()->setStretchLastSection(true);
	skillTable->setMaximumHeight(150);

	auto *addSkillBtn = new QPushButton("Add Skill");
	auto *removeSkillBtn = new QPushButton("Remove Selected");
	auto *skillNameInput = new QLineEdit();
	auto *skillBonusInput = new QSpinBox();

	formLayout->addRow("Skill Name:", skillNameInput);
	formLayout->addRow("Bonus", skillBonusInput);
	formLayout->addWidget(addSkillBtn);
	formLayout->addWidget(removeSkillBtn);
	formLayout->addRow("Skills:", skillTable);

	connect(addSkillBtn, &QPushButton::clicked, [this, skillNameInput, skillBonusInput]() {
		if (!skillNameInput->text().isEmpty()) {
			int row = skillTable->rowCount();
			skillTable->insertRow(row);
			skillTable->setItem(row, 0, new QTableWidgetItem(skillNameInput->text()));
			skillTable->setItem(row, 1, new QTableWidgetItem(skillBonusInput->text()));
			skillNameInput->clear();
		}
	});

	connect(removeSkillBtn, &QPushButton::clicked, [this]() {
		QList<QTableWidgetSelectionRange> selected = skillTable->selectedRanges();
		for (int i = selected.count() -1; i >= 0; i--) {
			int topRow = selected[i].topRow();
			int bottomRow = selected[i].bottomRow();
			for (int row = bottomRow; row >= topRow; --row) {
				skillTable->removeRow(row);
			}
		}
	});

	savingThrowTable = new QTableWidget(0, 2);
	savingThrowTable->setHorizontalHeaderLabels({"Save", "Bonus"});
  savingThrowTable->horizontalHeader()->setStretchLastSection(true);
  savingThrowTable->setMaximumHeight(150);

  auto *addSaveBtn = new QPushButton("Add Saving Throw");
  auto *saveNameInput = new QLineEdit();
  auto *saveBonusInput = new QSpinBox();

  formLayout->addRow("Save Name:", saveNameInput);
  formLayout->addRow("Save Bonus:", saveBonusInput);
  formLayout->addWidget(addSaveBtn);
  formLayout->addRow("Saving Throws:", savingThrowTable);

  connect(addSaveBtn, &QPushButton::clicked, [this, saveNameInput, saveBonusInput]() {
    if (!saveNameInput->text().isEmpty()) {
      int row = savingThrowTable->rowCount();
      savingThrowTable->insertRow(row);
      savingThrowTable->setItem(row, 0, new QTableWidgetItem(saveNameInput->text()));
      savingThrowTable->setItem(row, 1, new QTableWidgetItem(saveBonusInput->text()));
      saveNameInput->clear();
    }
});

  hpInput = new QLineEdit("20");
  formLayout->addRow("HP:", hpInput);
  acInput = new QLineEdit("12");
  formLayout->addRow("AC:", acInput);
  passiveInput = new QLineEdit("10");
  formLayout->addRow("Passive Perception ", passiveInput);
  crInput = new QLineEdit("0.5");
  formLayout->addRow("CR", crInput);

  // 3. Add to main layout
  monsterList = new QListWidget(this);
  auto *saveBtn = new QPushButton("Save Monster", this);

  layout->addLayout(formLayout);
  layout->addWidget(monsterList);
  layout->addWidget(saveBtn);

	connect(saveBtn, &QPushButton::clicked, [this]() {
		// Construct and save
		Monster m = getMonsterFromUI();
		if (m.name.empty()) return;

		manager.saveMonster(m);

		// Add to UI
		auto *item = new QListWidgetItem(QString::fromStdString(m.name));
		monsterList->addItem(item);

		refreshMonsterList();
	});

	auto *deleteBtn = new QPushButton("Delete Monster", this);
	layout->addWidget(deleteBtn);

	connect(deleteBtn, &QPushButton::clicked, [this]() {
		// Get the currently selected item
		QListWidgetItem *selected = monsterList->currentItem();
		if (!selected) return; // Nothing selected

		// Delete from storage
		manager.deleteMonster(selected->text().toStdString());

		// Refresh the UI to reflect changes
		refreshMonsterList();
	});

	connect(monsterList, &QListWidget::itemSelectionChanged, [this]() {
		QListWidgetItem *selected = monsterList->currentItem();
		if (!selected) return;

		// Fetch the monster from your manager
		std::string name = selected->text().toStdString();
		Monster m = manager.findMonster(name); // You'll need to add this method

		// Populate the UI
		nameInput->setText(QString::fromStdString(m.name));
		hpInput->setText(QString::number(m.hp));
    acInput->setText(QString::number(m.ac));
    passiveInput->setText(QString::number(m.passivePerception));
    crInput->setText(QString::number(m.cr));

    abilityList->clear();
		actionList->clear();
		legendaryActionList->clear();
		reactionList->clear();

		// 1. Clear existing rows
    skillTable->setRowCount(0);
    savingThrowTable->setRowCount(0);

    // 2. Repopulate Skills Table
    for (auto const& [name, bonus] : m.skills) {
      int row = skillTable->rowCount();
      skillTable->insertRow(row);
      skillTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(name)));
      skillTable->setItem(row, 1, new QTableWidgetItem(QString::number(bonus)));
}

    // 3. Repopulate Saving Throws Table
    for (auto const& [name, bonus] : m.savingThrows) {
      int row = savingThrowTable->rowCount();
      savingThrowTable->insertRow(row);
      savingThrowTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(name)));
      savingThrowTable->setItem(row, 1, new QTableWidgetItem(QString::number(bonus)));
	    }

    for (const auto& ability : m.abilities) {
      abilityList->addItem(QString::fromStdString(ability));
    }

		for (const auto& action : m.actions) {
			actionList->addItem(QString::fromStdString(action));
		}

		for (auto it = statInputs.begin(); it != statInputs.end(); ++it) {
			int val = m.getStat(it.key().toStdString());
			it.value()->setText(QString::number(val));
		}
  });
}

Monster MainWindow::getMonsterFromUI() {
  std::vector<std::string> currentAbilities;
  for(int i = 0; i < abilityList->count(); i++) {
    currentAbilities.push_back(abilityList->item(i)->text().toStdString());
  }

  std::vector<std::string> currentActions;
  for(int j = 0; j < actionList->count(); j++) {
	currentActions.push_back(actionList->item(j)->text().toStdString());
  }

  std::vector<std::string> currentLegendaryActions;
  for(int l = 0; l < legendaryActionList->count(); l++) {
	currentLegendaryActions.push_back(legendaryActionList->item(l)->text().toStdString());
  }

  std::vector<std::string> currentReactions;
  for(int m = 0; m < reactionList->count(); m++) {
	currentReactions.push_back(reactionList->item(m)->text().toStdString());
  }

	std::map<std::string, int> currentSkills;
	for(int k = 0; k < skillTable->rowCount(); k++) {
		std::string name = skillTable->item(k, 0)->text().toStdString();
		int bonus = skillTable->item(k, 1)->text().toInt();
		currentSkills[name] = bonus;
	}

	std::map<std::string, int> currentSaves;
  for(int i = 0; i < savingThrowTable->rowCount(); i++) {
    std::string name = savingThrowTable->item(i, 0)->text().toStdString();
    int bonus = savingThrowTable->item(i, 1)->text().toInt();
    currentSaves[name] = bonus;
  }

  return Monster{
    nameInput->text().toStdString(),             // 1
    statInputs["Strength"]->text().toInt(),      // 2
    statInputs["Dexterity"]->text().toInt(),     // 3
    statInputs["Constitution"]->text().toInt(),  // 4
    statInputs["Intelligence"]->text().toInt(),  // 5
    statInputs["Wisdom"]->text().toInt(),        // 6
    statInputs["Charisma"]->text().toInt(),      // 7
    hpInput->text().toInt(),                     // 8
    acInput->text().toInt(),                     // 9
    passiveInput->text().toInt(),                // 10
    crInput->text().toDouble(),                  // 11
    currentAbilities,                            // 12
    currentActions,                              // 13
    currentSaves,                                // 14
    currentSkills,                               // 15
		currentLegendaryActions,                     // 16
		currentReactions                             // 17
  };
}

void MainWindow::refreshMonsterList() {
  monsterList->clear(); // Clear existing to avoid duplicates
  auto monsters = manager.loadAll();
  for (const auto& m : monsters) {
    monsterList->addItem(QString::fromStdString(m.name));
  }
}
