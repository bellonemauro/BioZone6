﻿/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB                                                              |
*  |  Copyright 2021 © Fluicell AB, http://fluicell.com/                       |
*  |  BioZone6                                                                 |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include <QtWidgets>
#include <QTextStream>
#include "protocolEditorDelegates.h"
#include "XmlProtocolWriter.h"


XmlProtocolWriter::XmlProtocolWriter(const QTreeWidget *treeWidget)
    : treeWidget(treeWidget)
{
    xml.setAutoFormatting(true);
}

bool XmlProtocolWriter::writeFile(QIODevice *device)
{
    xml.setDevice(device);

    xml.writeStartDocument();
    xml.writeDTD(QStringLiteral("<!DOCTYPE prt>"));
    xml.writeComment("Fluicell xml protocol file for BioZone 6 - https://www.fluicell.com/");
    QString toPrint = "File created on " + QDate::currentDate().toString() + "  "
        + QTime::currentTime().toString() + " ";
    xml.writeComment(toPrint);
    xml.writeStartElement(swProtocolAttribute());
    xml.writeAttribute(versionAttribute(), QStringLiteral("1.1"));
    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i)
        writeItem(treeWidget->topLevelItem(i));

    xml.writeEndDocument();
    return true;
}

void XmlProtocolWriter::writeItem(const QTreeWidgetItem *item)
{
	xml.writeStartElement(getCommandAsString(item->text(1).toInt()));
	xml.writeAttribute(valueAttribute(), item->text(3)); 
	xml.writeAttribute(messageAttribute(), item->text(4));
	for (int i = 0; i < item->childCount(); ++i)
		writeItem(item->child(i));
	xml.writeEndElement();
}

QString XmlProtocolWriter::getCommandAsString(int _instruction)
{
	ComboBoxDelegate* tmp_combo = new ComboBoxDelegate();
	QString text = QString::fromStdString(tmp_combo->getElementAt(_instruction));
	return text;
}
