/*  Max Lair Consoles
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "Common/Compiler.h"
#include "CommonFramework/Globals.h"
#include "CommonFramework/Options/EnumDropdownWidget.h"
#include "Pokemon/Inference/Pokemon_NameReader.h"
#include "PokemonSwSh_MaxLair_Options_Consoles.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSwSh{
namespace MaxLairInternal{


CaughtScreenActionOption::CaughtScreenActionOption(
    bool take_non_shiny, bool reset_if_high_winrate,
    QString label, CaughtScreenAction default_action
)
    : EnumDropdownOption(
        std::move(label),
        {
            "Stop Program",
            take_non_shiny
                ? "Continue Running. (Take any shiny non-boss " + STRING_POKEMON + " along the way.)"
                : "Continue Running",
            !reset_if_high_winrate
                ? "Reset Game"
                : take_non_shiny
                    ? "Reset Game if win-rate is above the threshold. Otherwise take any non-boss shinies and continue."
                    : "Reset Game if win-rate is above the threshold. Otherwise continue running.",
        },
        (size_t)default_action
    )
{}

CaughtScreenActionsOption::CaughtScreenActionsOption(
    bool host_tooltip, bool winrate_reset_tooltip,
    CaughtScreenAction default_no_shinies,
    CaughtScreenAction default_shiny_nonboss,
    CaughtScreenAction default_shiny_boss
)
    : GroupOption("End Adventure Actions")
    , no_shinies(false, winrate_reset_tooltip, "<b>No Shinies:</b>", default_no_shinies)
    , shiny_nonboss(
        true, winrate_reset_tooltip,
        "<b>Boss is not shiny, but something else is:</b><br>"
        "If this is set to continue and there are multiple shinies, the program will take the highest one on the list.",
        default_shiny_nonboss
    )
    , shiny_boss(
        true, winrate_reset_tooltip,
        "<b>Boss/Legendary is Shiny:</b><br>If there are mulitiple shinies where one is the boss, this option still applies.",
        default_shiny_boss
    )
    , description(
        QString("Choosing \"Reset Game\" has the effect of preserving your balls at the cost of paying ore. (10 ore per reset after enough resets) "
        "Therefore, you should start with plenty of ore to avoid running out.") +
        (
            host_tooltip
                ? "<br>If this is the hosting Switch, \"Reset Game\" also has the effect of preserving the path. "
                "So if you have a path with a high win rate, set both \"No Shinies\" and \"Shiny Non-Boss\" to \"Reset Game\" to grind for the boss."
                : ""
        )
    )
{
    PA_ADD_OPTION(no_shinies);
    PA_ADD_OPTION(shiny_nonboss);
    PA_ADD_OPTION(shiny_boss);
    PA_ADD_STATIC(description);
}





ConsoleSpecificOptions::ConsoleSpecificOptions(QString label, const LanguageSet& languages)
    : GroupOption(std::move(label))
    , is_host_label("<font color=\"blue\" size=4><b>This is the host Switch.</b></font>")
    , language("<b>Game Language:</b>", languages, true)
{
    PA_ADD_STATIC(is_host_label);
    PA_ADD_OPTION(language);
}
void ConsoleSpecificOptions::set_host(bool is_host){
    this->is_host = is_host;
    is_host_label.visibility = is_host ? ConfigOptionState::ENABLED : ConfigOptionState::HIDDEN;
}





Consoles::Consoles(const ConsoleSpecificOptionsFactory& factory)
    : m_languages(PokemonNameReader::instance().languages())
{
    PLAYERS[0] = factory.make("Switch 0 (Top Left)", m_languages);
    PLAYERS[1] = factory.make("Switch 1 (Top Right)", m_languages);
    PLAYERS[2] = factory.make("Switch 2 (Bottom Left)", m_languages);
    PLAYERS[3] = factory.make("Switch 3 (Bottom Right)", m_languages);

    PA_ADD_OPTION(HOST);
    add_option(*PLAYERS[0], "Console0");
    add_option(*PLAYERS[1], "Console1");
    add_option(*PLAYERS[2], "Console2");
    add_option(*PLAYERS[3], "Console3");
}
void Consoles::set_active_consoles(size_t consoles){
    size_t c = 0;
    for (; c < consoles; c++){
        PLAYERS[c]->visibility = ConfigOptionState::ENABLED;
    }
    for (; c < 4; c++){
        PLAYERS[c]->visibility = ConfigOptionState::HIDDEN;
    }
}
ConfigWidget* Consoles::make_ui(QWidget& parent){
    return new ConsolesUI(parent, *this);
}
ConsolesUI::ConsolesUI(QWidget& parent, Consoles& value)
    : BatchWidget(parent, value)
{
    EnumDropdownWidget* host = static_cast<EnumDropdownWidget*>(m_options[0]);
    connect(
        host, &EnumDropdownWidget::on_changed,
        this, [=]{ update_visibility(); }
    );
}
void ConsolesUI::update_visibility(){
    EnumDropdownWidget* host = static_cast<EnumDropdownWidget*>(m_options[0]);
    size_t host_index = static_cast<EnumDropdownOption&>(host->option());
    for (size_t c = 0; c < 4; c++){
        ConfigWidget* console_widget = m_options[c + 1];
        ConsoleSpecificOptions& console = static_cast<ConsoleSpecificOptions&>(console_widget->option());
        console.set_host(c == host_index);
    }
    BatchWidget::update_visibility();
}





}
}
}
}
