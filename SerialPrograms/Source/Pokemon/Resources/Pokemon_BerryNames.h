/*  Pokemon Berry Names
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_Pokemon_BerryNames_H
#define PokemonAutomation_Pokemon_BerryNames_H

#include <string>
#include <vector>
#include <map>
#include <QString>

namespace PokemonAutomation{
namespace Pokemon{


class BerryNames{
public:
    const QString& display_name() const{ return m_display_name; }

private:
    friend struct BerryNameDatabase;

    QString m_display_name;
};


const BerryNames& get_berry_name(const std::string& slug);
const std::string& parse_berry_name(const QString& display_name);
const std::string& parse_berry_name_nothrow(const QString& display_name);

const std::vector<std::string>& BERRY_SLUGS();


}
}
#endif
