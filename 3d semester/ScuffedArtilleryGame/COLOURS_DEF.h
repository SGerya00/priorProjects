#pragma once
#ifndef COLOURS_DEF_H
#define COLOURS_DEF_H

//for basegrid
#define GRIDCOLOUR (QColor(107,142,35)) //colour of background(battlefield)
#define MISSCOLOUR (Qt::black) //colour of a square that was hit and did not hold artillery units
#define HITCOLOUR (Qt::red) //colour of a square that was hit and had an artillery unit
#define ONENDRED (QColor(150, 0, 0, 100)) //colour "tint" used to paint a widget at game's end to indicate defeat
#define ONENDGREEN (QColor(110, 255, 0, 100)) //colour "tint" used to paint a widget at game's end to indicate victory
//
//for yourgrid
#define ARTCOLOUR (Qt::blue) //colour of artillery units on left("your") widget
//
//for foesgrid
#define HIT_MARKER_COLOUR (QColor(255,140,0)) //colour of a hit marker on right(foe's) widget
//

#endif // COLOURS_DEF_H
