// edit_hot_keys.cpp
//
// Edit the Hot Key Configuration for a Rivendell Workstation.
//

#include <qsignalmapper.h>
#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qsqldatabase.h>
#include <qmessagebox.h>
#include <qlistbox.h>
#include <rdhotkeylist.h>
#include <edit_hotkeys.h>
#include <globals.h>


EditHotkeys::EditHotkeys(const QString &station,const QString &module,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
    QString sql;
    RDSqlQuery *q;

    hotkey_conf=station;
    hotkey_module=module;
    station_hotkeys= new RDHotkeys(hotkey_conf,hotkey_module);
    myhotkeylist = new RDHotKeyList();
    //
    // Create Fonts
    //
    QFont normal_font=QFont("Helvetica",12,QFont::Normal);
    normal_font.setPixelSize(12);
    QFont font=QFont("Helvetica",12,QFont::Bold);
    font.setPixelSize(12);
    QFont section_font=QFont("Helvetica",14,QFont::Bold);
    section_font.setPixelSize(14);

    //
    // Fix the Window Size
    //
    setMinimumWidth(sizeHint().width());
    setMaximumWidth(sizeHint().width());
    setMinimumHeight(sizeHint().height());
    setMaximumHeight(sizeHint().height());

    //
    // Hot Key Configuration Label
    //
    setCaption(hotkey_module.upper()+" "+tr("Hot Key Configuration for")+" "+
	       hotkey_conf);

    list_view=new QListView(this,"list_view");
    list_view->setGeometry(10,24,320,220);
    QLabel *label=new QLabel(list_view,tr("Host Hot Key Configurations"),
                       this,"list_view_label");
    label->setFont(font);
    label->setGeometry(14,5,sizeHint().width()-28,19);
    //list_view->setItemMargin(5);
    list_view->setSorting(-1);
    list_view->addColumn(tr("Button / Function "));
    list_view->setColumnAlignment(0,AlignLeft|AlignVCenter);
    list_view->addColumn(tr("KeyStroke"));
    list_view->setColumnAlignment(1,AlignLeft|AlignVCenter);
    list_view->setAllColumnsShowFocus(true);
 
    connect(list_view,SIGNAL(clicked(QListViewItem *,const QPoint &,int)),
                 this,SLOT(showCurrentKey()));
    connect(list_view,SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
                 this,SLOT(showCurrentKey()));



    //  Keystroke Value field
    keystroke = new QLineEdit(this);
    keystroke->setFocusPolicy(QWidget::StrongFocus);
    keystroke->setGeometry(sizeHint().width()-270,sizeHint().height()-210,200,35);
  
  
    // Set Button
    //
    QPushButton *set_button = new QPushButton(this,"set_button");
    set_button->setGeometry(sizeHint().width()-290,sizeHint().height()-160,60,30);
    set_button->setDefault(true);
    set_button->setFont(font);
    set_button->setText(tr("Set"));
    connect(set_button,SIGNAL(clicked()), this, SLOT(SetButtonClicked()) );
  
    // Clear Button
    //
    QPushButton *clear_button = new QPushButton(this,"clear_button");
    clear_button->setGeometry(sizeHint().width()-215,sizeHint().height()-160,60,30);
    clear_button->setDefault(true);
    clear_button->setFont(font);
    clear_button->setText(tr("Clear"));
    connect(clear_button,SIGNAL(clicked()), this, SLOT(clearCurrentItem()) );
  
    // Clear All Hot Keys Button
    //
    QPushButton *clear_all_button = new QPushButton(this,"clear_all_button");
    clear_all_button->setGeometry(sizeHint().width()-140,sizeHint().height()-160,130,30);
    clear_all_button->setDefault(true);
    clear_all_button->setFont(font);
    clear_all_button->setText(tr("Clear All Hotkeys"));
    connect(clear_all_button,SIGNAL(clicked()), this,SLOT(clearAll_Hotkeys()) );
  
    // Clone Host Drop Box
    //
    clone_from_host_box=new QComboBox(this,"clone_from_host_box");
    clone_from_host_box->setGeometry(sizeHint().width()-295,sizeHint().height()-110,130,30);
    clone_from_host_label=new QLabel(clone_from_host_box,tr("Set From Host:"),
                               this,"clone_from_host_label");
    clone_from_host_label->setFont(font);
    clone_from_host_label->setGeometry(sizeHint().width()-420,sizeHint().height()-110,120,30);
    clone_from_host_label->setAlignment(AlignRight|AlignVCenter);
    sql=QString().sprintf("select NAME from STATIONS");
    q=new RDSqlQuery(sql);
    while(q->next()) {
        clone_from_host_box->insertItem(q->value(0).toString());
        if (hotkey_conf == q->value(0).toString()) {
            clone_from_host_box->setCurrentItem(clone_from_host_box->count()-1);
            current_station_clone_item = clone_from_host_box->count()-1;
        }
    }
    delete q;
    connect (clone_from_host_box,SIGNAL(activated(const QString&)),
             this,SLOT(Clone_RefreshList(const QString&))); 
 
    //
    //  Save Button
    //
    QPushButton *save_button=new QPushButton(this,"save_button");
    save_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
    save_button->setDefault(true);
    save_button->setFont(font);
    save_button->setText(tr("Save"));
    connect(save_button,SIGNAL(clicked()),this,SLOT(save()));
  
    //
    //  Cancel Button
    //
    QPushButton *cancel_button=new QPushButton(this,"cancel_button");
    cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
                               80,50);
    cancel_button->setFont(font);
    cancel_button->setText(tr("Cancel"));
  
  
    connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancel()));
  
    keystrokecount=0;
    AltKeyHit  = false;
    CtrlKeyHit = false;

  
    RefreshList();

}


EditHotkeys::~EditHotkeys()
{
}


QSize EditHotkeys::sizeHint() const
{
  return QSize(400,500);
} 


QSizePolicy EditHotkeys::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

void EditHotkeys::save()
{

    QString sql;
    RDSqlQuery *q;

    QListViewItemIterator  *start;

    start = new QListViewItemIterator(list_view);

    QString stringlist [40][45];   //  assumes no more than 40 entries...
    int cur, top, i = 0;
    while (start->current()) {
        if (start->current()->text(1).isEmpty())  {
            stringlist[i++][0] = QString("");
        }
        else  {
            stringlist[i++][0] = QString().sprintf("%s",
                  (const char *) start->current()->text(1));
        }
    ++(*start);
    }
    delete start;

    for ( top = 0; top < (i- 1) ; top++)  {
        for (cur = top + 1; cur < i; cur ++) {
            if ( (strcmp(stringlist[top][0],stringlist[cur][0]) == 0)  &&
                 (!(stringlist[top][0].isEmpty()) ) ){
                QString str = tr(QString().sprintf( \
                    "Duplicate Hotkey defined %s\n No Duplicates allowed.",
                             (const char *)stringlist[cur][0] ));
                QMessageBox::warning(this,tr("Duplicate Entries"),str);
                return;
            }
        }
    }

    start = new QListViewItemIterator(list_view);
    
    while (start->current()) {
        sql = QString().sprintf("UPDATE RDHOTKEYS  SET KEY_VALUE = \"%s\" \
               WHERE KEY_LABEL = \"%s\" AND \
               STATION_NAME = \"%s\" AND \
               MODULE_NAME = \"%s\"",
               (const char *)start->current()->text(1),              
               (const char *)start->current()->text(0),              
               (const char *)hotkey_conf,
               (const char *)hotkey_module);
        q=new RDSqlQuery(sql);
        delete q;
        ++(*start);
    }

    delete start;
    delete station_hotkeys;
    done(0);
}

void EditHotkeys::SetHotKey()
{

    QListViewItem *item=list_view->selectedItem();
    if (item==NULL) return;

    item->setText(1,hotkeystrokes);
    keyupdated = true;
}

void EditHotkeys::clearAll_Hotkeys()
{
    switch(QMessageBox::warning(this,tr("Hotkeys Clear"),
                   "Are you sure - This will Clear All Hot Key Settings!",
                         QMessageBox::Yes,QMessageBox::No)) {
        case QMessageBox::No:
        case QMessageBox::NoButton:
            return;
        default:
            break;
    }
    
    QListViewItem *l = list_view->firstChild();
    while (l)  {
        l->setText(1,"");
        l = l->nextSibling();
    }
    keystroke->setText("");
    keystroke->setFocus();
    hotkeystrokes=QString("");
    keystrokecount=0;
    keyupdated = true;
}
 


void EditHotkeys::cancel()
{
    if (keyupdated) {
        switch(QMessageBox::warning(this,tr("Hotkeys Updated"),
                       "Are you sure - All Hot Keys changes will be Lost!",
                              QMessageBox::Yes,QMessageBox::No)) {
        case QMessageBox::No:
        case QMessageBox::NoButton:
            return;
        default:
            break;
        }
    }
    close();
}

void EditHotkeys::keyPressEvent (QKeyEvent *e)
{
    if ( (e->key() == Qt::Key_Backspace) ||
         (e->key() == Qt::Key_Delete) )    {
        keystroke->setText("");
        keystroke->setFocus();
        hotkeystrokes=QString("");
        keystrokecount=0;
        return;
    }

    if (e->key() == Qt::Key_Alt)  {
        keystrokecount++;
        AltKeyHit = true;
    }

    if (e->key() == Qt::Key_Control)  {
        keystrokecount++;
        CtrlKeyHit = true;
    }

}

void EditHotkeys::keyReleaseEvent (QKeyEvent *e)
{
    int keyhit = e->key();
    QString mystring=(*myhotkeylist).GetKeyCode(keyhit);

    if (mystring.length() == 0 ) {        // should never happen unless shell got it...
        keystroke->setFocus();
        keystroke->setText("");
        hotkeystrokes=QString("");
        keystrokecount = 0;
        AltKeyHit = false;
        CtrlKeyHit = false;
        return;
    }

    if ( (e->key() == Qt::Key_Backspace) ||
         (e->key() == Qt::Key_Space) ||
         (e->key() == Qt::Key_Delete) )    {
         keystroke->setFocus();
         keystroke->setText("");
         hotkeystrokes=QString("");
         keystrokecount = 0;
         return;
    }

    if (e->key() == Qt::Key_Shift) {
        QWidget::keyReleaseEvent(e);
        return;
    }

    if ( (e->key() == Qt::Key_Up) || (e->key() == Qt::Key_Left) ||
         (e->key() == Qt::Key_Right) || (e->key() == Qt::Key_Down) )    {
        QWidget::keyReleaseEvent(e);
        keystrokecount = 0;
        hotkeystrokes = QString ("");
        keystroke->setText("");
        return;
    }

    if ( (e->key() == Qt::Key_Tab)  || (e->key() == Qt::Key_Left) ||
         (e->key() == Qt::Key_Right) ){
        QWidget::keyReleaseEvent(e);
        keystrokecount = 0;
        hotkeystrokes = QString ("");
        keystroke->setText("");
        return;
    }

    if ((e->key() == Qt::Key_Alt) ||
        (e->key() == Qt::Key_Control)) {
        if (keystrokecount != 0 ) hotkeystrokes = QString ("");
        if (AltKeyHit) {
            AltKeyHit = false;
            if (keystrokecount > 0) keystrokecount--;
        }
        if (CtrlKeyHit) {
            CtrlKeyHit = false;
            if (keystrokecount > 0) keystrokecount--;
            }
        keystroke->setText(QString().sprintf("%s",(const char *)hotkeystrokes));
        return;
    }
    if (keystrokecount > 2)  {
        keystroke->setText(QString().sprintf("%s",(const char *)hotkeystrokes));
        return;
    }

    if (!e->isAutoRepeat()) {
        if (keystrokecount == 0)
            hotkeystrokes = QString ("");
        if (AltKeyHit) {
            hotkeystrokes =  (*myhotkeylist).GetKeyCode(Qt::Key_Alt);
            hotkeystrokes +=  QString(" + ");
        }
        if (CtrlKeyHit) {
            if (AltKeyHit) {
                hotkeystrokes +=  (*myhotkeylist).GetKeyCode(Qt::Key_Control);
                hotkeystrokes += QString (" + ");
            }
            else {
                hotkeystrokes =  (*myhotkeylist).GetKeyCode(Qt::Key_Control);
                hotkeystrokes += QString (" + ");
            }
        }

        hotkeystrokes += mystring;
        keystroke->setText(QString().sprintf("%s",(const char *)hotkeystrokes));
        keystrokecount = 0;
        return;
    }
}

void EditHotkeys::RefreshList()
{

    station_hotkeys= new RDHotkeys(hotkey_conf,hotkey_module);

    QString sql;
    RDSqlQuery *q;
    QListViewItem *l;
    list_view->clear();
  
    keyupdated = false;

    //  Build Rows of List View  I do this in reverse...

    sql=QString().sprintf("select KEY_LABEL , KEY_VALUE from RDHOTKEYS \
                           where STATION_NAME = \"%s\" AND \
                           MODULE_NAME = \"%s\" \
                           ORDER BY KEY_ID DESC",
                           (const char *)hotkey_conf,
                           (const char *)hotkey_module);

    q=new RDSqlQuery(sql);
    while(q->next()) {
        l=new QListViewItem(list_view);
        l->setText(0,q->value(0).toString());
        l->setText(1,q->value(1).toString());
    }
    delete q;
}

void EditHotkeys::Clone_RefreshList(const QString& clone_station)
{
    QString sql;
    RDSqlQuery *q;

    QString tmp_hotkey_conf = QString().sprintf("%s",
                                (const char *)clone_station);
    RDHotkeys *tmp_station_hotkeys= new RDHotkeys(tmp_hotkey_conf,hotkey_module);
    keyupdated = true;
    QListViewItem *l;
    list_view->clear();

    sql=QString().sprintf("select KEY_LABEL , KEY_VALUE from RDHOTKEYS \
                           where STATION_NAME = \"%s\" AND \
                           MODULE_NAME = \"%s\" \
                           ORDER BY ID DESC",
                           (const char *)tmp_hotkey_conf,
                           (const char *)hotkey_module);

    q=new RDSqlQuery(sql);
    while(q->next()) {

        l=new QListViewItem(list_view);
        l->setText(0,q->value(0).toString());
        l->setText(1,q->value(1).toString());
    }
    delete q;
    hotkeystrokes = QString ("");
    keystroke->setText(QString().sprintf("%s",(const char *)hotkeystrokes));
    delete tmp_station_hotkeys;
}

void EditHotkeys::showCurrentKey()
{
    QListViewItem *item=list_view->selectedItem();
    if (item==NULL) return;
    keystroke->setText((const char *)item->text(1));
    keystroke->displayText();
    hotkeystrokes=QString((const char *)item->text(1));
    return;
}


void EditHotkeys::clearCurrentItem()
{
    keystrokecount=0;
    keystroke->setText("");
    keystroke->setFocus();
    hotkeystrokes=QString("");
    return;
}

void EditHotkeys::SetButtonClicked()
{

    QListViewItem *item=list_view->selectedItem();
    if (item==NULL) {
        QMessageBox::warning(this,tr("No Items Selected"),
                         tr("Please Select an Item From the List"));
        return;
    }
    SetHotKey();
    return;
}

