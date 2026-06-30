// MainWindow.cpp
#include "model/MonsterManager.hpp"
#include "model/Templates.hpp"
#include <QListWidget>
#include <QFormLayout>
#include <QTableWidget>
#include <QSpinBox>
#include <QHeaderView>
#include <QScrollArea>
#include <QComboBox>
#include <set>
#include "gui/MainWindow.hpp"

namespace {
void addUniqueListItem(QListWidget *list, const QString &text) {
  if (text.trimmed().isEmpty()) {
    return;
  }

  for (int i = 0; i < list->count(); ++i) {
    if (list->item(i)->text().trimmed() == text.trimmed()) {
      return;
    }
  }

  list->addItem(text);
}

void addUniqueTableRow(QTableWidget *table, const QString &name, const QString &bonus) {
  if (name.trimmed().isEmpty()) {
    return;
  }

  for (int row = 0; row < table->rowCount(); ++row) {
    QTableWidgetItem *existingName = table->item(row, 0);
    if (existingName && existingName->text().trimmed() == name.trimmed()) {
      if (QTableWidgetItem *existingBonus = table->item(row, 1)) {
        existingBonus->setText(bonus);
      } else {
        table->setItem(row, 1, new QTableWidgetItem(bonus));
      }
      return;
    }
  }

  int row = table->rowCount();
  table->insertRow(row);
  table->setItem(row, 0, new QTableWidgetItem(name));
  table->setItem(row, 1, new QTableWidgetItem(bonus));
}

void removeSelectedListItems(QListWidget *list) {
  const auto selected = list->selectedItems();
  for (QListWidgetItem *item : selected) {
    const int row = list->row(item);
    if (row >= 0) {
      delete list->takeItem(row);
    }
  }
}

void removeSelectedTableRows(QTableWidget *table) {
  const auto selected = table->selectedRanges();
  for (int i = selected.count() - 1; i >= 0; --i) {
    const int topRow = selected[i].topRow();
    const int bottomRow = selected[i].bottomRow();
    for (int row = bottomRow; row >= topRow; --row) {
      table->removeRow(row);
    }
  }
}

std::vector<std::string> collectUniqueListValues(QListWidget *list) {
  std::vector<std::string> values;
  std::set<std::string> seen;

  for (int i = 0; i < list->count(); ++i) {
    std::string value = list->item(i)->text().trimmed().toStdString();
    if (!value.empty() && seen.insert(value).second) {
      values.push_back(value);
    }
  }

  return values;
}
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  auto *central = new QWidget(this);
  layout = new QHBoxLayout(central);
  setCentralWidget(central);
  resize(1650, 975);
  setMinimumSize(1350, 825);

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
  // Create a sidebar for the monster list and a main area for the tabs
  auto *sidebarWidget = new QWidget(this);
  auto *sidebarLayout = new QVBoxLayout(sidebarWidget);

  monsterList = new QListWidget(this);
  monsterList->setMaximumWidth(240);
  sidebarLayout->addWidget(new QLabel("Monsters:"));
  sidebarLayout->addWidget(monsterList);

  auto *saveBtn = new QPushButton("Save Monster", this);
  auto *deleteBtn = new QPushButton("Delete Monster", this);
  auto *buttonRow = new QHBoxLayout();
  buttonRow->addWidget(saveBtn);
  buttonRow->addWidget(deleteBtn);
  sidebarLayout->addLayout(buttonRow);

  auto *contentWidget = new QWidget(this);
  auto *contentLayout = new QVBoxLayout(contentWidget);

  // Create tab widget and logical tabs: General, Actions, Defenses
  tabWidget = new QTabWidget();

  QWidget *generalTab = new QWidget();
  QFormLayout *generalLayout = new QFormLayout(generalTab);

  QWidget *actionsTab = new QWidget();
  QVBoxLayout *actionsLayout = new QVBoxLayout(actionsTab);

  QWidget *defensesTab = new QWidget();
  auto *defensesTabLayout = new QVBoxLayout(defensesTab);
  auto *defensesScroll = new QScrollArea(defensesTab);
  defensesScroll->setWidgetResizable(true);
  defensesScroll->setFrameShape(QFrame::NoFrame);

  auto *defensesContent = new QWidget(defensesTab);
  QFormLayout *defensesLayout = new QFormLayout(defensesContent);
  defensesScroll->setWidget(defensesContent);
  defensesTabLayout->addWidget(defensesScroll);

  // 2. Define your inputs (General tab)
  nameInput = new QLineEdit();
  generalLayout->addRow("Name:", nameInput);

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
    generalLayout->addRow(stat + ":", rowLayout);

    connect(input, &QLineEdit::textChanged, [input, modLabel]() {
      int val = input->text().toInt();
      int mod = (val - 10) / 2;
      QString modStr = (mod >= 0) ? "+" + QString::number(mod) : QString::number(mod);
      modLabel->setText("(" + modStr + ")");
    });
  }

  // Actions tab: Abilities, Actions, Legendary, Reactions
  abilityInput = new QLineEdit();
  abilityInput->setPlaceholderText("Enter ability (e.g. Fly, Darkvision)...");
  auto *addAbilityBtn = new QPushButton("Add Ability");
  auto *removeAbilityBtn = new QPushButton("Remove Selected");

  abilityList = new QListWidget();
  abilityList->setMaximumHeight(100);

  auto *abilityButtonRow = new QHBoxLayout();
  abilityButtonRow->addWidget(addAbilityBtn);
  abilityButtonRow->addWidget(removeAbilityBtn);

  actionsLayout->addWidget(new QLabel("New Ability:"));
  actionsLayout->addWidget(abilityInput);
  actionsLayout->addLayout(abilityButtonRow);
  actionsLayout->addWidget(new QLabel("Abilities:"));
  actionsLayout->addWidget(abilityList);

  connect(addAbilityBtn, &QPushButton::clicked, [this]() {
    if (!abilityInput->text().isEmpty()) {
      abilityList->addItem(abilityInput->text());
      abilityInput->clear();
    }
  });

  connect(removeAbilityBtn, &QPushButton::clicked, [this]() {
    removeSelectedListItems(abilityList);
  });

  actionInput = new QLineEdit();
  actionInput->setPlaceholderText("Enter action (e.g. Multiattack, Bite)...");
  auto *addActionBtn = new QPushButton("Add Action");
  auto *removeActionBtn = new QPushButton("Remove Selected");

  actionList = new QListWidget();
  actionList->setMaximumHeight(100);

  auto *actionButtonRow = new QHBoxLayout();
  actionButtonRow->addWidget(addActionBtn);
  actionButtonRow->addWidget(removeActionBtn);

  actionsLayout->addWidget(new QLabel("New Action:"));
  actionsLayout->addWidget(actionInput);
  actionsLayout->addLayout(actionButtonRow);
  actionsLayout->addWidget(new QLabel("Actions:"));
  actionsLayout->addWidget(actionList);

  connect(addActionBtn, &QPushButton::clicked, [this]() {
    if (!actionInput->text().isEmpty()) {
      actionList->addItem(actionInput->text());
      actionInput->clear();
    }
  });

  connect(removeActionBtn, &QPushButton::clicked, [this]() {
    removeSelectedListItems(actionList);
  });

  legendaryActionInput = new QLineEdit();
  legendaryActionInput->setPlaceholderText("Enter action (e.g. Multiattack, Bite)...");
  auto *addLegendaryActionBtn = new QPushButton("Add Legendary Action");
  auto *removeLegendaryActionBtn = new QPushButton("Remove Selected");

  legendaryActionList = new QListWidget();
  legendaryActionList->setMaximumHeight(100);

  auto *legendaryButtonRow = new QHBoxLayout();
  legendaryButtonRow->addWidget(addLegendaryActionBtn);
  legendaryButtonRow->addWidget(removeLegendaryActionBtn);

  actionsLayout->addWidget(new QLabel("New Legendary Action:"));
  actionsLayout->addWidget(legendaryActionInput);
  actionsLayout->addLayout(legendaryButtonRow);
  actionsLayout->addWidget(new QLabel("Legendary Actions:"));
  actionsLayout->addWidget(legendaryActionList);

  connect(addLegendaryActionBtn, &QPushButton::clicked, [this]() {
    if (!legendaryActionInput->text().isEmpty()) {
      legendaryActionList->addItem(legendaryActionInput->text());
      legendaryActionInput->clear();
    }
  });

  connect(removeLegendaryActionBtn, &QPushButton::clicked, [this]() {
    removeSelectedListItems(legendaryActionList);
  });

  reactionInput = new QLineEdit();
  reactionInput->setPlaceholderText("Enter reaction (e.g. Multiattack, Bite)...");
  auto *addReactionBtn = new QPushButton("Add Reaction");
  auto *removeReactionBtn = new QPushButton("Remove Selected");

  reactionList = new QListWidget();
  reactionList->setMaximumHeight(100);

  auto *reactionButtonRow = new QHBoxLayout();
  reactionButtonRow->addWidget(addReactionBtn);
  reactionButtonRow->addWidget(removeReactionBtn);

  actionsLayout->addWidget(new QLabel("New Reaction:"));
  actionsLayout->addWidget(reactionInput);
  actionsLayout->addLayout(reactionButtonRow);
  actionsLayout->addWidget(new QLabel("Reactions:"));
  actionsLayout->addWidget(reactionList);

  connect(addReactionBtn, &QPushButton::clicked, [this]() {
    if (!reactionInput->text().isEmpty()) {
      reactionList->addItem(reactionInput->text());
      reactionInput->clear();
    }
  });

  connect(removeReactionBtn, &QPushButton::clicked, [this]() {
    removeSelectedListItems(reactionList);
  });

  // Defenses tab: Damage resistances/immunities and condition immunities as checkboxes
  QStringList damageTypes = {"Acid", "Bludgeoning", "Cold", "Fire", "Force", "Lightning", "Necrotic", "Piercing", "Poison", "Psychic", "Radiant", "Slashing", "Thunder"};
  QStringList conditionTypes = {"Blinded", "Charmed", "Deafened", "Exhaustion", "Frightened", "Grappled", "Incapacitated", "Invisible", "Paralyzed", "Petrified", "Poisoned", "Prone", "Restrained", "Stunned", "Unconscious"};

  auto *defenseColumnLayout = new QHBoxLayout();
  defenseColumnLayout->setSpacing(12);

  auto *resistanceGroup = new QGroupBox("Damage Resistances");
  auto *resistanceLayout = new QVBoxLayout(resistanceGroup);
  for (const QString &type : damageTypes) {
    auto *box = new QCheckBox(type);
    dmgResistanceCheckBoxes[type] = box;
    resistanceLayout->addWidget(box);
  }
  defenseColumnLayout->addWidget(resistanceGroup, 1);

  auto *immunityGroup = new QGroupBox("Damage Immunities");
  auto *immunityLayout = new QVBoxLayout(immunityGroup);
  for (const QString &type : damageTypes) {
    auto *box = new QCheckBox(type);
    dmgImmunityCheckBoxes[type] = box;
    immunityLayout->addWidget(box);
  }
  defenseColumnLayout->addWidget(immunityGroup, 1);

  auto *conditionGroup = new QGroupBox("Condition Immunities");
  auto *conditionLayout = new QVBoxLayout(conditionGroup);
  for (const QString &type : conditionTypes) {
    auto *box = new QCheckBox(type);
    condImmunityCheckBoxes[type] = box;
    conditionLayout->addWidget(box);
  }
  defenseColumnLayout->addWidget(conditionGroup, 1);

  defensesLayout->addRow(QString(), defenseColumnLayout);

  sensesInput = new QLineEdit();
  sensesInput->setPlaceholderText("Enter Senses...");
  auto *addSensesBtn = new QPushButton("Add Senses");
  auto *removeSensesBtn = new QPushButton("Remove Selected");

  sensesList = new QListWidget();
  sensesList->setMaximumHeight(100);

  auto *sensesButtonRow = new QHBoxLayout();
  sensesButtonRow->addWidget(addSensesBtn);
  sensesButtonRow->addWidget(removeSensesBtn);

  generalLayout->addRow(new QLabel("New Senses:"), sensesInput);
  generalLayout->addRow(QString(), sensesButtonRow);
  generalLayout->addRow(new QLabel("Senses:"), sensesList);

  connect(addSensesBtn, &QPushButton::clicked, [this]() {
    addUniqueListItem(sensesList, sensesInput->text());
    sensesInput->clear();
  });

  connect(removeSensesBtn, &QPushButton::clicked, [this]() {
    removeSelectedListItems(sensesList);
  });

  languagesInput = new QLineEdit();
  languagesInput->setPlaceholderText("Enter Language...");
  auto *addLanguagesBtn = new QPushButton("Add Languages");
  auto *removeLanguagesBtn = new QPushButton("Remove Selected");

  languagesList = new QListWidget();
  languagesList->setMaximumHeight(100);

  auto *languagesButtonRow = new QHBoxLayout();
  languagesButtonRow->addWidget(addLanguagesBtn);
  languagesButtonRow->addWidget(removeLanguagesBtn);

  generalLayout->addRow(new QLabel("New Languages:"), languagesInput);
  generalLayout->addRow(QString(), languagesButtonRow);
  generalLayout->addRow(new QLabel("Languages:"), languagesList);

  connect(addLanguagesBtn, &QPushButton::clicked, [this]() {
    addUniqueListItem(languagesList, languagesInput->text());
    languagesInput->clear();
  });

  connect(removeLanguagesBtn, &QPushButton::clicked, [this]() {
    removeSelectedListItems(languagesList);
  });

	skillTable = new QTableWidget(0, 2);
	skillTable->setHorizontalHeaderLabels({"Skill", "Bonus"});
	skillTable->horizontalHeader()->setStretchLastSection(true);
	skillTable->setMaximumHeight(150);

	auto *addSkillBtn = new QPushButton("Add Skill");
	auto *removeSkillBtn = new QPushButton("Remove Selected");
	auto *skillNameInput = new QLineEdit();
	auto *skillBonusInput = new QSpinBox();

	auto *skillButtonRow = new QHBoxLayout();
	skillButtonRow->addWidget(addSkillBtn);
	skillButtonRow->addWidget(removeSkillBtn);

	defensesLayout->addRow("Skill Name:", skillNameInput);
	defensesLayout->addRow("Bonus", skillBonusInput);
	defensesLayout->addRow(QString(), skillButtonRow);
	defensesLayout->addRow("Skills:", skillTable);

	connect(addSkillBtn, &QPushButton::clicked, [this, skillNameInput, skillBonusInput]() {
		addUniqueTableRow(skillTable, skillNameInput->text(), skillBonusInput->text());
		skillNameInput->clear();
	});

	connect(removeSkillBtn, &QPushButton::clicked, [this]() {
		removeSelectedTableRows(skillTable);
	});

	savingThrowTable = new QTableWidget(0, 2);
	savingThrowTable->setHorizontalHeaderLabels({"Save", "Bonus"});
  savingThrowTable->horizontalHeader()->setStretchLastSection(true);
  savingThrowTable->setMaximumHeight(150);

  auto *addSaveBtn = new QPushButton("Add Saving Throw");
  auto *removeSaveBtn = new QPushButton("Remove Selected");
  auto *saveNameInput = new QLineEdit();
  auto *saveBonusInput = new QSpinBox();

  auto *saveButtonRow = new QHBoxLayout();
  saveButtonRow->addWidget(addSaveBtn);
  saveButtonRow->addWidget(removeSaveBtn);

  defensesLayout->addRow("Save Name:", saveNameInput);
  defensesLayout->addRow("Save Bonus:", saveBonusInput);
  defensesLayout->addRow(QString(), saveButtonRow);
  defensesLayout->addRow("Saving Throws:", savingThrowTable);

  connect(addSaveBtn, &QPushButton::clicked, [this, saveNameInput, saveBonusInput]() {
    addUniqueTableRow(savingThrowTable, saveNameInput->text(), saveBonusInput->text());
    saveNameInput->clear();
  });

  connect(removeSaveBtn, &QPushButton::clicked, [this]() {
    removeSelectedTableRows(savingThrowTable);
  });

  hpInput = new QLineEdit("20");
  currentHpInput = new QLineEdit("20");
  initiativeInput = new QLineEdit("0");
  acInput = new QLineEdit("12");
  passiveInput = new QLineEdit("10");
  crInput = new QLineEdit("0.5");

  generalLayout->addRow("Max HP:", hpInput);
  generalLayout->addRow("Current HP:", currentHpInput);
  generalLayout->addRow("Initiative:", initiativeInput);
  generalLayout->addRow("AC:", acInput);
  generalLayout->addRow("Passive Perception ", passiveInput);
  generalLayout->addRow("CR", crInput);

  auto *templateCombo = new QComboBox();
  templateCombo->addItem("Undead");
  templateCombo->addItem("Brute");
  templateCombo->addItem("Spellcaster");
  templateCombo->addItem("Skirmisher");

  auto *addTemplateBtn = new QPushButton("Apply Template");
  templateList = new QListWidget();
  templateList->setMaximumHeight(100);

  generalLayout->addRow("Template:", templateCombo);
  generalLayout->addRow(QString(), addTemplateBtn);
  generalLayout->addRow("Applied Templates:", templateList);

  connect(addTemplateBtn, &QPushButton::clicked, [this, templateCombo]() {
    const QString selectedTemplate = templateCombo->currentText();
    if (selectedTemplate.isEmpty()) {
      return;
    }

    Monster currentMonster = getMonsterFromUI();
    applyTemplate(currentMonster, selectedTemplate.toStdString());

    hpInput->setText(QString::number(currentMonster.hp));
    currentHpInput->setText(QString::number(currentMonster.currentHp));
    initiativeInput->setText(QString::number(currentMonster.initiative));
    templateList->clear();
    for (const auto& templateName : currentMonster.templates) {
      addUniqueListItem(templateList, QString::fromStdString(templateName));
    }
  });

  // Add tabs to the tab widget (only General, Actions, Defenses)
  tabWidget->addTab(generalTab, "General");
  tabWidget->addTab(actionsTab, "Actions");
  tabWidget->addTab(defensesTab, "Defenses");

  contentLayout->addWidget(tabWidget);
  layout->addWidget(sidebarWidget);
  layout->addWidget(contentWidget, 1);

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
    currentHpInput->setText(QString::number(m.currentHp));
    initiativeInput->setText(QString::number(m.initiative));
    acInput->setText(QString::number(m.ac));
    passiveInput->setText(QString::number(m.passivePerception));
    crInput->setText(QString::number(m.cr));

    abilityList->clear();
		actionList->clear();
		legendaryActionList->clear();
		reactionList->clear();
		sensesList->clear();
		languagesList->clear();
		templateList->clear();

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

		populateCheckboxGroup(m.dmgResistances, dmgResistanceCheckBoxes);
		populateCheckboxGroup(m.dmgImmunes, dmgImmunityCheckBoxes);
		populateCheckboxGroup(m.condImmunes, condImmunityCheckBoxes);

		for (const auto& legendaryAction : m.legendaryActions) {
			legendaryActionList->addItem(QString::fromStdString(legendaryAction));
		}

		for (const auto& reaction : m.reactions) {
			reactionList->addItem(QString::fromStdString(reaction));
		}

		for (const auto& sense : m.senses) {
			addUniqueListItem(sensesList, QString::fromStdString(sense));
		}

		for (const auto& language : m.languages) {
			addUniqueListItem(languagesList, QString::fromStdString(language));
		}

		for (const auto& templateName : m.templates) {
			addUniqueListItem(templateList, QString::fromStdString(templateName));
		}

		for (auto it = statInputs.begin(); it != statInputs.end(); ++it) {
			int val = m.getStat(it.key().toStdString());
			it.value()->setText(QString::number(val));
		}
  });
}

void MainWindow::populateCheckboxGroup(const std::vector<std::string>& values, QMap<QString, QCheckBox*>& checkboxes) {
  for (auto it = checkboxes.begin(); it != checkboxes.end(); ++it) {
    it.value()->setChecked(false);
  }

  for (const auto& value : values) {
    auto *box = checkboxes.value(QString::fromStdString(value), nullptr);
    if (box) {
      box->setChecked(true);
    }
  }
}

std::vector<std::string> MainWindow::collectCheckedValues(const QMap<QString, QCheckBox*>& checkboxes) const {
  std::vector<std::string> values;
  for (auto it = checkboxes.begin(); it != checkboxes.end(); ++it) {
    if (it.value() && it.value()->isChecked()) {
      values.push_back(it.key().toStdString());
    }
  }
  return values;
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

std::vector<std::string> currentDmgResistances = collectCheckedValues(dmgResistanceCheckBoxes);
  std::vector<std::string> currentDmgImmunes = collectCheckedValues(dmgImmunityCheckBoxes);
  std::vector<std::string> currentCondImmunes = collectCheckedValues(condImmunityCheckBoxes);

	std::vector<std::string> currentSenses = collectUniqueListValues(sensesList);
	std::vector<std::string> currentLanguages = collectUniqueListValues(languagesList);
	std::vector<std::string> currentTemplates = collectUniqueListValues(templateList);

  return Monster{
    nameInput->text().toStdString(),             // 1 name
    statInputs["Strength"]->text().toInt(),    // 2 strength
    statInputs["Dexterity"]->text().toInt(),   // 3 dexterity
    statInputs["Constitution"]->text().toInt(),// 4 constitution
    statInputs["Intelligence"]->text().toInt(),// 5 intelligence
    statInputs["Wisdom"]->text().toInt(),      // 6 wisdom
    statInputs["Charisma"]->text().toInt(),    // 7 charisma
    hpInput->text().toInt(),                     // 8 hp
    acInput->text().toInt(),                     // 9 ac
    currentDmgResistances,                       // 10 dmgResistances
    currentDmgImmunes,                           // 11 dmgImmunes
    currentCondImmunes,                          // 12 condImmunes
    currentSenses,                               // 13 senses
    passiveInput->text().toInt(),                // 14 passivePerception
    currentLanguages,                            // 15 languages
    crInput->text().toDouble(),                  // 16 cr
    currentAbilities,                            // 17 abilities
    currentActions,                              // 18 actions
    currentSaves,                                // 19 savingThrows
    currentSkills,                               // 20 skills
    currentLegendaryActions,                     // 21 legendaryActions
    currentReactions,                            // 22 reactions
    currentHpInput->text().toInt(),              // 23 currentHp
    initiativeInput->text().toInt(),            // 24 initiative
    currentTemplates,                            // 25 templates
    ""                                          // 26 notes
  };
}

void MainWindow::refreshMonsterList() {
  monsterList->clear(); // Clear existing to avoid duplicates
  auto monsters = manager.loadAll();
  for (const auto& m : monsters) {
    monsterList->addItem(QString::fromStdString(m.name));
  }
}
