public class Coords {

    Coords() {
        x = 0;
        y = 0;
    }

    Coords(Coords another) {
        x = another.x;
        y = another.y;
    }

    Coords(int x, int y) {
        this.x = x;
        this.y = y;
    }

    int x;
    int y;
}
