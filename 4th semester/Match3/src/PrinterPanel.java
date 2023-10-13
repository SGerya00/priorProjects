import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.concurrent.ArrayBlockingQueue;
import javax.swing.*;
import javax.swing.border.EtchedBorder;
import javax.swing.Timer;

public class PrinterPanel extends JPanel
                          implements ActionListener {

    private final GamePanel parent;

    private final Tile[][] field;

    private final ArrayBlockingQueue<CalculatorQElem> CalculatorQ;
    private final ArrayBlockingQueue<PrinterQElem> PrinterQ;

    private final GameItself game;

    private final int myWidth;
    private final int myHeight;

    private final int tileSize;

    private int whatToPaint;
    // animation constants -----------------------------------------------------------------
    private final int DELETION_ANIMATION = 1;
    private final int SPAWN_ANIMATION = 2;
    private final int SWAP_ANIMATION = 3;
    private final int ON_CLICK = 4;
    private final int ANIMATION_INTERVAL_MS = 25;
    //--------------------------------------------------------------------------------------
    private boolean performingAnimationFlag;
    private void setPerformingAnimationFlag(boolean value) {
        performingAnimationFlag = value;
    }

    private Coords firstClicked;
    private Coords secondClicked;

    //--------------------------------------------------------------------------------------
    private CalculatorQElem cqe = null;
    private Tile[] arrayForPaintingNeeds = null;
    private boolean fieldFlag = false;
    private int executeNextIdx = 0;

    private final Timer timer;

    private void CopyField(Tile[] gotField) {
        for (Tile tile : gotField) {
            field[tile.coords.x][tile.coords.y].coords.x = tile.coords.x;
            field[tile.coords.x][tile.coords.y].coords.y = tile.coords.y;
            field[tile.coords.x][tile.coords.y].colourCode = tile.colourCode;
        }
    }

    private int disX = 0;
    private int disY = 0;
    private int axisMod = 0; //1 - X, -1 - Y
    private int dirMod = 0;
    private boolean shouldStop = false;
    private void PaintSwapInitial() {
        Coords first = forSwapFirst;
        Coords second = forSwapSecond;

        disX = 0;
        disY = 0;
        dirMod = 0;

        int diffX = Math.abs(first.x - second.x);
        int diffY = Math.abs(first.y - second.y);

        if (diffX > diffY) {
            axisMod = 1;
            if (0 > (first.x - second.x)) {
                dirMod = 1;
            } else {
                dirMod = -1;
            }
        } else if (diffX < diffY) {
            axisMod = -1;
            if (0 > (first.y - second.y)) {
                dirMod = 1;
            } else {
                dirMod = -1;
            }
        }
    } //determine swap axis, displacement and direction

    private boolean needToInitialize = true;
    @Override
    public void actionPerformed(ActionEvent av) {
        switch(executeNextIdx) {
            case(-2) : {
                if (needToInitialize) {
                    PrinterQElem pqe = new PrinterQElem(firstClicked, secondClicked);
                    try {
                        PrinterQ.put(pqe);
                    } catch (InterruptedException ie) {
                        System.err.println(ie.getMessage());
                    }
                    needToInitialize = false;
                    forSwapFirst = firstClicked;
                    forSwapSecond = secondClicked;
                    whatToPaint = SWAP_ANIMATION;
                    PaintSwapInitial();
                } else {
                    if (shouldStop) {
                        executeNextIdx += 1;
                        needToInitialize = true;
                        arrayForPaintingNeeds = null;
                        SwapTwo(forSwapFirst, forSwapSecond);
                        parent.getInfoPanel().IncrementNumOfMoves();
                        parent.getInfoPanel().SetMessage("Good one!");
                        firstClicked = null;
                        secondClicked = null;
                        shouldStop = false;
                        whatToPaint = 0;
                        this.repaint();
                    } else {
                        this.repaint();
                    }
                }
                break;
            } //animate tile swap after mouse click and move on to CQ
            case(-1) : {
                try {
                    cqe = CalculatorQ.take();
                } catch (InterruptedException e) {
                    System.err.println(e.getMessage());
                }
                if (cqe.getPoison() == GameItself.POISON) {
                    setPerformingAnimationFlag(false);
                    timer.stop();
                    break;
                }
                if (!fieldFlag) {
                    CopyField(cqe.getGeneratedArray());
                    fieldFlag = true;
                    executeNextIdx = 1;
                } else {
                    executeNextIdx += 1;
                }
                arrayForPaintingNeeds = cqe.getGeneratedArray();
                needToInitialize = true;
                break;
            } //get element from CQ and process it
            case(0) : {
                if (needToInitialize) {
                    needToInitialize = false;
                    decr = 0;
                    whatToPaint = DELETION_ANIMATION;
                } else {
                    if (decr < (tileSize / 2)) {
                        this.repaint();
                    } else {
                        parent.getInfoPanel().ChangeScore(cqe.getScore());
                        executeNextIdx += 1;
                        needToInitialize = true;
                        arrayForPaintingNeeds = cqe.getGeneratedArray();
                    }
                }
                break;
            } //animate deletion
            case(1) : {
                if(needToInitialize) {
                    needToInitialize = false;
                    incr = 0;
                    whatToPaint = SPAWN_ANIMATION;
                    UpdateFieldWithArray(cqe.getGeneratedArray());
                } else {
                    if (incr < (tileSize / 2)) {
                        this.repaint();
                    } else {
                        executeNextIdx += 1;
                        needToInitialize = true;
                        arrayForPaintingNeeds = null;
                    }
                }
                break;
            } //animate spawn
            default : {
                executeNextIdx = -1;
                break;
            } //go back to -1
        }
    }
    //--------------------------------------------------------------------------------------

    // methods for painting ----------------------------------------------------------------
    @Override
    public void paintComponent(Graphics g){
        PaintComponentMain(g);
        switch (whatToPaint) {
            case (DELETION_ANIMATION) -> PaintDeletionWithTimerSub(g);
            case (SPAWN_ANIMATION) -> PaintSpawnWithTimerSub(g);
            case (SWAP_ANIMATION) -> PaintSwapWithTimerSub(g);
            case (ON_CLICK) -> PaintOnFirstClickSub(g);
            default -> {/*nothing*/}
        }
    }

    private void PaintComponentMain(Graphics g) {
        super.paintComponent(g);

        Dimension d = this.getSize();

        int smallHorizontalSide = d.width / GameItself.SIZE;
        int smallVerticalSide = d.height / GameItself.SIZE;

        for (int i = 0; i < GameItself.SIZE; i++) {
            for (int j = 0; j < GameItself.SIZE; j++) {
                g.setColor(game.GetColourByID(field[i][j].colourCode));
                g.fillRect(i * smallHorizontalSide, j * smallVerticalSide,
                        smallHorizontalSide, smallVerticalSide);
            }
        }
    }

    private int incr = 0;
    private void PaintSpawnWithTimerSub(Graphics g) {
        incr++;
        Tile[] generatedArray = arrayForPaintingNeeds;

        g.setColor(game.GetColourByID(0));
        for (Tile generated : generatedArray) {
            g.fillRect(generated.coords.x * tileSize, generated.coords.y * tileSize, tileSize, tileSize);
        } //paint background of deleted tiles
        for (Tile generated : generatedArray) {
            g.setColor(game.GetColourByID(generated.colourCode));
            g.fillRect((generated.coords.x * tileSize) + (tileSize / 2) - incr,
                    (generated.coords.y * tileSize) + (tileSize / 2) - incr,
                    incr * 2, incr * 2);
        } //paint deleted tiles smaller than they were
    }

    private int decr = 0;
    private void PaintDeletionWithTimerSub(Graphics g) {
        decr++;
        Tile[] generatedArray = arrayForPaintingNeeds;

        g.setColor(game.GetColourByID(0));
        for (Tile generated : generatedArray) {
            g.fillRect(generated.coords.x * tileSize, generated.coords.y * tileSize, tileSize, tileSize);
        } //paint background of deleted tiles
        for (Tile generated : generatedArray) {
            g.setColor(game.GetColourByID(field[generated.coords.x][generated.coords.y].colourCode));
            g.fillRect((generated.coords.x * tileSize) + decr, (generated.coords.y * tileSize) + decr,
                    tileSize - (2 * decr), tileSize - (2 * decr));
        } //paint deleted tiles smaller than they were
    }

    private void PaintSwapWithTimerSub(Graphics g) {
        if (axisMod == 1) {
            disX++;
            if ((disX + 1) > (tileSize / 2)) {
                shouldStop = true;
                PaintSwapSub(g, disX * 2, disY * 2, dirMod);
                return;
            }
            PaintSwapSub(g, disX * 2, disY * 2, dirMod);
        } else {
            disY++;
            if ((disY + 1) > (tileSize / 2)) {
                shouldStop = true;
                PaintSwapSub(g, disX * 2, disY * 2, dirMod);
                return;
            }
            PaintSwapSub(g, disX * 2, disY * 2, dirMod);
        }
    }
    private void PaintSwapSub(Graphics g, int displacementX, int displacementY, int directionModifier) {
        Coords first = forSwapFirst;
        Coords second = forSwapSecond;

        g.setColor(game.GetColourByID(0));
        g.fillRect(first.x * tileSize, first.y * tileSize, tileSize, tileSize);
        g.fillRect(second.x * tileSize, second.y * tileSize, tileSize, tileSize);

        g.setColor(game.GetColourByID(field[first.x][first.y].colourCode));
        g.fillRect((first.x * tileSize) + (directionModifier * displacementX),
                (first.y * tileSize) + (directionModifier * displacementY),
                tileSize, tileSize);

        g.setColor(game.GetColourByID(field[second.x][second.y].colourCode));
        g.fillRect((second.x * tileSize) - (directionModifier * displacementX),
                (second.y * tileSize) - (directionModifier * displacementY),
                tileSize, tileSize);
    }

    private void PaintOnFirstClick() {
        whatToPaint = ON_CLICK;
        this.repaint();
    }
    private void PaintOnFirstClickSub(Graphics g) {
        Color colour = game.GetColourByID(field[firstClicked.x][firstClicked.y].colourCode);
        g.setColor(colour.darker());
        g.fillRect(field[firstClicked.x][firstClicked.y].coords.x * tileSize,
                field[firstClicked.x][firstClicked.y].coords.y * tileSize,
                tileSize, tileSize);
    }

    private void PaintOnSecondSameClick() {
        whatToPaint = 0;
        this.repaint();
    }
    //--------------------------------------------------------------------------------------

    // constructor of the MyPanel class ----------------------------------------------------
    PrinterPanel(GamePanel parent, int width, int height, ArrayBlockingQueue<CalculatorQElem> CQ,
                        ArrayBlockingQueue<PrinterQElem> PQ, GameItself g){

        this.parent = parent;

        CalculatorQ = CQ;
        PrinterQ = PQ;

        field = new Tile[GameItself.SIZE][];
        for (int i = 0; i < GameItself.SIZE; i++) {
            field[i] = new Tile[GameItself.SIZE];
            for (int j = 0; j < GameItself.SIZE; j++) {
                field[i][j] = new Tile();
            }
        }
        game = g;

        this.myWidth = width;
        this.myHeight = width;
        this.tileSize = (width / GameItself.SIZE);
        this.whatToPaint = 0;
        this.firstClicked = null;
        this.secondClicked = null;
        this.performingAnimationFlag = false;
        //----------------------------------------------------------
        this.setBackground(Color.GRAY);
        this.setBorder(new EtchedBorder());
        this.setPreferredSize(new Dimension(width, height));
        //----------------------------------------------------------
        MouseAdapter ma = new MyMouseAdapter();
        this.addMouseListener(ma);
        //----------------------------------------------------------
        timer = new Timer(ANIMATION_INTERVAL_MS, this);
        timer.setInitialDelay(0);
    }

    @Override
    public Dimension getPreferredSize() {
        return new Dimension(myWidth, myWidth);
    }

    public void SendPoisonSignal() {
        try {
            PrinterQElem PQE = new PrinterQElem(null, null);
            PQE.MakePoisonous();
            PrinterQ.put(PQE);
        } catch (InterruptedException e) {
            System.err.println(e.getMessage());
        }
    }

    //--------------------------------------------------
    private Coords ConvertFromPixToCoords(int pixx, int pixy) {
        int x = pixx / (myHeight / GameItself.SIZE);
        int y = pixy / (myWidth / GameItself.SIZE);
        return new Coords(x, y);
    }

    // nested class ------------------------------------------------------------------------
    private class MyMouseAdapter extends MouseAdapter {

        public void mouseClicked(MouseEvent e) {
            if (performingAnimationFlag) {
                return;
            }

            Coords coords = ConvertFromPixToCoords(e.getX(), e.getY());
            if (-1 == field[coords.x][coords.y].colourCode) {
                parent.getInfoPanel().SetMessage("Blank spot!");
                return;
            }

            if (null != firstClicked) {
                if ( (Math.abs(coords.x - firstClicked.x)) + (Math.abs(coords.y - firstClicked.y)) >= 2 ) {
                    parent.getInfoPanel().SetMessage("Not adjacent!");
                    return;
                }

                if ((coords.x == firstClicked.x) && (coords.y == firstClicked.y)) {
                    firstClicked = null;
                    setPerformingAnimationFlag(true);
                    PaintOnSecondSameClick();
                    setPerformingAnimationFlag(false);
                } else {
                    secondClicked = coords;
                    setPerformingAnimationFlag(true);
                    executeNextIdx = -2;
                    timer.start();
                }
            } else {
                firstClicked = coords;
                setPerformingAnimationFlag(true);
                PaintOnFirstClick();
                setPerformingAnimationFlag(false);
            }
        }

    }
    //--------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------
    public void InitialAnimation() {
        setPerformingAnimationFlag(true);
        executeNextIdx = -1;
        timer.start();
    }

    private void UpdateFieldWithArray(Tile[] array) {
        for (Tile tile : array) {
            field[tile.coords.x][tile.coords.y].colourCode = tile.colourCode;
        }
    }

    private Coords forSwapFirst = null;
    private Coords forSwapSecond = null;
    private void SwapTwo(Coords first, Coords second) {

        //swap colourCodes
        int colourCodeTemp = field[first.x][first.y].colourCode;
        field[first.x][first.y].colourCode = field[second.x][second.y].colourCode;
        field[second.x][second.y].colourCode = colourCodeTemp;
    }

}