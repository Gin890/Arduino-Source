/*  Multi-Host Table
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonSwSh_MultiHostTable_H
#define PokemonAutomation_PokemonSwSh_MultiHostTable_H

#include "Common/Qt/Options/EditableTable/EditableTableBaseOption.h"
#include "Common/Qt/Options/MultiHostTableBaseOption.h"
#include "CommonFramework/Options/ConfigOption.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSwSh{


class MultiHostTableOption : public ConfigOption{
public:
    MultiHostTableOption();
    virtual void load_json(const QJsonValue& json) override;
    virtual QJsonValue to_json() const override;

    size_t size() const;
    const MultiHostSlot& operator[](size_t index) const;

    virtual QString check_validity() const override;
    virtual void restore_defaults() override;

    virtual ConfigWidget* make_ui(QWidget& parent) override;

private:
    friend class MultiHostTableWidget;
    MultiHostSlotOptionFactory m_factory;
    EditableTableBaseOption m_table;
};




}
}
}
#endif
