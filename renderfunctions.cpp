#include "renderfunctions.h"


QColor BotColor(Bot bot){
    int energy = bot.energy;
    return QColor(energy, energy, energy);
}
