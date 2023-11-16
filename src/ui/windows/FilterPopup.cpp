/*
 * Marvelous OTF2 Traces Interactive Visualizer (MOTIV)
 * Copyright (C) 2023 Florian Gallrein, Björn Gehrke, Jessica Lafontaine
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <QGridLayout>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include "FilterPopup.hpp"

FilterPopup::FilterPopup(const Filter &filter, QWidget *parent, const Qt::WindowFlags &f)
    : QDialog(parent, f), filter_(filter) {

    auto *grid = new QGridLayout();

    // Show checkboxes for different slot kinds
    auto slotKindsGroupBox = new QGroupBox(tr("Function calls"));
    mpiSlotKindCheckBox = new QCheckBox(tr("Show &MPI function calls"));
    mpiSlotKindCheckBox->setChecked(filter_.getSlotKinds() & SlotKind::MPI);
    openMpSlotKindCheckBox = new QCheckBox(tr("Show &OpenMp function calls"));
    openMpSlotKindCheckBox->setChecked(filter_.getSlotKinds() & SlotKind::OpenMP);
    plainSlotKindCheckBox = new QCheckBox(tr("Show &plain function calls"));
    plainSlotKindCheckBox->setChecked(filter_.getSlotKinds() & SlotKind::Plain);

    // Show checkboxes for different MPI-Communication kinds
    auto pointToPointCommunicationKindsGroupBox = new QGroupBox(tr("Point to Point Communication"));
    pointToPointCheckBox = new QCheckBox(tr("Show &point to point communication"));
    pointToPointCheckBox->setChecked((filter_.getCommunicationKinds() & CommunicationKind::PointToPoint) == CommunicationKind::PointToPoint);

    blockingPointToPointCheckBox = new QCheckBox(tr("Show &blocking point to point communication"));
    blockingPointToPointCheckBox->setDisabled(pointToPointCheckBox->isChecked());
    blockingPointToPointCheckBox->setChecked((filter_.getCommunicationKinds() & CommunicationKind::BlockingPointToPoint) == CommunicationKind::BlockingPointToPoint);

    nonBlockingPointToPointCheckBox = new QCheckBox(tr("Show &non-blocking point to point communication"));
    nonBlockingPointToPointCheckBox->setDisabled(pointToPointCheckBox->isChecked());
    nonBlockingPointToPointCheckBox->setChecked((filter_.getCommunicationKinds() & CommunicationKind::NonBlockingPointToPoint) == CommunicationKind::NonBlockingPointToPoint);
   
    connect(pointToPointCheckBox, &QCheckBox::toggled, [this] {
        if(pointToPointCheckBox->isChecked()){
            blockingPointToPointCheckBox->setChecked(pointToPointCheckBox->isChecked());
            blockingPointToPointCheckBox->setDisabled(pointToPointCheckBox->isChecked());

            nonBlockingPointToPointCheckBox->setChecked(pointToPointCheckBox->isChecked());
            nonBlockingPointToPointCheckBox->setDisabled(pointToPointCheckBox->isChecked());

        }else{
            blockingPointToPointCheckBox->setDisabled(false);
            nonBlockingPointToPointCheckBox->setDisabled(false);
        }
    });


    auto collectivCommunicationKindsGroupBox = new QGroupBox(tr("Collectiv Communication"));
    collectiveCheckBox = new QCheckBox(tr("Show &collective communication"));
    collectiveCheckBox->setChecked((filter_.getCommunicationKinds() & CommunicationKind::Collective)==CommunicationKind::Collective);
    
    synchoCheckBox = new QCheckBox(tr("Show &synchronizing communication"));
    synchoCheckBox->setDisabled(collectiveCheckBox->isChecked());
    synchoCheckBox->setChecked((filter_.getCommunicationKinds() & CommunicationKind::Synchronizing)==CommunicationKind::Synchronizing);
    
    connect(collectiveCheckBox, &QCheckBox::toggled, [this] {
        if(collectiveCheckBox->isChecked()){
            synchoCheckBox->setChecked(collectiveCheckBox->isChecked());
            synchoCheckBox->setDisabled(collectiveCheckBox->isChecked());
        }else synchoCheckBox->setDisabled(false);
    });

    // Slot layout
    auto vboxSlot = new QVBoxLayout();
    vboxSlot->addWidget(mpiSlotKindCheckBox);
    vboxSlot->addWidget(openMpSlotKindCheckBox);
    vboxSlot->addWidget(plainSlotKindCheckBox);

    vboxSlot->addStretch(1);
    slotKindsGroupBox->setLayout(vboxSlot);

    // Communication layouts
    auto vboxPointToPointCommunication = new QVBoxLayout();
    vboxPointToPointCommunication->addWidget(pointToPointCheckBox);
    vboxPointToPointCommunication->addWidget(blockingPointToPointCheckBox);
    vboxPointToPointCommunication->addWidget(nonBlockingPointToPointCheckBox);
    pointToPointCommunicationKindsGroupBox->setLayout(vboxPointToPointCommunication);

    auto vboxCollectivCommunication = new QVBoxLayout();
    vboxCollectivCommunication->addWidget(collectiveCheckBox);
    vboxCollectivCommunication->addWidget(synchoCheckBox);
    collectivCommunicationKindsGroupBox->setLayout(vboxCollectivCommunication);


    // MPI GroupBox to hold both communication layouts
    auto mpiGroupBox = new QGroupBox(tr("MPI Communication"));
    auto hboxMPI = new QHBoxLayout();
    hboxMPI->addWidget(pointToPointCommunicationKindsGroupBox);
    hboxMPI->addWidget(collectivCommunicationKindsGroupBox);
    mpiGroupBox->setLayout(hboxMPI);
    


    grid->addWidget(slotKindsGroupBox, 0, 0, 1, 2);
    grid->addWidget(mpiGroupBox, 1, 0, 1, 2);


    auto okButton = new QPushButton(tr("&Ok"));
    okButton->setDefault(true);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    auto cancelButton = new QPushButton(tr("&Cancel"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    grid->addWidget(cancelButton, 2, 0, Qt::AlignLeft);
    grid->addWidget(okButton, 2, 1, Qt::AlignRight);


    connect(this, SIGNAL(accepted()), this, SLOT(updateFilter()));

    setLayout(grid);
    setWindowTitle(tr("Set filter"));
    setModal(true);
}

void FilterPopup::updateFilter() {
    auto slotKinds = static_cast<SlotKind>(
        SlotKind::Plain * plainSlotKindCheckBox->isChecked() |
        SlotKind::OpenMP * openMpSlotKindCheckBox->isChecked() |
        SlotKind::MPI * mpiSlotKindCheckBox->isChecked());

    filter_.setSlotKinds(slotKinds);

    auto communicationKinds = static_cast<CommunicationKind>(
        CommunicationKind::PointToPoint * pointToPointCheckBox->isChecked() | 
        CommunicationKind::BlockingPointToPoint * blockingPointToPointCheckBox->isChecked() |
        CommunicationKind::NonBlockingPointToPoint * nonBlockingPointToPointCheckBox->isChecked() |
        CommunicationKind::Synchronizing * synchoCheckBox->isChecked() |
        CommunicationKind::Collective * collectiveCheckBox->isChecked());

    filter_.setCommunicationKind(communicationKinds);


    Q_EMIT filterChanged(filter_);
}
