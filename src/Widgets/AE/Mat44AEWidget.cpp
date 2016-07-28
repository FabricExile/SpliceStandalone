#include "Mat44AEWidget.h"

#include <QHBoxLayout>
#include <QPainter>
#include <QMenu>
#include <QScrollBar>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSizePolicy>

#include "macros.h"
#include "SpliceStandalone.h"

using namespace FabricSplice;

AEWidget * Mat44AEWidget::create( FabricSplice::DGPort port , QWidget* parent)
{
	return new Mat44AEWidget(port ,parent);
}

Mat44AEWidget::Mat44AEWidget(FabricSplice::DGPort port ,QWidget* parent)
    : AEWidget(port ,parent)
{
	setLayout(createLabelControlLayout());

  m_listWidget = new QWidget();
  m_listWidget->setLayout(new QGridLayout(m_listWidget));
	layout()->addWidget(m_listWidget);

  m_validator = new QDoubleValidator(this);

  setPort(port);
			
	// QSpacerItem * spacerItem = new QSpacerItem(20,1,QSizePolicy::Expanding , QSizePolicy::Minimum);
	// list->addItem(spacerItem);

}

FabricCore::RTVal Mat44AEWidget::getValueArray()
{
  FabricCore::RTVal values;

  FABRIC_TRY_RETURN("Mat44AEWidget::getValueArray", FabricCore::RTVal(), 

  	values = rtValConstruct("Mat44");
    rtValSetArraySize(values, m_widgetsRow0X.size());

    std::vector<FabricCore::RTVal> args(16);
    for (unsigned int i = 0; i < m_widgetsRow0X.size(); ++i)
    {
      QString row0X = m_widgetsRow0X[i]->text();
      QString row0Y = m_widgetsRow0Y[i]->text();
      QString row0Z = m_widgetsRow0Z[i]->text();
      QString row0T = m_widgetsRow0T[i]->text();
      QString row1X = m_widgetsRow1X[i]->text();
      QString row1Y = m_widgetsRow1Y[i]->text();
      QString row1Z = m_widgetsRow1Z[i]->text();
      QString row1T = m_widgetsRow1T[i]->text();
      QString row2X = m_widgetsRow2X[i]->text();
      QString row2Y = m_widgetsRow2Y[i]->text();
      QString row2Z = m_widgetsRow2Z[i]->text();
      QString row2T = m_widgetsRow2T[i]->text();
      QString row3X = m_widgetsRow3X[i]->text();
      QString row3Y = m_widgetsRow3Y[i]->text();
      QString row3Z = m_widgetsRow3Z[i]->text();
      QString row3T = m_widgetsRow3T[i]->text();
      args[0] = constructFloat32RTVal(row0X.toFloat());
      args[1] = constructFloat32RTVal(row0Y.toFloat());
      args[2] = constructFloat32RTVal(row0Z.toFloat());
      args[3] = constructFloat32RTVal(row0T.toFloat());
      args[4] = constructFloat32RTVal(row1X.toFloat());
      args[5] = constructFloat32RTVal(row1Y.toFloat());
      args[6] = constructFloat32RTVal(row1Z.toFloat());
      args[7] = constructFloat32RTVal(row1T.toFloat());
      args[8] = constructFloat32RTVal(row2X.toFloat());
      args[9] = constructFloat32RTVal(row2Y.toFloat());
      args[10] = constructFloat32RTVal(row2Z.toFloat());
      args[11] = constructFloat32RTVal(row2T.toFloat());
      args[12] = constructFloat32RTVal(row3X.toFloat());
      args[13] = constructFloat32RTVal(row3Y.toFloat());
      args[14] = constructFloat32RTVal(row3Z.toFloat());
      args[15] = constructFloat32RTVal(row3T.toFloat());
      rtValSetArrayElement(values, i, constructRTVal("Mat44", 16, &args[0]));
    }
  );
	return values;
}

void Mat44AEWidget::setPort(FabricSplice::DGPort port)
{
  FABRIC_TRY("Mat44AEWidget::setPort",
  
    AEWidget::setPort(port);
    setValueArray(port.getRTVal());

  );
}

void Mat44AEWidget::setValueArray(FabricCore::RTVal values)
{
  FABRIC_TRY("Mat44AEWidget::setValueArray", 

    unsigned int precision = 3;

    // clear the layout
    if(rtValGetArraySize(values) != m_widgetsRow0X.size())
    {
      QLayoutItem* item;
      while ( ( item = m_listWidget->layout()->takeAt( 0 ) ) != NULL )
      {
        delete item->widget();
        delete item;
      }

      m_validator->setDecimals(precision);
      m_validator->setNotation(QDoubleValidator::StandardNotation);

      QGridLayout * layout = (QGridLayout*)m_listWidget->layout();

      m_widgetsRow0X.resize(rtValGetArraySize(values));
      m_widgetsRow0Y.resize(rtValGetArraySize(values));
      m_widgetsRow0Z.resize(rtValGetArraySize(values));
      m_widgetsRow0T.resize(rtValGetArraySize(values));
      m_widgetsRow1X.resize(rtValGetArraySize(values));
      m_widgetsRow1Y.resize(rtValGetArraySize(values));
      m_widgetsRow1Z.resize(rtValGetArraySize(values));
      m_widgetsRow1T.resize(rtValGetArraySize(values));
      m_widgetsRow2X.resize(rtValGetArraySize(values));
      m_widgetsRow2Y.resize(rtValGetArraySize(values));
      m_widgetsRow2Z.resize(rtValGetArraySize(values));
      m_widgetsRow2T.resize(rtValGetArraySize(values));
      m_widgetsRow3X.resize(rtValGetArraySize(values));
      m_widgetsRow3Y.resize(rtValGetArraySize(values));
      m_widgetsRow3T.resize(rtValGetArraySize(values));
      m_widgetsRow3Z.resize(rtValGetArraySize(values));
      for (unsigned int i = 0; i < rtValGetArraySize(values); ++i)
      {
        m_widgetsRow0X[i] = new ItemWidget(m_listWidget);
        m_widgetsRow0X[i]->setFont( getApplicationWidgetFont() );
        m_widgetsRow0X[i]->setValidator(m_validator);
        m_widgetsRow0Y[i] = new ItemWidget(m_listWidget);
        m_widgetsRow0Y[i]->setFont( getApplicationWidgetFont() );
        m_widgetsRow0Y[i]->setValidator(m_validator);
        m_widgetsRow0Z[i] = new ItemWidget(m_listWidget);
        m_widgetsRow0Z[i]->setFont( getApplicationWidgetFont() );
        m_widgetsRow0Z[i]->setValidator(m_validator);
        m_widgetsRow0T[i] = new ItemWidget(m_listWidget);
        m_widgetsRow0T[i]->setFont( getApplicationWidgetFont() );
        m_widgetsRow0T[i]->setValidator(m_validator);
        m_widgetsRow1X[i] = new ItemWidget(m_listWidget);
        m_widgetsRow1X[i]->setFont( getApplicationWidgetFont() );
        m_widgetsRow1X[i]->setValidator(m_validator);
        m_widgetsRow1Y[i] = new ItemWidget(m_listWidget);
        m_widgetsRow1Y[i]->setFont( getApplicationWidgetFont() );
        m_widgetsRow1Y[i]->setValidator(m_validator);
        m_widgetsRow1Z[i] = new ItemWidget(m_listWidget);
        m_widgetsRow1Z[i]->setFont( getApplicationWidgetFont() );
        m_widgetsRow1Z[i]->setValidator(m_validator);
        m_widgetsRow1T[i] = new ItemWidget(m_listWidget);
        m_widgetsRow1T[i]->setFont( getApplicationWidgetFont() );
        m_widgetsRow1T[i]->setValidator(m_validator);
        m_widgetsRow2X[i] = new ItemWidget(m_listWidget);
        m_widgetsRow2X[i]->setFont( getApplicationWidgetFont() );
        m_widgetsRow2X[i]->setValidator(m_validator);
        m_widgetsRow2Y[i] = new ItemWidget(m_listWidget);
        m_widgetsRow2Y[i]->setFont( getApplicationWidgetFont() );
        m_widgetsRow2Y[i]->setValidator(m_validator);
        m_widgetsRow2Z[i] = new ItemWidget(m_listWidget);
        m_widgetsRow2Z[i]->setFont( getApplicationWidgetFont() );
        m_widgetsRow2Z[i]->setValidator(m_validator);
        m_widgetsRow2T[i] = new ItemWidget(m_listWidget);
        m_widgetsRow2T[i]->setFont( getApplicationWidgetFont() );
        m_widgetsRow2T[i]->setValidator(m_validator);
        m_widgetsRow3X[i] = new ItemWidget(m_listWidget);
        m_widgetsRow3X[i]->setFont( getApplicationWidgetFont() );
        m_widgetsRow3X[i]->setValidator(m_validator);
        m_widgetsRow3Y[i] = new ItemWidget(m_listWidget);
        m_widgetsRow3Y[i]->setFont( getApplicationWidgetFont() );
        m_widgetsRow3Y[i]->setValidator(m_validator);
        m_widgetsRow3Z[i] = new ItemWidget(m_listWidget);
        m_widgetsRow3Z[i]->setFont( getApplicationWidgetFont() );
        m_widgetsRow3Z[i]->setValidator(m_validator);
        m_widgetsRow3T[i] = new ItemWidget(m_listWidget);
        m_widgetsRow3T[i]->setFont( getApplicationWidgetFont() );
        m_widgetsRow3T[i]->setValidator(m_validator);
    
        // connect the color box to uiChanged()
        connect( m_widgetsRow0X[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsRow0Y[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsRow0Z[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsRow0T[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsRow1X[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsRow1Y[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsRow1Z[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsRow1T[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsRow2X[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsRow2Y[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsRow2Z[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsRow2T[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsRow3X[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsRow3Y[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsRow3Z[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsRow3T[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );

        layout->addWidget( m_widgetsRow0X[i], i * 4 + 0, 0);
        layout->addWidget( m_widgetsRow0Y[i], i * 4 + 0, 1);
        layout->addWidget( m_widgetsRow0Z[i], i * 4 + 0, 2);
        layout->addWidget( m_widgetsRow0T[i], i * 4 + 0, 3);
        layout->addWidget( m_widgetsRow1X[i], i * 4 + 1, 0);
        layout->addWidget( m_widgetsRow1Y[i], i * 4 + 1, 1);
        layout->addWidget( m_widgetsRow1Z[i], i * 4 + 1, 2);
        layout->addWidget( m_widgetsRow1T[i], i * 4 + 1, 3);
        layout->addWidget( m_widgetsRow2X[i], i * 4 + 2, 0);
        layout->addWidget( m_widgetsRow2Y[i], i * 4 + 2, 1);
        layout->addWidget( m_widgetsRow2Z[i], i * 4 + 2, 2);
        layout->addWidget( m_widgetsRow2T[i], i * 4 + 2, 3);
        layout->addWidget( m_widgetsRow3X[i], i * 4 + 3, 0);
        layout->addWidget( m_widgetsRow3Y[i], i * 4 + 3, 1);
        layout->addWidget( m_widgetsRow3Z[i], i * 4 + 3, 2);
        layout->addWidget( m_widgetsRow3T[i], i * 4 + 3, 3);
      }
    }

    for (unsigned int i = 0; i < rtValGetArraySize(values); ++i)
    {
      FabricCore::RTVal value = rtValGetArrayElement(values, i);
      FabricCore::RTVal row0 = value.maybeGetMember("row0");
      FabricCore::RTVal row1 = value.maybeGetMember("row1");
      FabricCore::RTVal row2 = value.maybeGetMember("row2");
      FabricCore::RTVal row3 = value.maybeGetMember("row3");
      m_widgetsRow0X[i]->setText(QString::number(row0.maybeGetMember("x").getFloat32(), 'g', precision));
      m_widgetsRow0Y[i]->setText(QString::number(row0.maybeGetMember("y").getFloat32(), 'g', precision));
      m_widgetsRow0Z[i]->setText(QString::number(row0.maybeGetMember("z").getFloat32(), 'g', precision));
      m_widgetsRow0T[i]->setText(QString::number(row0.maybeGetMember("t").getFloat32(), 'g', precision));
      m_widgetsRow1X[i]->setText(QString::number(row1.maybeGetMember("x").getFloat32(), 'g', precision));
      m_widgetsRow1Y[i]->setText(QString::number(row1.maybeGetMember("y").getFloat32(), 'g', precision));
      m_widgetsRow1Z[i]->setText(QString::number(row1.maybeGetMember("z").getFloat32(), 'g', precision));
      m_widgetsRow1T[i]->setText(QString::number(row1.maybeGetMember("t").getFloat32(), 'g', precision));
      m_widgetsRow2X[i]->setText(QString::number(row2.maybeGetMember("x").getFloat32(), 'g', precision));
      m_widgetsRow2Y[i]->setText(QString::number(row2.maybeGetMember("y").getFloat32(), 'g', precision));
      m_widgetsRow2Z[i]->setText(QString::number(row2.maybeGetMember("z").getFloat32(), 'g', precision));
      m_widgetsRow2T[i]->setText(QString::number(row2.maybeGetMember("t").getFloat32(), 'g', precision));
      m_widgetsRow3X[i]->setText(QString::number(row3.maybeGetMember("x").getFloat32(), 'g', precision));
      m_widgetsRow3Y[i]->setText(QString::number(row3.maybeGetMember("y").getFloat32(), 'g', precision));
      m_widgetsRow3Z[i]->setText(QString::number(row3.maybeGetMember("z").getFloat32(), 'g', precision));
      m_widgetsRow3T[i]->setText(QString::number(row3.maybeGetMember("t").getFloat32(), 'g', precision));
    }

  );
}

void Mat44AEWidget::uiChanged()
{
  FABRIC_TRY("Mat44AEWidget::uiChanged", 

    FabricCore::RTVal values = getValueArray();
    m_port.setRTVal(values);

  );

  AEWidget::uiChanged();
}
