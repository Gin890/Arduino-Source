/*  Virtual Game Console
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "Common/Qt/CollapsibleGroupBox.h"
#include "NintendoSwitch/Framework/NintendoSwitch_SwitchSystemWidget.h"
#include "NintendoSwitch_VirtualConsole.h"

namespace PokemonAutomation{
namespace NintendoSwitch{

VirtualConsole_Descriptor::VirtualConsole_Descriptor()
    : PanelDescriptor(
        Color(),
        "NintendoSwitch:VirtualConsole",
        "Nintendo Switch", "Virtual Console",
        "ComputerControl/blob/master/Wiki/Programs/NintendoSwitch/VirtualConsole.md",
        "Play your Switch from your computer. Device logging is logged to the output window."
    )
{}



VirtualConsole::VirtualConsole(const VirtualConsole_Descriptor& descriptor)
    : PanelInstance(descriptor)
    , m_switch(0, PABotBaseLevel::NOT_PABOTBASE, FeedbackType::NONE, false)
{}
void VirtualConsole::from_json(const QJsonValue& json){
    m_switch.load_json(json.toObject());
}
QJsonValue VirtualConsole::to_json() const{
    return m_switch.to_json();
}
QWidget* VirtualConsole::make_widget(QWidget& parent, PanelHolder& holder){
    return VirtualConsole_Widget::make(parent, *this, holder);
}



VirtualConsole_Widget* VirtualConsole_Widget::make(
    QWidget& parent,
    VirtualConsole& instance,
    PanelHolder& holder
){
    VirtualConsole_Widget* widget = new VirtualConsole_Widget(parent, instance, holder);
    widget->construct();
    return widget;
}
VirtualConsole_Widget::VirtualConsole_Widget(
    QWidget& parent,
    VirtualConsole& instance,
    PanelHolder& holder
)
    : PanelWidget(parent, instance, holder)
{}
void VirtualConsole_Widget::construct(){
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(make_header(*this));

    VirtualConsole& instance = static_cast<VirtualConsole&>(m_instance);
    m_switch = (SwitchSystemWidget*)instance.m_switch.make_ui(*this, m_holder.raw_logger(), 0);
    layout->addWidget(m_switch);
}






}
}

