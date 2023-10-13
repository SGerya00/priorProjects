public class CalculatorQElem {

    CalculatorQElem(Tile[] array, long scr, long moves) {
        generatedArray = array;
        score = scr;
        n_o_moves = moves;
    }
    void MakePoisonous() {
        poison = GameItself.POISON;
    }

    private final Tile[] generatedArray;
    Tile[] getGeneratedArray() {
        return generatedArray;
    }

    private final long score;
    long getScore() {
        return score;
    }
    private final long n_o_moves;
    long getN_o_moves() {
        return n_o_moves;
    }

    private int poison = 0;
    int getPoison() {
        return poison;
    }

}
