/*  Serial Connection Selector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include <QJsonValue>
#include <QSerialPortInfo>
#include "Common/Cpp/Pimpl.tpp"
#include "SerialSelector.h"
#include "SerialSelectorWidget.h"

namespace PokemonAutomation{


SerialSelector::~SerialSelector(){}
SerialSelector::SerialSelector(
    QString label,
    PABotBaseLevel minimum_pabotbase
)
    : m_label(std::move(label))
    , m_minimum_pabotbase(minimum_pabotbase)
{}
SerialSelector::SerialSelector(
    QString label,
    PABotBaseLevel minimum_pabotbase,
    const QJsonValue& json
)
    : SerialSelector(std::move(label), minimum_pabotbase)
{
    load_json(json);
}
void SerialSelector::load_json(const QJsonValue& json){
    QString name = json.toString();
    if (name.size() > 0){
        m_port = QSerialPortInfo(name);
    }
}
QJsonValue SerialSelector::to_json() const{
    return QJsonValue(m_port->isNull() ? "" : m_port->portName());
}

const QSerialPortInfo* SerialSelector::port() const{
    if (m_port->isNull()){
        return nullptr;
    }
    return m_port.get();
}

SerialSelectorWidget* SerialSelector::make_ui(QWidget& parent, LoggerQt& logger){
    return new SerialSelectorWidget(parent, *this, logger);
}






}




