/*
 * Marvelous OTF2 Traces Interactive Visualizer (MOTIV)
 * Copyright (C) 2023 Florian Gallrein, Björn Gehrke, Tomas Cirkov
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
#include "Timeline.hpp"
#include "src/models/ViewSettings.hpp"
#include "src/ui/TimeUnit.hpp"
#include "src/ui/TraceDataProxy.hpp"
#include "src/ui/views/TimelineView.hpp"
#include "src/ui/widgets/TimelineHeader.hpp"
#include "src/ui/windows/FlamegraphPopup.hpp"
#include "src/ui/ScrollSynchronizer.hpp"

#include <qaction.h>
#include <QGridLayout>
#include <QVBoxLayout>
#include <qnamespace.h>
#include <qtmetamacros.h>
#include <cmath>
#include <string>



Timeline::Timeline(TraceDataProxy *data, QWidget *parent) : QWidget(parent), data(data) {
    this->data->triggerUITimerStartIfPossible();
    auto layout = new QGridLayout(this);

    // this->header = new TimelineHeader(this->data, this);
    // layout->addWidget(this->header, 0, 1);

    this->labelList = new TimelineLabelList(this->data, this);
    layout->addWidget(this->labelList, 1, 0);
    
    connect(this->data, SIGNAL(flamegraphRequest()), this, SLOT(showFlamegraphPopup()));

    // We don't want to have the context menu for layout elements in the label region
    this->labelList->setContextMenuPolicy(Qt::PreventContextMenu);

    this->prepareSlidersBoxLayouts();
    this->addSliderTicks();
    layout->addWidget(this->slidersBox, 2, 0, 1, 2);

    // Make the Box hideable via shortcut (the relevant action is located in MainWindow)
    connect(this->data, &TraceDataProxy::hideSlidersBoxRequest, this, &Timeline::hideSliderBox);
    // It's not enough to update the tooltips alongside slider changes, if the selection changes we have to consider new time values
    connect(this->data, &TraceDataProxy::selectionChanged, this, &Timeline::updateAllTooltips);

    this->view = new TimelineView(this->data, this);
    QBrush backgroundPattern = QBrush(QColorConstants::Svg::silver, Qt::Dense7Pattern);
    this->view->setBackgroundBrush(backgroundPattern);
    layout->addWidget(this->view, 1, 1);

    // We don't need more space than the max width of the contents (32+16 pixel for the +/- icons)
    this->labelList->setMaximumWidth(this->labelList->getMaxLabelLength()+48);
    // "MPI rank 5" -> "... rank 5" makes more sense than "MPI rank ..."
    this->labelList->setTextElideMode (Qt::ElideLeft);

    auto scrollSyncer = new ScrollSynchronizer(this);
    scrollSyncer->addWidget(this->labelList);
    scrollSyncer->addWidget(this->view);

}

void Timeline::showFlamegraphPopup(){
    FlamegraphPopup* flamegraph = new FlamegraphPopup(this->data, this);
    flamegraph->openFlamegraphWindow();
}

QHBoxLayout* Timeline::prepareSlider(QSlider* sliderObj, QString Name = ""){

    QHBoxLayout* sliderLabelBox = new QHBoxLayout();

    if(Name!=""){
        QLabel* sliderLabel = new QLabel(Name, this);
        QFont font;
        font.setPointSize(8);
        sliderLabel->setFont(font);
        sliderLabel->setFixedWidth(24);
        sliderLabel->setFixedHeight(32);
        sliderLabel->setToolTip(QString("Active threshold slider for %1").arg(this->giveElementDescr(sliderObj->objectName())));
        sliderObj->setRange(0, 1000);
        sliderObj->setValue(0);
        sliderObj->setFixedHeight(32);
        sliderObj->setToolTip("No active threshold");
        // We don't want to rush a user to read possibly complex tooltips: 600*10^3 msec => 10 min 
        sliderObj->setToolTipDuration(600000);

        sliderLabelBox->addWidget(sliderLabel);
        sliderLabelBox->addWidget(sliderObj);
    } else {
        if(this->modeLabel==nullptr)this->modeLabel = new QLabel("Mode:\nperc", this);
        if(this->modeIntensitySlider==nullptr) this->modeIntensitySlider = new QSlider(Qt::Vertical, this);
        connect(this->modeIntensitySlider, &QSlider::valueChanged, this, [this]() {
            this->changeModeEvent();
        });
        this->modeIntensitySlider->setDisabled(true);
        QFont font;
        font.setPointSize(8);
        this->modeLabel->setFont(font);
        this->modeLabel->setToolTipDuration(600000);
        sliderObj->setRange(0, 2);
        sliderObj->setValue(0);
        sliderObj->setToolTip("Mode control");
        this->modeIntensitySlider->setRange(0, 6);
        this->modeIntensitySlider->setToolTip("Intensity control\ne.g. higher value means even slower growth in slow mode");
        this->modeLabel->setFixedWidth(this->labelList->width()-32);


        sliderLabelBox->addWidget(sliderObj);
        sliderLabelBox->addSpacing(4);
        sliderLabelBox->addWidget(this->modeIntensitySlider);
        sliderLabelBox->addSpacing(4);
        sliderLabelBox->addWidget(this->modeLabel);
    }

    return sliderLabelBox;
}

void Timeline::addSliderTicks(){
    this->thresholdSliderOV->setTickPosition(QSlider::TicksAbove);
    this->thresholdSliderOV->setTickInterval(100);

    this->thresholdSliderP2P->setTickPosition(QSlider::TicksAbove);
    this->thresholdSliderP2P->setTickInterval(100);

    this->thresholdSliderREG->setTickPosition(QSlider::TicksBelow);
    this->thresholdSliderREG->setTickInterval(100);

    this->thresholdSliderCCM->setTickPosition(QSlider::TicksBelow);
    this->thresholdSliderCCM->setTickInterval(100);
}

void Timeline::changeModeEvent(){
    switch(this->modeSlider->value()){
        case Mode::perc: 
            this->modeLabel->setText("Mode:\nperc");
            this->modeLabel->setToolTip("f(x) = x");
            this->modeIntensitySlider->setDisabled(true);
            this->modeIntensitySlider->setValue(0);
            break;
        case Mode::slow:
            this->modeLabel->setText("Mode:\nslow");
            this->modeLabel->setToolTip(QString("f(x) = ((1 + 6931669/10^(9+%1))^(x*(x/1000)*10^%1) - 1) * ((x/100)^%1/10^%1)").arg(this->modeIntensitySlider->value()));
            this->modeIntensitySlider->setDisabled(false);
            break;
        case Mode::fast:
            this->modeLabel->setText("Mode:\nfast");
            this->modeLabel->setToolTip(QString("f(x) = 1000 * (x^(%1+1))/(x^(%1+1)+10)").arg(this->modeIntensitySlider->value()));
            this->modeIntensitySlider->setDisabled(false);
            break;
    }
    if(!ViewSettings::getInstance()->getUseREGSliderForOV())this->changeOverviewEvent();
    this->changeMainviewEvent();
}

QString Timeline::giveElementDescr(QString compactName){
    if(compactName=="OV") return "regions/functions in overview";
    if(compactName=="REG") return "regions/functions";
    if(compactName=="P2P") return "point-to-point communications";
    if(compactName=="CCM") return "collective communications";
    return "unkown";
}

double Timeline::scaleSliderValue(int trueVal){
    int intensityValue = this->modeIntensitySlider->value();
    double scaledVal, base, exponent, factor;
    switch(this->modeSlider->value()){
        case Mode::perc: 
            return (double) trueVal;
        case Mode::slow:
            // This is only needed because of the -1 in the final scaledVal calc, which caps the value at 999 rather than 1000
            if(trueVal==1000) {
                scaledVal=trueVal;
            } else {
                // This growth-factor was chosen because of 1.0069...^1000 approx 1000
                base = (1 + 6931669/(pow(10, 9+intensityValue)));
                exponent = trueVal * pow(10, intensityValue) * trueVal/1000.0;
                factor = pow(trueVal/100.0, intensityValue)/pow(10, intensityValue);
                scaledVal = (pow(base, exponent) - 1) * factor;
            }
            return scaledVal;
        case Mode::fast:
            if(trueVal==1000) {
                scaledVal=trueVal;
            } else {
                scaledVal = 1000.0 * std::pow(trueVal, intensityValue+1) / (std::pow(trueVal, intensityValue+1) + 10);
            }
            return scaledVal;
    }
    return 0;
}

void Timeline::changeOverviewEvent(){
    auto settings = ViewSettings::getInstance();
    settings->setActiveThresholdOV(this->scaleSliderValue(this->thresholdSliderOV->value()));
    auto fullTime = this->data->getTotalRuntime().count();
    this->updateTooltip(this->thresholdSliderOV, settings->getActiveThresholdOV(), fullTime);
    Q_EMIT data->refreshOverviewRequest();
}

void Timeline::changeMainviewEvent(){
    auto settings = ViewSettings::getInstance();
    settings->setActiveThresholdREG(this->scaleSliderValue(this->thresholdSliderREG->value()));   
    settings->setActiveThresholdP2P(this->scaleSliderValue(this->thresholdSliderP2P->value()));
    settings->setActiveThresholdCCM(this->scaleSliderValue(this->thresholdSliderCCM->value()));
    this->updateAllTooltips();
    this->view->updateView();
    if(settings->getUseREGSliderForOV()){
        this->thresholdSliderOV->setToolTip("currently not used!");
        Q_EMIT data->refreshOverviewRequest();
    } 
}

void Timeline::updateAllTooltips(){
    auto settings = ViewSettings::getInstance();
    auto fullTime = this->data->getSelection()->getDuration().count();
    // The OV-update is handled in Timeline::changeOverviewEvent
    this->updateTooltip(this->thresholdSliderREG, settings->getActiveThresholdREG(), fullTime);
    this->updateTooltip(this->thresholdSliderP2P, settings->getActiveThresholdP2P(), fullTime);
    this->updateTooltip(this->thresholdSliderCCM, settings->getActiveThresholdCCM(), fullTime);
}

void Timeline::updateTooltip(QSlider* sliderObj, double currentScaledValue, long fullTime){

    QString relativeToWhat = "current selection size";
    if(sliderObj->objectName()=="OV") relativeToWhat = "full trace size";

    if(sliderObj->value()){
        QString activeFilterMsg = QString("Active threshold:\n%1 < %2 [ns] are ignored!\nwhich is %3\% of the %4 %5 [ns]").arg(this->giveElementDescr(sliderObj->objectName())).arg(fullTime*currentScaledValue/1000.0).arg(currentScaledValue/10.0).arg(relativeToWhat).arg((double)fullTime);
        QString activeFilterMsgFull = QString("Active threshold:\n%1 < %4 %5 [ns] are ignored!").arg(this->giveElementDescr(sliderObj->objectName())).arg(relativeToWhat).arg((double)fullTime);
        sliderObj->value() < 1000 ? sliderObj->setToolTip(activeFilterMsg) : sliderObj->setToolTip(activeFilterMsgFull);
    } else {
        sliderObj->setToolTip("No active threshold");
    }
}

void Timeline::prepareSlidersBoxLayouts(){
    this->modeSlider = new QSlider(Qt::Vertical, this);
    this->modeSlider->setObjectName("");
    connect(this->modeSlider, &QSlider::valueChanged, this, [this]() {
        this->changeModeEvent();
    });


    this->thresholdSliderOV = new QSlider(Qt::Horizontal, this);
    this->thresholdSliderOV->setObjectName("OV");
    connect(this->thresholdSliderOV, &QSlider::valueChanged, this, [this]() {
        this->changeOverviewEvent();
    });

    this->thresholdSliderREG = new QSlider(Qt::Horizontal, this);
    this->thresholdSliderREG->setObjectName("REG");
        connect(this->thresholdSliderREG, &QSlider::valueChanged, this, [this]() {
        this->changeMainviewEvent();
    });

    this->thresholdSliderP2P = new QSlider(Qt::Horizontal, this);
    this->thresholdSliderP2P->setObjectName("P2P");
        connect(this->thresholdSliderP2P, &QSlider::valueChanged, this, [this]() {
        this->changeMainviewEvent();
    });
    this->thresholdSliderCCM = new QSlider(Qt::Horizontal, this);
    this->thresholdSliderCCM->setObjectName("CCM");
        connect(this->thresholdSliderCCM, &QSlider::valueChanged, this, [this]() {
        this->changeMainviewEvent();
    });
    
    this->slidersBox = new QGroupBox();
    auto slidersLayoutREG = new QVBoxLayout();
    slidersLayoutREG->addLayout(this->prepareSlider(this->thresholdSliderOV, this->thresholdSliderOV->objectName()));
    slidersLayoutREG->addLayout(this->prepareSlider(this->thresholdSliderREG, this->thresholdSliderREG->objectName()));
    auto slidersLayoutCOM = new QVBoxLayout();
    slidersLayoutCOM->addLayout(this->prepareSlider(this->thresholdSliderP2P, this->thresholdSliderP2P->objectName()));
    slidersLayoutCOM->addLayout(this->prepareSlider(this->thresholdSliderCCM, this->thresholdSliderCCM->objectName()));
    QHBoxLayout* slidersLayout = new QHBoxLayout();
    slidersLayout->addLayout(this->prepareSlider(this->modeSlider, this->modeSlider->objectName()));
    slidersLayout->addLayout(slidersLayoutREG);
    slidersLayout->addLayout(slidersLayoutCOM);
    this->slidersBox->setLayout(slidersLayout);
    this->slidersBox->setMaximumHeight(86);
}

void Timeline::hideSliderBox(){
    this->slidersBox->isHidden() ? this->slidersBox->setHidden(false) : this->slidersBox->setHidden(true);
}