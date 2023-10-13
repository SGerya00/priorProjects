import java.awt.*;
import java.util.HashMap;
import java.util.concurrent.ArrayBlockingQueue;

public class GameItself {

    GameItself() {
        CodesAndColours = new HashMap<Integer, Color>();
        InitialiseCodesAndColours();

        CQ = new ArrayBlockingQueue<CalculatorQElem>(Q_MAX_SIZE);
        PQ = new ArrayBlockingQueue<PrinterQElem>(Q_MAX_SIZE);

        calc = new Calculator(CQ, PQ);
    }

    public void setCalcConstraint(int constraint) {
        calc.setConstraintCode(constraint);
    }

    private void InitialiseCodesAndColours() {
        CodesAndColours.clear();
        CodesAndColours.put(0, Color.DARK_GRAY);
        CodesAndColours.put(1, Color.CYAN);
        CodesAndColours.put(2, Color.GREEN);
        CodesAndColours.put(3, Color.MAGENTA);
        CodesAndColours.put(4, Color.ORANGE);

        CodesAndColours.put(-1, Color.BLACK);
    }

    public Color GetColourByID(int ID) {
        return CodesAndColours.get(ID);
    }

    private final Calculator calc;
    Calculator getCalc() {
        return calc;
    }

    private final ArrayBlockingQueue<CalculatorQElem> CQ;
    ArrayBlockingQueue<CalculatorQElem> getCQ() {
        return CQ;
    }
    private final ArrayBlockingQueue<PrinterQElem> PQ;
    ArrayBlockingQueue<PrinterQElem> getPQ() {
        return PQ;
    }

    private final HashMap<Integer, Color> CodesAndColours;

    // game defining constants -------------------------------------------------------------
    static final int N_O_USUAL_COLOURS = 4;
    static final int SIZE = 10;
    // other constants ---------------------------------------------------------------------
    static final int Q_MAX_SIZE = 16;
    static final int POISON = -1;

}
