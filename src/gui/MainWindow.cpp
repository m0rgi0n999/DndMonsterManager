#include "model/MonsterManager.hpp"
#include "model/Templates.hpp"
#include <QListWidget>
#include <QFormLayout>
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
    
  hpInput = new QLineEdit("20");
  formLayout->addRow("HP:", hpInput);

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
		for (auto it = statInputs.begin(); it != statInputs.end(); ++it) {
			int val = m.getStat(it.key().toStdString());
			it.value()->setText(QString::number(val));
		}
	});
}

Monster MainWindow::getMonsterFromUI() {
  return Monster{
    nameInput->text().toStdString(),
    statInputs["Strength"]->text().toInt(), // Added ()
    statInputs["Dexterity"]->text().toInt(),
    statInputs["Constitution"]->text().toInt(),
    statInputs["Intelligence"]->text().toInt(),
    statInputs["Wisdom"]->text().toInt(),
    statInputs["Charisma"]->text().toInt(),
    hpInput->text().toInt(),
    12, 0.5, {}
  };
}

void MainWindow::refreshMonsterList() {
  monsterList->clear(); // Clear existing to avoid duplicates
  auto monsters = manager.loadAll();
  for (const auto& m : monsters) {
    monsterList->addItem(QString::fromStdString(m.name));
  }
}
