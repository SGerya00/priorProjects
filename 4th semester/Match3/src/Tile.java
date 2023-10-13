public class Tile {

    Tile() {
        coords = new Coords();
        colourCode = 0;
        //colour = new ColoursV2();
    }

    Tile(Tile another) {
        coords = new Coords(another.coords);
        colourCode = another.colourCode;
        //colour = new ColoursV2(another.colour);
    }

    Coords coords;
    int colourCode;
}
