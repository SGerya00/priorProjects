public class PrinterQElem {

    PrinterQElem(Coords f, Coords s) {
        first = f;
        second = s;
    }
    public void MakePoisonous() {
        poison = GameItself.POISON;
    }

    private final Coords first;
    Coords getFirst() {
        return first;
    }
    private final Coords second;
    Coords getSecond() {
        return second;
    }
    private int poison = 0;
    int getPoison() {
        return poison;
    }
}
