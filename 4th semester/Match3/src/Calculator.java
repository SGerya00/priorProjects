import java.util.Random;
import java.util.Vector;
import java.util.concurrent.ArrayBlockingQueue;

public class Calculator implements Runnable {

    Calculator(ArrayBlockingQueue<CalculatorQElem> CQ, ArrayBlockingQueue<PrinterQElem> PQ) {
        CalculatorQ = CQ;
        PrinterQ = PQ;

        field = new Tile[GameItself.SIZE][];
        for (int i = 0; i < GameItself.SIZE; i++) {
            field[i] = new Tile[GameItself.SIZE];
            for (int j = 0; j < GameItself.SIZE; j++) {
                field[i][j] = new Tile();
            }
        }

        int seed = 15; //change later to whatever you want
        RNG = new Random(seed);
        totalScore = 0;
        n_o_moves = 0;
    }

    @Override
    public void run() {
        GenerateField();
        ConstrainField();
        Tile[] arr = MakeFieldArrayForGUI();
        try {
            PutInCQ(arr);
        } catch (InterruptedException e) {
            System.err.println(e.getMessage());
        }
        DoMoveSub();
        try {
            PutPoisonInCQ();
        } catch (InterruptedException e) {
            System.err.println(e.getMessage());
        }

        PrinterQElem pqe;
        while(true) {
            try {
                pqe = PrinterQ.take();
                if (pqe.getPoison() == GameItself.POISON) {
                    break;
                }
                DoMove(pqe.getFirst(), pqe.getSecond());
                PutPoisonInCQ();
                //
                //Thread.currentThread().wait(1000);
                //Thread.sleep(10000); ---------------------------------------------------------------------------------
                //
            } catch (InterruptedException e) {
                System.err.println(e.getMessage());
            }
        }
    }

    private void GenerateField() {
        for (int i = 0; i < GameItself.SIZE; i++) {
            for (int j = 0; j < GameItself.SIZE; j++) {
                field[i][j].coords.x = i;
                field[i][j].coords.y = j;
                field[i][j].colourCode = GenerateColourCode();
            }
        }
    }

    private void ConstrainField() {
        switch(constraintCode) {
            case(1) : {
                field[0][0].colourCode = -1;
                field[1][0].colourCode = -1;
                field[0][1].colourCode = -1;

                field[9][9].colourCode = -1;
                field[8][9].colourCode = -1;
                field[9][8].colourCode = -1;

                field[0][9].colourCode = -1;
                field[1][9].colourCode = -1;
                field[0][8].colourCode = -1;

                field[9][0].colourCode = -1;
                field[8][0].colourCode = -1;
                field[9][1].colourCode = -1;

                field[4][4].colourCode = -1;
                field[4][5].colourCode = -1;
                field[5][4].colourCode = -1;
                field[5][5].colourCode = -1;

                break;
            }
            case(2) : {
                for (int i = 0; i < GameItself.SIZE; i++) {
                    field[i][2].colourCode = -1;
                    field[i][5].colourCode = -1;
                    field[i][8].colourCode = -1;

                    field[2][i].colourCode = -1;
                    field[5][i].colourCode = -1;
                    field[8][i].colourCode = -1;
                }
                break;
            }
            default : {break;}
        }
    }
    public void setConstraintCode(int constraintCode) {
        this.constraintCode = constraintCode;
    }

    private int GenerateColourCode() {
        return (RNG.nextInt(GameItself.N_O_USUAL_COLOURS) + 1);
    }

    private Tile[] MakeFieldArrayForGUI() {
        Tile[] arrayField = new Tile[GameItself.SIZE * GameItself.SIZE];
        for (int i = 0; i < GameItself.SIZE; i++) {
            for (int j = 0; j < GameItself.SIZE; j++) {
                arrayField[(j * GameItself.SIZE) + i] = new Tile();
                arrayField[(j * GameItself.SIZE) + i].coords.x = field[i][j].coords.x;
                arrayField[(j * GameItself.SIZE) + i].coords.y = field[i][j].coords.y;
                arrayField[(j * GameItself.SIZE) + i].colourCode = field[i][j].colourCode;
            }
        }
        return arrayField;
    }

    private void DoMoveSub() {
        while(true) {
            boolean result = checkIfPoppable();
            if (!result) {
                break;
            }
            Tile[] poppedArray = FindAndPop();
            totalScore += (2 * ((long)(poppedArray.length)) - 3);
            Tile[] generatedArray = GenerateNewTiles(poppedArray);
            //add to queue
            try {
                PutInCQ(generatedArray);
            } catch (InterruptedException e) {
                System.err.println(e.getMessage());
            }
        }
    }

    private void DoMove(Coords first, Coords second) {
        int colourCodeTemp = field[first.x][first.y].colourCode;
        field[first.x][first.y].colourCode = field[second.x][second.y].colourCode;
        field[second.x][second.y].colourCode = colourCodeTemp;
        n_o_moves += 1;

        DoMoveSub();
    }

    // scan field --------------------------------------------------------------------------
    private boolean checkIfPoppable() {
        for (int i = 0; i < GameItself.SIZE; i++) {
            for (int j = 0; j < GameItself.SIZE; j++) {
                boolean result1 = checkRight(i, j);
                boolean result2 = checkDown(i, j);
                if (result1 || result2) {
                    return true;
                }
            }
        }
        return false;
    }

    private boolean checkRight(int x, int y) {
        boolean result = false;
        int currentColourCode = field[x][y].colourCode;
        if (-1 == currentColourCode) {
            return result;
        }
        if (x < (GameItself.SIZE - 2)) {
            if ((field[x + 1][y].colourCode == currentColourCode) &&
                (field[x + 2][y].colourCode == currentColourCode)) {
                result = true;
            }
        }
        return result;
    }

    private boolean checkDown(int x, int y) {
        boolean result = false;
        int currentColourCode = field[x][y].colourCode;
        if (-1 == currentColourCode) {
            return result;
        }
        if (y < (GameItself.SIZE - 2)) {
            if ((field[x][y + 1].colourCode == currentColourCode) &&
                    (field[x][y + 2].colourCode == currentColourCode)) {
                result = true;
            }
        }
        return result;
    }
    //--------------------------------------------------------------------------------------
    private Tile[] FindAndPop() {
        Vector<Tile> destroyedTilesVector = new Vector<Tile>(); //houses references to field tiles
        //go right
        for (int i = 0; i < GameItself.SIZE; i++) { //row
            for (int j = 0; j < GameItself.SIZE; /* j += 0; */) { //column
                int currentColourCode = field[j][i].colourCode;
                if (-1 == currentColourCode) {
                    j += 1;
                    continue;
                }
                int inARowCounter = 1;
                for (int k = (j + 1); k < GameItself.SIZE; k++) {
                    if (field[k][i].colourCode == currentColourCode) {
                        inARowCounter += 1;
                    } else {
                        break;
                    }
                }
                if (3 <= inARowCounter) {
                    for (int k = 0; k < inARowCounter; k++) {
                        destroyedTilesVector.add(field[j + k][i]);
                    }
                }
                j += inARowCounter;
            }
        }
        //go down
        for (int i = 0; i < GameItself.SIZE; i++) { //column
            for (int j = 0; j < GameItself.SIZE; /* j += 0; */) { //row
                int currentColourCode = field[i][j].colourCode;
                if (-1 == currentColourCode) {
                    j += 1;
                    continue;
                }
                int inAColumnCounter = 1;
                for (int k = (j + 1); k < GameItself.SIZE; k++) {
                    if (field[i][k].colourCode == currentColourCode) {
                        inAColumnCounter += 1;
                    } else {
                        break;
                    }
                }
                if (3 <= inAColumnCounter) {
                    for (int k = 0; k < inAColumnCounter; k++) {
                        destroyedTilesVector.add(field[i][j + k]);
                        //added tiles may have already been added, but that is acceptable
                    }
                }
                j += inAColumnCounter;
            }
        }
        //copy to array and make colourCode-s 0
        Tile[] arrayToDelete = new Tile[destroyedTilesVector.size()];
        for (int i = 0; i < destroyedTilesVector.size(); i++) {
            arrayToDelete[i] = new Tile(destroyedTilesVector.elementAt(i));
        }

        return arrayToDelete;
    }

    private Tile[] GenerateNewTiles(Tile[] poppedArray) {
        for (Tile tile : poppedArray) {
            tile.colourCode = GenerateColourCode();
            field[tile.coords.x][tile.coords.y].colourCode = tile.colourCode;
        }
        return poppedArray;
    } //returns ref to the same array, modified
    //--------------------------------------------------------------------------------------

    private void PutInCQ(Tile[] generatedArray) throws InterruptedException {
        CalculatorQ.put(new CalculatorQElem(generatedArray, totalScore, n_o_moves));
    }

    private void PutPoisonInCQ() throws InterruptedException {
        CalculatorQElem cqe = new CalculatorQElem(null, totalScore, n_o_moves);
        cqe.MakePoisonous();
        CalculatorQ.put(cqe);
    }

    private final Random RNG;
    private int constraintCode;

    private final Tile[][] field; //field[X][Y]
    private long totalScore;
    private long n_o_moves;

    private final ArrayBlockingQueue<CalculatorQElem> CalculatorQ;
    private final ArrayBlockingQueue<PrinterQElem> PrinterQ;
}
