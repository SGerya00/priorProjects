import javax.swing.*;
import java.awt.*;

public class GamePanel extends JPanel {

    private final MainPanel parent;
    public MainPanel getParent() {
        return parent;
    }

    private final PrinterPanel pp;
    private final InfoPanel ip;
    private final GameItself game;
    private Thread calculatorThread;

    private int constraint = 0;
    public void setConstraint(int x) {
        constraint = x;
    }

    GamePanel(MainPanel parent) {
        this.setBackground(Color.GRAY);
        this.setPreferredSize(new Dimension(600, 500));
        this.parent = parent;

        this.game = new GameItself();

        pp = new PrinterPanel(this, 500, 500, game.getCQ(), game.getPQ(), game);
        ip = new InfoPanel(this, 100, 500);

        this.setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
        this.add(pp);
        this.add(ip);
    }

    public void Start() {
        game.setCalcConstraint(constraint);
        //---------------------------
        calculatorThread = new Thread(game.getCalc());
        calculatorThread.start();
        //---------------------------
        pp.InitialAnimation();
    }

    public void End() {
        try {
            calculatorThread.join();
        } catch (InterruptedException e) {
            System.err.println(e.getMessage());
        }
    }

    public PrinterPanel getInteractivePanel(){
        return pp;
    }

    public InfoPanel getInfoPanel() {
        return ip;
    }

}
