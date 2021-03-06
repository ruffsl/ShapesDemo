// Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "eprosimashapesdemo/qt/optionsdialog.h"
#include "ui_optionsdialog.h"
#include "eprosimashapesdemo/shapesdemo/ShapesDemo.h"
#include "eprosimashapesdemo/qt/mainwindow.h"

OptionsDialog::OptionsDialog(MainWindow *mw, ShapesDemo* psd, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog),
    mp_sd(psd),
    mp_mw(mw)
{
    ui->setupUi(this);
    ShapesDemoOptions opt = this->mp_sd->getOptions();
    this->ui->spin_domainId->setValue(opt.m_domainId);
    if(mp_sd->isInitialized())
        this->ui->spin_domainId->setEnabled(false);
    this->ui->spin_updateInterval->setValue(opt.m_updateIntervalMs);
    this->ui->horizontalSlider_speed->setValue(opt.m_movementSpeed);

    setEnableState();
    setAttribute ( Qt::WA_DeleteOnClose, true );

}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::setEnableState()
{
    if(this->mp_sd->isInitialized())
    {
        this->ui->pushButton_start->setEnabled(false);
        this->ui->spin_domainId->setEnabled(false);
        this->ui->pushButton_stop->setEnabled(true);
    }
    else
    {
        this->ui->pushButton_start->setEnabled(true);
        this->ui->spin_domainId->setEnabled(true);
        this->ui->pushButton_stop->setEnabled(false);
    }
}

void OptionsDialog::on_OptionsDialog_accepted()
{
    ShapesDemoOptions opt;
    opt.m_domainId = this->ui->spin_domainId->value();
    opt.m_movementSpeed = this->ui->horizontalSlider_speed->value();
    opt.m_updateIntervalMs = this->ui->spin_updateInterval->value();
    mp_sd->setOptions(opt);
}

void OptionsDialog::on_pushButton_start_clicked()
{
    this->mp_mw->on_actionStart_triggered();
   setEnableState();
}

void OptionsDialog::on_pushButton_stop_clicked()
{
    this->mp_mw->on_actionStop_triggered();
   setEnableState();
}

void OptionsDialog::on_spin_domainId_valueChanged(int arg1)
{
    ShapesDemoOptions opt;
    opt.m_domainId = arg1;
    opt.m_movementSpeed = this->ui->horizontalSlider_speed->value();
    opt.m_updateIntervalMs = this->ui->spin_updateInterval->value();
    mp_sd->setOptions(opt);
}
