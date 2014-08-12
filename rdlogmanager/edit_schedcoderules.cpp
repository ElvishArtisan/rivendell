// edit_schedcoderules.cpp
//
// Change rules for scheduler codes dialog
//
//   Stefan Gabriel <stg@st-gabriel.de>
//
//   
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <math.h>

#include <qdialog.h>
#include <qstring.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qsqldatabase.h>
#include <qspinbox.h>
#include <qcombobox.h>

#include <rd.h>

#include <edit_schedcoderules.h>
#include <schedruleslist.h>



editSchedCodeRules::editSchedCodeRules(QListViewItem *item,SchedRulesList *sched_rules_list, QWidget* parent, const char* name)
    : QDialog( parent, name, true)
{
    item_edit = item;

    if ( !name )
	setName( "editSchedCodeRules" );
    //
    // Fix the Window Size
    //
    setMinimumWidth(sizeHint().width());
    setMaximumWidth(sizeHint().width());
    setMinimumHeight(sizeHint().height());
    setMaximumHeight(sizeHint().height());
    
    setCaption(tr("Edit Rules for Code"));


    // Create Font
    QFont font=QFont("Helvetica",12,QFont::Bold);
    font.setPixelSize(12);


    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setDefault( true );
    buttonOk->setFont( font );
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( okData() ) );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
    buttonCancel->setText( tr( "&Cancel" ) );
    buttonCancel->setFont( font );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( cancelData() ) );
    
    label_code_name = new QLabel( this, "label_code_name" );
    label_code_name->setGeometry( QRect( 40, 10, 150, 20 ) );
    label_code_name->setText( tr( "Code:" ) );
    label_code_name->setFont( font ); 
    label_code_name->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    label_max_row = new QLabel( this, "label_max_row" );
    label_max_row->setGeometry( QRect( 30, 80, 160, 20 ) );
    label_max_row->setText( tr( "Max. in a row:" ) );
    label_max_row->setFont( font ); 
    label_max_row->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    label_min_wait = new QLabel( this, "label_min_wait" );
    label_min_wait->setGeometry( QRect( 30, 110, 160, 20 ) );
    label_min_wait->setText( tr( "Min. wait:" ) );
    label_min_wait->setFont( font ); 
    label_min_wait->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    label_not_after = new QLabel( this, "label_not_after" );
    label_not_after->setGeometry( QRect( 30, 140, 160, 20 ) );
    label_not_after->setText( tr( "Do not schedule after:" ) );
    label_not_after->setFont( font ); 
    label_not_after->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    label_or_after = new QLabel( this, "label_or_after" );
    label_or_after->setGeometry( QRect( 30, 180, 160, 20 ) );
    label_or_after->setText( tr( "Or after:" ) );
    label_or_after->setFont( font ); 
    label_or_after->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    label_or_after_II = new QLabel( this, "label_or_after_II" );
    label_or_after_II->setGeometry( QRect( 30, 220, 160, 20 ) );
    label_or_after_II->setText( tr( "Or after:" ) );
    label_or_after_II->setFont( font ); 
    label_or_after_II->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    spinBox_max_row = new QSpinBox( this, "spinBox_max_row" );
    spinBox_max_row->setGeometry( QRect( 200, 80, 70, 20 ) );
    spinBox_max_row->setMaxValue( 999 );
    spinBox_max_row->setLineStep( 1 );
    spinBox_max_row->setValue( item->text(1).toInt() );

    spinBox_min_wait = new QSpinBox( this, "spinBox_min_wait" );
    spinBox_min_wait->setGeometry( QRect( 200, 110, 70, 20 ) );
    spinBox_min_wait->setMaxValue( 999 );
    spinBox_max_row->setLineStep( 1 );
    spinBox_min_wait->setValue( item->text(2).toInt() );

    label_code = new QLabel( this, "label_code" );
    label_code->setGeometry( QRect( 200, 10, 90, 20 ) );
    label_code->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignLeft ) );
    label_code->setText( item->text(0) );
    
    comboBox_not_after = new QComboBox( FALSE, this, "comboBox_not_after" );
    comboBox_not_after->setGeometry( QRect( 200, 140, 180, 30 ) );
    comboBox_not_after->setDuplicatesEnabled( FALSE );

    comboBox_or_after = new QComboBox( FALSE, this, "comboBox_or_after" );
    comboBox_or_after->setGeometry( QRect( 200, 180, 180, 30 ) );
    comboBox_or_after->setDuplicatesEnabled( FALSE );

    comboBox_or_after_II = new QComboBox( FALSE, this, "comboBox_or_after_II" );
    comboBox_or_after_II->setGeometry( QRect( 200, 220, 180, 30 ) );
    comboBox_or_after_II->setDuplicatesEnabled( FALSE );

    comboBox_not_after->insertItem("");
    comboBox_or_after->insertItem("");
    comboBox_or_after_II->insertItem("");
    for (int i=0; i<sched_rules_list->getNumberOfItems(); i++)
      {
      comboBox_not_after->insertItem(sched_rules_list->getItemSchedCode(i));
      comboBox_or_after->insertItem(sched_rules_list->getItemSchedCode(i));
      comboBox_or_after_II->insertItem(sched_rules_list->getItemSchedCode(i));
      }
    comboBox_not_after->setCurrentText(item->text(3));
    comboBox_or_after->setCurrentText(item->text(4));
    comboBox_or_after->setCurrentText(item->text(5));

    label_description = new QLabel( this, "label_description" );
    label_description->setGeometry( QRect( 200, 40, 300, 40 ) );
    label_description->setAlignment( int( QLabel::AlignTop | QLabel::AlignLeft ) );
    label_description->setText( item->text(6) );
}


editSchedCodeRules::~editSchedCodeRules()
{
}

QSize editSchedCodeRules::sizeHint() const
{
  return QSize(500,350);
} 


QSizePolicy editSchedCodeRules::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}



void editSchedCodeRules::okData()
{
  item_edit->setText(1,spinBox_max_row->text());  
  item_edit->setText(2,spinBox_min_wait->text());  
  item_edit->setText(3,comboBox_not_after->currentText());  
  item_edit->setText(4,comboBox_or_after->currentText());  
  item_edit->setText(5,comboBox_or_after_II->currentText());  
  done(0);
}


void editSchedCodeRules::cancelData()
{
  done(-1);
}


void editSchedCodeRules::closeEvent(QCloseEvent *e)
{
  cancelData();
}
